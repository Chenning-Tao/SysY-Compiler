//
// Created by tcn on 22-4-11.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H
#include <memory>
using namespace std;

// all AST base class
class BaseAST {
public:
    virtual ~BaseAST() = default;
};

class CompUnitAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_def;
};

class FuncDefAST : public BaseAST {
public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> block;
};


#endif //COMPILER_AST_H
