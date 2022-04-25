//
// Created by tcn on 22-4-19.
//

#ifndef COMPILER_GEN_HPP
#define COMPILER_GEN_HPP

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ast.hpp"

using namespace llvm;

class gen {
private:
    std::unique_ptr<LLVMContext> GenContext;
    std::unique_ptr<IRBuilder<>> GenBuilder;
    std::unique_ptr<Module> GenModule;
    std::map<std::string, Value *> NamedValues;
    std::unordered_map<std::string, GlobalVariable *> GlobalValues;
    BasicBlock *createBB(Function *fooFunc, const std::string& Name);
    GlobalVariable *createGlob(Type *type, const std::string& name);
    AllocaInst *createEntryBlockAlloca(Function *TheFunction, const std::string &VarName, type VarType);
    Value *IntToFloat(Value *input);

    Value *ExpGen(unique_ptr<BaseAST> &input);
    Value *ConditionGen(unique_ptr<BaseAST> &input);
    Type *GetFuncType(type FuncType);
    bool FloatGen(Value *&L, Value *&R);
    void GlobalVarGen(unique_ptr<BaseAST> &Unit);
    void FuncGen(unique_ptr<BaseAST> &Unit);
public:
    explicit gen(const string& name);
    void ProgramGen(unique_ptr<CompUnit> &program);
};


#endif //COMPILER_GEN_HPP
