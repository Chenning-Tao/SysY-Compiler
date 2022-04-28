//
// Created by tcn on 22-4-19.
//

#include "gen.hpp"

using namespace llvm;

void gen::ProgramGen(unique_ptr<CompUnit> &program) {
    // init external function
    InitExternalFunction();
    for (auto & Unit : program->CompUnits) {
        if (Unit->AST_type == FUNC) FuncGen(Unit);
        else if (Unit->AST_type == DECL) GlobalVarGen(Unit);
    }
    GenModule->print(outs(), nullptr);
    OutputGen();
}

void gen::InitExternalFunction() {
    // int getint()
    FunctionType *getintTy = FunctionType::get(GetFuncType(Int), false);
    Function *getint = Function::Create(getintTy, Function::ExternalLinkage, "getint", GenModule.get());
    // void putint(a)
    std::vector<Type *> putintParas(1, GetFuncType(Int));
    FunctionType *putintTy = FunctionType::get(GetFuncType(Void), putintParas, false);
    Function *putint = Function::Create(putintTy, Function::ExternalLinkage, "putint", GenModule.get());
}

void gen::OutputGen() {
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = sys::getDefaultTargetTriple();
    GenModule->setTargetTriple(TargetTriple);

    string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        exit(0);
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine =
            Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    GenModule->setDataLayout(TheTargetMachine->createDataLayout());

    auto Filename = "output.o";
    error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        exit(0);
    }

    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        exit(0);
    }

    pass.run(*GenModule);
    dest.flush();

    outs() << "Wrote " << Filename << "\n";
}

void gen::GlobalVarGen(unique_ptr<BaseAST> &Unit) {
    unique_ptr<Decl> global(reinterpret_cast<Decl*>(Unit.release()));
    // determine whether it has been declared
    if (GlobalValues.find(global->Var_name) == GlobalValues.end()) {
        GlobalVariable *gVar = createGlob(GetFuncType(global->Decl_type), global->Var_name);
        if (global->Exp != nullptr)
            gVar->setInitializer(dyn_cast<Constant>(ValueGen(global->Exp)));
        GlobalValues.emplace(global->Var_name, gVar);
    }
    else cout << "error: redefinition of '"<< global->Var_name << "'" << endl;
}

void gen::FuncGen(unique_ptr<BaseAST> &Unit) {
    unique_ptr<Func> FuncUnit(reinterpret_cast<Func*>(Unit.release()));
    // Parameters
    vector<Type*> Para{};

    // Function
    // FunctionType *FT = FunctionType::get(Type::getVoidTy(GenContext), Para,false);
    // std::vector<Type *> Doubles(2, Type::getDoubleTy(*GenContext));
    unique_ptr<FuncPrototype> Proto(reinterpret_cast<FuncPrototype*>(FuncUnit->Prototype.release()));
    FunctionType *FT = FunctionType::get(GetFuncType(Proto->Func_type), Para, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Proto->Func_name, GenModule.get());

    // create block
    BasicBlock *BB = createBB(F, Proto->Func_name);
    GenBuilder->SetInsertPoint(BB);
    std::vector<std::string> removeList;
    for (auto & Block : FuncUnit->Blocks){
        if (Block->AST_type == STMT) StmtGen(F, Block);
        else if(Block->AST_type == DECL) DeclGen(Block, removeList);
        else if(Block->AST_type == FUNCPROTO) FuncCallGen(Block);
    }
    NamedValues.remove(removeList);
}

void gen::DeclGen(unique_ptr<BaseAST> &Block, vector<std::string> &removeList) {
    unique_ptr<Decl> DeclUnit(reinterpret_cast<Decl*>(Block.release()));
    auto *cur = GenBuilder->GetInsertBlock();
    // check redefinition
    auto check_sym = find(removeList.begin(), removeList.end(), DeclUnit->Var_name);
    if (check_sym != removeList.end()) {
        cout << "error: redefinition of '"<< DeclUnit->Var_name << "'" << endl;
        exit(0);
    }
    Value *InitVal = nullptr;
    if (DeclUnit->Exp != nullptr) {
        InitVal = ValueGen(DeclUnit->Exp);
        if (DeclUnit->Decl_type == Float && InitVal->getType()->isIntegerTy())
            InitVal = IntToFloat(InitVal);
    }
    // create IR
    AllocaInst *Alloca = createBlockAlloca(*cur, DeclUnit->Var_name, DeclUnit->Decl_type);
    if (InitVal != nullptr) GenBuilder->CreateStore(InitVal, Alloca);
    // add to symbol table
    NamedValues.insert(DeclUnit->Var_name, Alloca);
    removeList.push_back(DeclUnit->Var_name);
}

