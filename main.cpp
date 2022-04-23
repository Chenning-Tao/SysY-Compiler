#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "frontend/ast.hpp"
#include "backend/gen.hpp"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);
extern int yydebug;

int main(int argc, const char *argv[]) {
    auto input = argv[1];

    // open file for lexer
    yyin = fopen(input, "r");
    assert(yyin);

    // using parser func
    unique_ptr<BaseAST> ast;
    // yydebug = 1;
    auto ret = yyparse(ast);
    assert(!ret);
    ast->print(0);

    // generate LLVM IR
    gen program_gen("test");
    unique_ptr<CompUnit> program_ast(reinterpret_cast<CompUnit*>(ast.release()));
    program_gen.ProgramGen(program_ast);

    return 0;
}
