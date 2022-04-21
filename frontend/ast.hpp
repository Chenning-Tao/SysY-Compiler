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
    virtual ~BaseAST() = default;
    virtual void print(int indent) const = 0;
};

// class for function
class Func : public BaseAST {
public:
    string Func_name;
    type Func_type;
    vector<unique_ptr<BaseAST>> Params;
    vector<unique_ptr<BaseAST>> Blocks;

    void print(int indent) const override;
};

// class for declaration
class Decl : public BaseAST {
public:
    bool Const;
    type Decl_type;
    string Var_name;
    unique_ptr<BaseAST> Exp;

    void print(int indent) const override;
};

// class for statement(e.g. for, if)
// TODO: add unit
class Stat : public BaseAST {
public:
    void print(int indent) const override;
};

// class for expression(e.g. 3+4)
class Exp : public BaseAST {
public:
    unique_ptr<BaseAST> Left_exp;
    string Operator;
    unique_ptr<BaseAST> Right_exp;

    void print(int indent) const override;
};

// class for number(e.g. 2, 3.4)
class FinalExp : public BaseAST {
public:
    type Exp_type;
    float number;

    void print(int indent) const override;
};

class CompUnit : public BaseAST {
public:
    vector<unique_ptr<BaseAST>> CompUnits;

    void print(int indent) const override;
};

#endif //COMPILER_AST_HPP
