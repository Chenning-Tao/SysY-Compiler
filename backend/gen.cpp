//
// Created by tcn on 22-4-19.
//

#include "gen.hpp"

using namespace llvm;

void gen::ProgramGen(shared_ptr<CompUnit> &program) {\
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
    FunctionType *getintTy = FunctionType::get(GetType(Int), false);
    Function *getint = Function::Create(getintTy, Function::ExternalLinkage, "getint", GenModule.get());
    // void putint(a)
    std::vector<Type *> putintParas(1, GetType(Int));
    FunctionType *putintTy = FunctionType::get(GetType(Void), putintParas, false);
    Function *putint = Function::Create(putintTy, Function::ExternalLinkage, "putint", GenModule.get());
    // int getch()
    FunctionType *getchTy = FunctionType::get(GetType(Int), false);
    Function *getch = Function::Create(getchTy, Function::ExternalLinkage, "getch", GenModule.get());
    // int getfloat()
    FunctionType *getfloatTy = FunctionType::get(GetType(Float), false);
    Function *getfloat = Function::Create(getfloatTy, Function::ExternalLinkage, "getfloat", GenModule.get());
    // void putch(a)
    std::vector<Type *> putchParas(1, GetType(Int));
    FunctionType *putchTy = FunctionType::get(GetType(Void), putchParas, false);
    Function *putch = Function::Create(putchTy, Function::ExternalLinkage, "putch", GenModule.get());
    // void putfloat(a)
    std::vector<Type *> putfloatParas(1, GetType(Float));
    FunctionType *putfloatTy = FunctionType::get(GetType(Void), putfloatParas, false);
    Function *putfloat = Function::Create(putfloatTy, Function::ExternalLinkage, "putfloat", GenModule.get());
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

void gen::GlobalVarGen(shared_ptr<BaseAST> &Unit) {
    shared_ptr<Decl> global(reinterpret_pointer_cast<Decl>(Unit));
    // determine whether it has been declared
    // TODO: add array
    shared_ptr<Variable> var(reinterpret_pointer_cast<Variable>(global->Var));
    if (GlobalValues.find(var->Var_name) == GlobalValues.end()) {
        GlobalVariable *gVar = createGlob(GetType(global->Decl_type), var->Var_name);
        if (global->Exp != nullptr)
            gVar->setInitializer(dyn_cast<Constant>(ValueGen(global->Exp)));
        GlobalValues.emplace(var->Var_name, gVar);
    }
    else cout << "error: redefinition of '"<< var->Var_name << "'" << endl;
}

void gen::FuncGen(shared_ptr<BaseAST> &Unit) {
    shared_ptr<Func> FuncUnit(reinterpret_pointer_cast<Func>(Unit));
    shared_ptr<FuncPrototype> Proto(reinterpret_pointer_cast<FuncPrototype>(FuncUnit->Prototype));
    // Parameters
    vector<Type*> Para{};
    vector<std::string> ParaNames{};
    vector<int> ParaDim{};
    vector<type> ParaType{};
    for (auto & Param : Proto->Params){
        shared_ptr<Decl> decl(reinterpret_pointer_cast<Decl>(Param));
        shared_ptr<Variable> var(reinterpret_pointer_cast<Variable>(decl->Var));
        ParaNames.push_back(var->Var_name);
        if (var->Length.empty()) Para.push_back(GetType(decl->Decl_type));
        else Para.push_back(GetPtrType(decl->Decl_type));
        ParaDim.push_back(var->Length.size());
        ParaType.push_back(decl->Decl_type);
    }

    // Function
    FunctionType *FT = FunctionType::get(GetType(Proto->Func_type), Para, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Proto->Func_name, GenModule.get());

    // create block
    BasicBlock *BB = createBB(F, Proto->Func_name);
    GenBuilder->SetInsertPoint(BB);

    // init symbol table
    auto *cur = GenBuilder->GetInsertBlock();
    int i = 0;
    for(auto &Arg : F->args()) {
        Value *a = &Arg;
        AllocaInst *Alloca;
        if (ParaDim[i] == 0) Alloca = createBlockAlloca(*cur, ParaNames[i], ParaType[i]);
        else Alloca = createBlockPtrAlloca(*cur, ParaNames[i], ParaType[i]);
        NamedValues.insert(ParaNames[i], Alloca, ParaDim[i]);
        GenBuilder->CreateStore(a, Alloca);
        ++i;
    }

    std::vector<std::string> removeList;
    for (auto & Block : FuncUnit->Blocks){
        if (Block->AST_type == STMT) StmtGen(F, Block);
        else if(Block->AST_type == DECL) DeclGen(Block, removeList);
        else if(Block->AST_type == FUNCPROTO) FuncCallGen(Block);
    }
    NamedValues.remove(removeList);
}

void gen::DeclGen(shared_ptr<BaseAST> &Block, vector<std::string> &removeList) {
    shared_ptr<Decl> DeclUnit(reinterpret_pointer_cast<Decl>(Block));
    shared_ptr<Variable> VarUnit(reinterpret_pointer_cast<Variable>(DeclUnit->Var));
    auto *cur = GenBuilder->GetInsertBlock();
    // check redefinition
    auto check_sym = find(removeList.begin(), removeList.end(), VarUnit->Var_name);
    if (check_sym != removeList.end()) {
        cout << "error: redefinition of '"<< VarUnit->Var_name << "'" << endl;
        exit(0);
    }
    Value *InitVal = nullptr;
    if (DeclUnit->Exp != nullptr) {
        InitVal = ValueGen(DeclUnit->Exp);
        if (DeclUnit->Decl_type == Float && InitVal->getType()->isIntegerTy())
            InitVal = IntToFloat(InitVal);
    }
    // create IR
    AllocaInst *Alloca;
    if (VarUnit->Length.empty()) Alloca = createBlockAlloca(*cur, VarUnit->Var_name, DeclUnit->Decl_type);
    else {
        // TODO: add multi dim array
        Value *arraySize = ValueGen(VarUnit->Length[0]);
        Alloca = createBlockAlloca(*cur, VarUnit->Var_name, DeclUnit->Decl_type, arraySize);
    }
    if (InitVal != nullptr) GenBuilder->CreateStore(InitVal, Alloca);
    // add to symbol table
    NamedValues.insert(VarUnit->Var_name, Alloca, VarUnit->Length.size());
    removeList.push_back(VarUnit->Var_name);
}

void gen::StmtGen(Function *F, shared_ptr<BaseAST> &Block) {
    shared_ptr<Stmt> StmtUnit(reinterpret_pointer_cast<Stmt>(Block));
    if (StmtUnit->Stmt_type == If) IfGen(F, StmtUnit);
    else if (StmtUnit->Stmt_type == While) WhileGen(F, StmtUnit);
    else if(StmtUnit->Stmt_type == Assign) AssignGen(StmtUnit);
    else if(StmtUnit->Stmt_type == Return){
        if(StmtUnit->RVal == nullptr) GenBuilder->CreateRetVoid();
        else GenBuilder->CreateRet(ValueGen(StmtUnit->RVal));
    }
}

void gen::AssignGen(shared_ptr<Stmt> &StmtUnit) {
    shared_ptr<Variable> VarUnit(reinterpret_pointer_cast<Variable>(StmtUnit->LVal));
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
    if (VarUnit->Length.empty()) GenBuilder->CreateStore(right, var);
    else {
        // if is array
        Value *location = GetLocation(VarUnit, var);
        GenBuilder->CreateStore(right, location);
    }
}

Value *gen::GetLocation(const shared_ptr<Variable> &VarUnit, AllocaInst *var) {
    Value *location;
    if (var->getAllocatedType()->isPointerTy()){
        Value *st = GenBuilder->CreateLoad(var->getAllocatedType(), var, var->getName().data());
        if (var->getAllocatedType() == GetPtrType(Int))
            location = GenBuilder->CreateGEP(GetType(Int), st, ValueGen(VarUnit->Length[0]));
        else
            location = GenBuilder->CreateGEP(GetType(Float), st, ValueGen(VarUnit->Length[0]));
    }
    else location = GenBuilder->CreateGEP(var->getAllocatedType(), var, ValueGen(VarUnit->Length[0]));
    return location;
}

void gen::IfGen(Function *F, shared_ptr<Stmt> &StmtUnit) {
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
    GenBuilder->SetInsertPoint(MergeBB);

}

// While CodeGen
void gen::WhileGen(Function *F, shared_ptr<Stmt> &StmtUnit) {

    BasicBlock *loopBB = createBB(F, "loop");
    BasicBlock *endLoopBB = createBB(F, "endLoop");

    // condition generation
    Value *EndCond = ConditionGen(StmtUnit->Condition);
    // 根据EndCond判断是否跳转
    GenBuilder->CreateCondBr(EndCond, loopBB, endLoopBB);

    // loop:
    GenBuilder->SetInsertPoint(loopBB);
    std::vector<std::string> removeList;
    for(auto &true_block : StmtUnit->First_block){
        if(true_block->AST_type == DECL) DeclGen(true_block, removeList);
        else if(true_block->AST_type == STMT) StmtGen(F, true_block);
    }
    NamedValues.remove(removeList);
    EndCond = ConditionGen(StmtUnit->Condition);
    GenBuilder->CreateCondBr(EndCond, loopBB, endLoopBB);

    // endLoop:
    GenBuilder->SetInsertPoint(endLoopBB);
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

Type *gen::GetType(type FuncType) {
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

Value *gen::ConditionGen(shared_ptr<BaseAST> &input) {
    shared_ptr<Exp> condition(reinterpret_pointer_cast<Exp>(input));
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
    else if (condition->Operator == "<") {
        if (is_float) return GenBuilder->CreateFCmpOLT(L, R, "cond");
        else return GenBuilder->CreateICmpULT(L, R, "cond");
    }
}

Value *gen::ValueGen(shared_ptr<BaseAST> &input) {
    if (input->AST_type == FINALEXP) {
        shared_ptr<FinalExp> expression(reinterpret_pointer_cast<FinalExp>(input));
        if(expression->Exp_type == Float) return ConstantFP::get(*GenContext, APFloat(expression->Number));
        else if(expression->Exp_type == Int) return ConstantInt::get(*GenContext, APInt(32, int(expression->Number)));
    }
    else if (input->AST_type == VARIABLE){
        shared_ptr<Variable> variable(reinterpret_pointer_cast<Variable>(input));
        AllocaInst *var = NamedValues.find(variable->Var_name);
        if (var == nullptr) {
            cout << "error: use of undeclared identifier '" << variable->Var_name << "'" << endl;
            exit(0);
        }
        if (variable->Length.empty())
            return GenBuilder->CreateLoad(var->getAllocatedType(), var, var->getName().data());
        else {
            Value *location = GetLocation(variable, var);
            type re = GetFuncType(var);
            return GenBuilder->CreateLoad(GetType(re), location, var->getName().data());
        }
    }
    else if (input->AST_type == FUNCPROTO) return FuncCallGen(input);
    else if (input->AST_type == EXP) {
        shared_ptr<Exp> expression(reinterpret_pointer_cast<Exp>(input));
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

type gen::GetFuncType(const AllocaInst *var) {
    type re;
    if (var->getAllocatedType()->isPointerTy()){
        if (var->getAllocatedType() == GetPtrType(Int))
            re = Int;
        else re = Float;
    }else {
        if (var->getAllocatedType() == GetType(Int))
            re = Int;
        else re = Float;
    }
    return re;
}

Value *gen::FuncCallGen(shared_ptr<BaseAST> &input) {
    shared_ptr<FuncPrototype> prototype(reinterpret_pointer_cast<FuncPrototype>(input));
    Function *CalleeF = GenModule->getFunction(prototype->Func_name);
    if (CalleeF == nullptr) {
        cout << "error: use of undeclared function '" << prototype->Func_name << "'" << endl;
        exit(0);
    }
    vector<Value*> ArgsV{};
    if (!prototype->Params.empty()){
        for(auto & Param : prototype->Params){
            if (Param->AST_type == VARIABLE){
                shared_ptr<Variable> VarUnit(reinterpret_pointer_cast<Variable>(Param));
                AllocaInst *var = NamedValues.find(VarUnit->Var_name);
                int dim = NamedValues.array_dim(VarUnit->Var_name);
                if (dim > 0 && VarUnit->Length.empty())
                    ArgsV.push_back(GenBuilder->CreateGEP(var->getAllocatedType(), var, ConstantInt::get(*GenContext, APInt(32, 0))));
                else{
                    shared_ptr<BaseAST> te(reinterpret_pointer_cast<BaseAST>(VarUnit));
                    ArgsV.push_back(ValueGen(te));
                }
            }
        }
    }
    return GenBuilder->CreateCall(CalleeF, ArgsV, "calltmp");
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
    return TmpB.CreateAlloca(GetType(VarType), nullptr, VarName);
}

AllocaInst *gen::createBlockPtrAlloca(BasicBlock &block, const string &VarName, type VarType) {
    IRBuilder<> TmpB(&block, block.begin());
    return TmpB.CreateAlloca(GetPtrType(VarType), nullptr, VarName);
}

AllocaInst *gen::createBlockAlloca(BasicBlock &block, const string &VarName, type VarType, Value* ArraySize) {
    IRBuilder<> TmpB(&block, block.begin());
    return TmpB.CreateAlloca(GetType(VarType), ArraySize, VarName);
}

Type *gen::GetPtrType(type FuncType) {
    switch (FuncType) {
        case Int:
            return Type::getInt32PtrTy(*GenContext);
        case Float:
            return Type::getFloatPtrTy(*GenContext);
    }
}
