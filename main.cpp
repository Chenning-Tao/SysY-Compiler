#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast/ast.h"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
//    assert(argc == 5);
//    auto mode = argv[1];
    auto input = argv[1];
//    auto output = argv[4];

    // open file for lexer
    yyin = fopen(input, "r");
    assert(yyin);

    // using parser function
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);
    ast->print(0);

    return 0;
}
