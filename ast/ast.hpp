//
// Created by tcn on 22-4-11.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H
#include <iostream>
#include <memory>
using namespace std;

// all AST base class
class BaseAST {
private:
public:
    static void print_indent(int indent) {
        for(int i = 0; i < indent; ++i) cout << "\t";
    }
    virtual ~BaseAST() = default;
    virtual void print(int indent) const = 0;
};

class CompUnitAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_def;
    void print(int indent) const override{
        cout << "CompUnit {" << endl;
        ++indent;
        print_indent(indent);
        func_def->print(indent);
        cout << "}" << endl;
    }
};

class FuncDefAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> block;
    void print(int indent) const override{
        cout << "FuncDef { " << endl;
        ++indent;
        print_indent(indent);
        func_type->print(indent);
        cout << ", " << ident << ", " ;
        block->print(indent);
        print_indent(--indent);
        cout << " }" << endl;
    }
};

class FuncTypeAST : public BaseAST {
public:
    string type;
    void print(int indent) const override{
        cout << "FuncType :" << type;
    }
};

class BlockAST : public BaseAST {
public:
    unique_ptr<BaseAST> block_item;
    void print(int indent) const override{
        cout << "Block {" << endl;
        ++indent;
        print_indent(indent);
        block_item->print(indent);
        print_indent(--indent);
        cout << "}" << endl;
    }
};

class StmtAST : public BaseAST {
public:
    string re;
    int number;
    void print(int indent) const override{
        cout << re << " " << number << endl;
    }
};

#endif //COMPILER_AST_H
