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
#include "symbolTable.hpp"
#include <algorithm>

using namespace llvm;

class gen {
private:
    std::unique_ptr<LLVMContext> GenContext;
    std::unique_ptr<IRBuilder<>> GenBuilder;
    std::unique_ptr<Module> GenModule;
    std::unordered_map<std::string, GlobalVariable *> GlobalValues;
    symbolTable NamedValues;

    BasicBlock *createBB(Function *fooFunc, const std::string& Name);
    GlobalVariable *createGlob(Type *type, const std::string& name);
    AllocaInst *createBlockAlloca(BasicBlock &block, const string &VarName, type VarType);
    Value *IntToFloat(Value *input);
    Value *FloatToInt(Value *InitVal);

    Value *ExpGen(unique_ptr<BaseAST> &input);
    Value *ConditionGen(unique_ptr<BaseAST> &input);
    Type *GetFuncType(type FuncType);
    bool FloatGen(Value *&L, Value *&R);
    void IfGen(Function *F, unique_ptr<Stmt> &StmtUnit);
    void GlobalVarGen(unique_ptr<BaseAST> &Unit);
    void DeclGen(unique_ptr<BaseAST> &Block, vector<std::string> &removeList);
    void FuncGen(unique_ptr<BaseAST> &Unit);
    void StmtGen(Function *F, unique_ptr<BaseAST> &Block);
public:
    explicit gen(const string& name);
    void ProgramGen(unique_ptr<CompUnit> &program);

    void AssignGen(unique_ptr<Stmt> &StmtUnit);

    Value *LoadValue(const string &temp_name);
};


#endif //COMPILER_GEN_HPP