void gen::StmtGen(Function *F, unique_ptr<BaseAST> &Block) {
    unique_ptr<Stmt> StmtUnit(reinterpret_cast<Stmt*>(Block.release()));
    if (StmtUnit->Stmt_type == If) IfGen(F, StmtUnit);
    else if(StmtUnit->Stmt_type == Assign) AssignGen(StmtUnit);
    else if(StmtUnit->Stmt_type == Return){
        if(StmtUnit->RVal == nullptr) GenBuilder->CreateRetVoid();
        else GenBuilder->CreateRet(ValueGen(StmtUnit->RVal));
    }
}

void gen::AssignGen(unique_ptr<Stmt> &StmtUnit) {
    unique_ptr<Variable> VarUnit(reinterpret_cast<Variable*>(StmtUnit->LVal.release()));
    // check symbol table
    AllocaInst *var = NamedValues.find(VarUnit->Var_name);
    if (var == nullptr) {
        cout << "error: use of undeclared identifier '" << VarUnit->Var_name << "'" << endl;
        exit(0);
    }
    // type conversion
    Value *right = ValueGen(StmtUnit->RVal);
    if (var->getAllocatedType() != right->getType()){
        if (var->getAllocatedType()->isIntegerTy()) right = FloatToInt(right);
        else if (var->getAllocatedType()->isFloatTy()) right = IntToFloat(right);
    }
    GenBuilder->CreateStore(right, var);
}

void gen::IfGen(Function *F, unique_ptr<Stmt> &StmtUnit) {
    BasicBlock *ThenBB = createBB(F, "then");
    BasicBlock *ElseBB = createBB(F, "else");
    BasicBlock *MergeBB = createBB(F, "ifcond");

    // condition generation
    Value *cond = ConditionGen(StmtUnit->Condition);
    GenBuilder->CreateCondBr(cond, ThenBB, ElseBB);

    // condition = true
    GenBuilder->SetInsertPoint(ThenBB);
    std::vector<std::string> removeList;
    for(auto &true_block : StmtUnit->First_block){
        if(true_block->AST_type == DECL) DeclGen(true_block, removeList);
        else if(true_block->AST_type == STMT) StmtGen(F, true_block);
    }
    NamedValues.remove(removeList);
    GenBuilder->CreateBr(MergeBB);

    // condition = false
    GenBuilder->SetInsertPoint(ElseBB);
    GenBuilder->CreateBr(MergeBB);

    // merge
//    PHINode *Phi = GenBuilder->CreatePHI()
}

Value *gen::IntToFloat(Value *InitVal) {
    InitVal = GenBuilder->CreateSIToFP(InitVal, Type::getFloatTy(*GenContext));
    return InitVal;
}

Value *gen::FloatToInt(Value *InitVal) {
    InitVal = GenBuilder->CreateFPToSI(InitVal, Type::getInt32Ty(*GenContext));
    return InitVal;
}

gen::gen(const string& name) {
    GenContext = std::make_unique<LLVMContext>();
    GenModule = std::make_unique<Module>(name, *GenContext);
    GenBuilder = std::make_unique<IRBuilder<>>(*GenContext);
}

Type *gen::GetFuncType(type FuncType) {
    switch (FuncType) {
        case Int:
            return Type::getInt32Ty(*GenContext);
        case Float:
            return Type::getFloatTy(*GenContext);
        case Void:
            return Type::getVoidTy(*GenContext);
    }
}

BasicBlock *gen::createBB(Function *fooFunc, const string &Name) {
    return BasicBlock::Create(*GenContext, Name, fooFunc);
}

