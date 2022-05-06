//
// Created by tcn on 22-4-11.
//

#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP
#include <iostream>
#include <memory>
#include <map>
#include <vector>
using namespace std;

enum type {Int, Float, Void};
enum stmt {If, While, Break, Continue, Return, Assign, Printf, Scanf};
enum ast {FUNC, FUNCPROTO, DECL, EXP, FINALEXP, STMT, COMPUNIT, VARIABLE};

static void print_indent(int indent) {
    for(int i = 0; i < indent; ++i) cout << "\t";
}

static void print_type(type func_type) {
    switch (func_type) {
        case Int:
            cout << "'int'";
            break;
        case Float:
            cout << "'float'";
            break;
        case Void:
            cout << "'void'";
            break;
    }
}

// all AST base class
class BaseAST {
private:
public:
    string Name;
    ast AST_type;
    virtual ~BaseAST() = default;
    virtual void print(int indent) const = 0;
};

// class for function declare
class FuncPrototype : public BaseAST {
public:
    string Func_name;
    type Func_type;
    vector<shared_ptr<BaseAST>> Params;

    void print(int indent) const override;
};

// class for function
class Func : public BaseAST {
public:
    shared_ptr<BaseAST> Prototype;
    vector<shared_ptr<BaseAST>> Blocks;

    void print(int indent) const override;
};

// class for declaration
class Decl : public BaseAST {
public:
    bool Const;
    type Decl_type;
    shared_ptr<BaseAST> Var;
    shared_ptr<BaseAST> Exp;

    void print(int indent) const override;
};

// class for statement(e.g. for, if)
class Stmt : public BaseAST {
public:
    stmt Stmt_type;
    shared_ptr<BaseAST> LVal;
    shared_ptr<BaseAST> RVal;
    // second_block is only used in IF-ELSE statement
    string IO;
    vector<shared_ptr<BaseAST>> First_block;
    vector<shared_ptr<BaseAST>> Second_block;
    shared_ptr<BaseAST> Condition;

    void print(int indent) const override;
};

// class for expression(e.g. 3+4)
class Exp : public BaseAST {
public:
    shared_ptr<BaseAST> Left_exp;
    string Operator;
    shared_ptr<BaseAST> Right_exp;

    void print(int indent) const override;
};

// class for number (e.g. 2, 3.4)
class FinalExp : public BaseAST {
public:
    type Exp_type;
    float Number;

    void print(int indent) const override;
};

// class for variable and array (e.g. a, a[0])
class Variable : public BaseAST {
public:
    string Var_name;
    // if not empty, indicates that this is an array
    vector<shared_ptr<BaseAST>> Length;

    void print(int indent) const override;
};

class CompUnit : public BaseAST {
public:
    vector<shared_ptr<BaseAST>> CompUnits;

    void print(int indent) const override;
};

#endif //COMPILER_AST_HPP
