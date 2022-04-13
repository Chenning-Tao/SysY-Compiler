#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast/ast.hpp"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
    auto input = argv[1];

    // open file for lexer
    yyin = fopen(input, "r");
    assert(yyin);

    // using parser func
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);
    ast->print(0);

    return 0;
}
