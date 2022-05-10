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
#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
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
    std::shared_ptr<LLVMContext> GenContext;
    std::shared_ptr<IRBuilder<>> GenBuilder;
    std::shared_ptr<Module> GenModule;
    std::unordered_map<std::string, GlobalVariable *> GlobalValues;
    symbolTable NamedValues;

    BasicBlock *createBB(Function *fooFunc, const std::string& Name);
    GlobalVariable *createGlob(Type *type, const std::string& name);
    AllocaInst *createBlockAlloca(BasicBlock &block, const string &VarName, type VarType);
    AllocaInst *createBlockPtrAlloca(BasicBlock &block, const string &VarName, type VarType);
    AllocaInst *createBlockAlloca(BasicBlock &block, const string &VarName, type VarType, Value* ArraySize);
    Value *IntToFloat(Value *input);
    Value *FloatToInt(Value *InitVal);

    Value *FuncCallGen(shared_ptr<BaseAST> &input);
    void OutputGen();
    void InitExternalFunction();
    Value *ValueGen(shared_ptr<BaseAST> &input);
    Value *ConditionGen(shared_ptr<BaseAST> &input);
    Type *GetType(type FuncType);
    Type *GetPtrType(type FuncType);
    type GetFuncType(const AllocaInst *var);
    Value *GetLocation(const shared_ptr<Variable> &VarUnit, AllocaInst *var);
    bool FloatGen(Value *&L, Value *&R);
    void AssignGen(shared_ptr<Stmt> &StmtUnit);
    void IfGen(Function *F, shared_ptr<Stmt> &StmtUnit);
    void IfGen(Function *F, shared_ptr<Stmt> &StmtUnit, BasicBlock *loopBB, BasicBlock *endLoopBB);
    void WhileGen(Function *F, shared_ptr<Stmt> &StmtUnit);
    void GlobalVarGen(shared_ptr<BaseAST> &Unit);
    void DeclGen(shared_ptr<BaseAST> &Block, vector<std::string> &removeList);
    void FuncGen(shared_ptr<BaseAST> &Unit);
    void StmtGen(Function *F, shared_ptr<BaseAST> &Block);
    void PrintfGen(shared_ptr<Stmt> &StmtUnit);
    void ScanfGen(shared_ptr<Stmt> &StmtUnit);
public:
    explicit gen(const string& name);
    void ProgramGen(shared_ptr<CompUnit> &program);
};


#endif //COMPILER_GEN_HPP