Value *gen::ConditionGen(unique_ptr<BaseAST> &input) {
    unique_ptr<Exp> condition(reinterpret_cast<Exp*>(input.release()));
    Value *L = ValueGen(condition->Left_exp);
    Value *R = ValueGen(condition->Right_exp);
    bool is_float = FloatGen(L, R);
    if (condition->Operator == "==") {
        if (is_float) return GenBuilder->CreateFCmpOEQ(L, R, "ifcond");
        else return GenBuilder->CreateICmpEQ(L, R, "ifcond");
    }
    // TODO: verify
    else if (condition->Operator == "!="){
        if (is_float) return GenBuilder->CreateFCmpONE(L, R, "ifcond");
        else return GenBuilder->CreateICmpNE(L, R, "ifcond");
    }
}

Value *gen::ValueGen(unique_ptr<BaseAST> &input) {
    if (input->AST_type == FINALEXP) {
        unique_ptr<FinalExp> expression(reinterpret_cast<FinalExp*>(input.release()));
        if(expression->Exp_type == Float) return ConstantFP::get(*GenContext, APFloat(expression->Number));
        else if(expression->Exp_type == Int) return ConstantInt::get(*GenContext, APInt(32, int(expression->Number)));
    }
    else if (input->AST_type == VARIABLE){
        unique_ptr<Variable> variable(reinterpret_cast<Variable*>(input.release()));
        return LoadValue(variable->Var_name);
    }
    else if (input->AST_type == FUNCPROTO) return FuncCallGen(input);
    else if (input->AST_type == EXP) {
        unique_ptr<Exp> expression(reinterpret_cast<Exp*>(input.release()));
        Value *L = ValueGen(expression->Left_exp);
        // fake EXP node
        if (expression->Operator.empty()) return L;
        Value *R = ValueGen(expression->Right_exp);
        bool is_float = FloatGen(L, R);

        // expression generation
        if(expression->Operator == "+"){
            if (is_float) return GenBuilder->CreateFAdd(L, R);
            else return GenBuilder->CreateAdd(L, R);
        }
        else if(expression->Operator == "-"){
            if (is_float) return GenBuilder->CreateFSub(L, R);
            else return GenBuilder->CreateSub(L, R);
        }
        else if(expression->Operator == "*"){
            if (is_float) return GenBuilder->CreateFMul(L, R);
            else return GenBuilder->CreateMul(L, R);
        }
        else if(expression->Operator == "/"){
            if(L->getType()->isFloatTy()) L = GenBuilder->CreateSIToFP(L, Type::getFloatTy(*GenContext));
            if(R->getType()->isFloatTy()) R = GenBuilder->CreateSIToFP(R, Type::getFloatTy(*GenContext));
            return GenBuilder->CreateFDiv(L, R);
        }
    }
}

Value *gen::FuncCallGen(unique_ptr<BaseAST> &input) {
    unique_ptr<FuncPrototype> prototype(reinterpret_cast<FuncPrototype*>(input.release()));
    Function *CalleeF = GenModule->getFunction(prototype->Func_name);
    if (CalleeF == nullptr) {
        cout << "error: use of undeclared function '" << prototype->Func_name << "'" << endl;
        exit(0);
    }
    vector<Value*> ArgsV{};
    if (!prototype->Params.empty()){
        for(auto & Param : prototype->Params)
            ArgsV.push_back(ValueGen(Param));
    }
    return GenBuilder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Value *gen::LoadValue(const string &temp_name) {// check symbol table
    AllocaInst *var = NamedValues.find(temp_name);
    if (var == nullptr) {
        cout << "error: use of undeclared identifier '" << temp_name << "'" << endl;
        exit(0);
    }
    return GenBuilder->CreateLoad(var->getAllocatedType(), var, var->getName().data());
}

bool gen::FloatGen(Value *&L, Value *&R) {
    bool is_float= !(L->getType()->isIntegerTy() && R->getType()->isIntegerTy());
    if (is_float){
        if(L->getType()->isIntegerTy()) L = IntToFloat(L);
        if(R->getType()->isIntegerTy()) R = IntToFloat(R);
    }
    return is_float;
}

GlobalVariable *gen::createGlob(Type *type, const std::string& name) {
    GenModule->getOrInsertGlobal(name, type);
    GlobalVariable *gVar = GenModule->getNamedGlobal(name);
    return gVar;
}

AllocaInst *gen::createBlockAlloca(BasicBlock &block, const string &VarName, type VarType) {
    IRBuilder<> TmpB(&block, block.begin());
    return TmpB.CreateAlloca(GetFuncType(VarType), nullptr, VarName);
}