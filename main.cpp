#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "frontend/myScanner.hpp"
#include "frontend/ast.hpp"
#include "backend/gen.hpp"

using namespace std;

// extern FILE *yyin;
extern int yyparse(shared_ptr<BaseAST> &ast, myScanner &scanner);
extern int yydebug;

int main(int argc, const char *argv[]) {
    auto input = argv[1];

    // open file for lexer
    // yyin = fopen(input, "r");
    // assert(yyin);
    ifstream* file = new ifstream(input);
    istream* sfile = file;
    myScanner scanner(sfile);

    // using parser func
    shared_ptr<BaseAST> ast;
    // yydebug = 1;
    auto ret = yyparse(ast, scanner);
    assert(!ret);
    ast->print(0);

    // generate LLVM IR
    gen program_gen("test");
    shared_ptr<CompUnit> program_ast(reinterpret_pointer_cast<CompUnit>(ast));
    program_gen.ProgramGen(program_ast);

    return 0;
}
