//
// Created by tcn on 22-4-11.
//

#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP
#include <iostream>
#include <memory>
#include <map>
#include <llvm-14/llvm/IR/Value.h>
#include <llvm-14/llvm/ADT/APFloat.h>
#include <llvm-14/llvm/ADT/STLExtras.h>
#include <llvm-14/llvm/IR/BasicBlock.h>
#include <llvm-14/llvm/IR/Constants.h>
#include <llvm-14/llvm/IR/DerivedTypes.h>
#include <llvm-14/llvm/IR/Function.h>
#include <llvm-14/llvm/IR/IRBuilder.h>
#include <llvm-14/llvm/IR/LLVMContext.h>
#include <llvm-14/llvm/IR/Module.h>
#include <llvm-14/llvm/IR/Type.h>
#include <llvm-14/llvm/IR/Verifier.h>
using namespace std;

static void print_indent(int indent) {
    for(int i = 0; i < indent; ++i) cout << "\t";
}

// all AST base class
class BaseAST {
private:
public:
    virtual ~BaseAST() = default;
    virtual void print(int indent) const = 0;
};

class CompUnitAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_def;

    void print(int indent) const override;
};


class FuncDefAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> block;

    void print(int indent) const override;
};

class FuncTypeAST : public BaseAST {
public:
    string type;

    void print(int indent) const override;
};

class BlockAST : public BaseAST {
public:
    unique_ptr<BaseAST> block_item;

    void print(int indent) const override;
};

class StmtAST : public BaseAST {
public:
    string re;
    int number;

    void print(int indent) const override;
};

#endif //COMPILER_AST_HPP
