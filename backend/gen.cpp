//
// Created by tcn on 22-4-19.
//

#include "gen.hpp"

using namespace llvm;

void gen::ProgramGen(unique_ptr<CompUnit> &program) {
    for (auto & Unit : program->CompUnits) {
        if (Unit->AST_type == FUNC) FuncGen(Unit);
        else if (Unit->AST_type == DECL) GlobalVarGen(Unit);
    }
    GenModule->print(outs(), nullptr);
}

void gen::GlobalVarGen(unique_ptr<BaseAST> &Unit) {
    unique_ptr<Decl> global(reinterpret_cast<Decl*>(Unit.release()));
    // determine whether it has been declared
    if (GlobalValues.find(global->Var_name) == GlobalValues.end()) {
        GlobalVariable *gVar = createGlob(GetFuncType(global->Decl_type), global->Var_name);
        if (global->Exp != nullptr)
            gVar->setInitializer(dyn_cast<Constant>(ExpGen(global->Exp)));
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
    FunctionType *FT = FunctionType::get(GetFuncType(FuncUnit->Func_type), Para, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, FuncUnit->Func_name, GenModule.get());

    // create block
    BasicBlock *BB = createBB(F, FuncUnit->Func_name);
    GenBuilder->SetInsertPoint(BB);

    Value *test;
    for (auto & Block : FuncUnit->Blocks){
        if (Block->AST_type == STMT){
            unique_ptr<Stmt> StmtUnit(reinterpret_cast<Stmt*>(Block.release()));
            if (StmtUnit->Stmt_type == If) {
                BasicBlock *entry = createBB(F, "entry");
                BasicBlock *ThenBB = createBB(F, "then");
                BasicBlock *ElseBB = createBB(F, "else");
                BasicBlock *MergeBB = createBB(F, "ifcond");

                // condition generation
                Value *cond = ConditionGen(StmtUnit->Condition);
                test = cond;
                GenBuilder->CreateCondBr(cond, ThenBB, ElseBB);

                // condition = true
                GenBuilder->SetInsertPoint(ThenBB);
                GenBuilder->CreateBr(MergeBB);

                // condition = false
                GenBuilder->SetInsertPoint(ElseBB);
                GenBuilder->CreateBr(MergeBB);
            }
        }
        else if(Block->AST_type == DECL){
            unique_ptr<Decl> DeclUnit(reinterpret_cast<Decl*>(Block.release()));
            Function *cur = GenBuilder->GetInsertBlock()->getParent();
            // get init value
            Value *InitVal = nullptr;
            if (DeclUnit->Exp != nullptr) {
                InitVal = ExpGen(DeclUnit->Exp);
                if (DeclUnit->Decl_type == Float && InitVal->getType()->isIntegerTy())
                    InitVal = IntToFloat(InitVal);
            }
            // create IR
            AllocaInst *Alloca = createEntryBlockAlloca(cur, DeclUnit->Var_name, DeclUnit->Decl_type);
            GenBuilder->CreateStore(InitVal, Alloca);
        }
    }

    // return val
    GenBuilder->CreateRet(test);
}

Value *gen::IntToFloat(Value *InitVal) {
    InitVal = GenBuilder->CreateUIToFP(InitVal, Type::getFloatTy(*GenContext));
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
    Value *L = ExpGen(condition->Left_exp);
    Value *R = ExpGen(condition->Right_exp);
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

Value *gen::ExpGen(unique_ptr<BaseAST> &input) {
    if (input->AST_type == FINALEXP) {
        unique_ptr<FinalExp> expression(reinterpret_cast<FinalExp*>(input.release()));
        if(expression->Exp_type == Float) return ConstantFP::get(*GenContext, APFloat(expression->Number));
        else if(expression->Exp_type == Int) return ConstantInt::get(*GenContext, APInt(32, int(expression->Number)));
    }
    else if (input->AST_type == EXP) {
        unique_ptr<Exp> expression(reinterpret_cast<Exp*>(input.release()));
        Value *L = ExpGen(expression->Left_exp);
        // fake EXP node
        if (expression->Operator.empty()) return L;
        Value *R = ExpGen(expression->Right_exp);
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
            if(L->getType()->isFloatTy()) L = GenBuilder->CreateUIToFP(L, Type::getFloatTy(*GenContext));
            if(R->getType()->isFloatTy()) R = GenBuilder->CreateUIToFP(R, Type::getFloatTy(*GenContext));
            return GenBuilder->CreateFDiv(L, R);
        }
    }
}

bool gen::FloatGen(Value *&L, Value *&R) {
    bool is_float= !(L->getType()->isIntegerTy() && R->getType()->isIntegerTy());
    if (is_float){
        if(L->getType()->isFloatTy()) L = IntToFloat(L);
        if(R->getType()->isFloatTy()) R = IntToFloat(R);
    }
    return is_float;
}

GlobalVariable *gen::createGlob(Type *type, const std::string& name) {
    GenModule->getOrInsertGlobal(name, type);
    GlobalVariable *gVar = GenModule->getNamedGlobal(name);
    return gVar;
}

AllocaInst *gen::createEntryBlockAlloca(Function *TheFunction, const string &VarName, type VarType) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(GetFuncType(VarType), nullptr, VarName);
}

//    IRBuilder<> Tmp(&F->getEntryBlock(), F->getEntryBlock().begin());
//    AllocaInst *Alloc = Tmp.CreateAlloca(Type::getInt32Ty(*GenContext), 0, "testint");
//    GenBuilder->CreateStore(Init, Alloc);
