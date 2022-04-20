//
// Created by tcn on 22-4-19.
//

#include "gen.hpp"

using namespace llvm;

void gen::ProgramGen(unique_ptr<CompUnit> &program) {
    for (auto & Unit : program->CompUnits) {
        if (Unit->Name == "FuncDef"){
            unique_ptr<Func> FuncUnit(reinterpret_cast<Func*>(Unit.release()));
            // Parameters
            std::vector<Type*> Para{};
            // Function
//            FunctionType *FT = FunctionType::get(Type::getVoidTy(GenContext), Para,false);
//            std::vector<Type *> Doubles(2, Type::getDoubleTy(*GenContext));
            FunctionType *FT = FunctionType::get(GetFuncType(FuncUnit->Func_type), Para, false);
            Function *F = Function::Create(FT, Function::ExternalLinkage, FuncUnit->Func_name, GenModule.get());

            // create block
            BasicBlock *BB = BasicBlock::Create(*GenContext, FuncUnit->Func_name, F);
            GenBuilder->SetInsertPoint(BB);

//            Value *
            GenBuilder->CreateRetVoid();
        }
    }
    GenModule->print(outs(), nullptr);
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

