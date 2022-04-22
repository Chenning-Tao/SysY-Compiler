//
// Created by tcn on 22-4-19.
//

#include "gen.hpp"

using namespace llvm;

void gen::ProgramGen(unique_ptr<CompUnit> &program) {
    for (auto & Unit : program->CompUnits) {
        if (Unit->Name == "FuncDef") FuncGen(Unit);
        else if (Unit->Name == "Decl") {
            Value *InitVal = ConstantFP::get(*GenContext, APFloat(0.0));
//    IRBuilder<> TmpB(&F->getEntryBlock(), F->getEntryBlock().begin());
//    AllocaInst *Alloca = TmpB.CreateAlloca(Type::getFloatTy(*GenContext), 0, "testfloat");
//    GenBuilder->CreateStore(InitVal, Alloca);

            Value *Init = ConstantInt::get(*GenContext, APInt(32, 1));
//    IRBuilder<> Tmp(&F->getEntryBlock(), F->getEntryBlock().begin());
//    AllocaInst *Alloc = Tmp.CreateAlloca(Type::getInt32Ty(*GenContext), 0, "testint");
//    GenBuilder->CreateStore(Init, Alloc);

            Value *re = GenBuilder->CreateFAdd(InitVal, Init, "addtmp");
        }
    }
    GenModule->print(outs(), nullptr);
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
    BasicBlock *BB = BasicBlock::Create(*GenContext, FuncUnit->Func_name, F);
    GenBuilder->SetInsertPoint(BB);

    // return val
    GenBuilder->CreateRetVoid();
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

