//
// Created by tcn on 22-4-11.
//

#include "ast.hpp"

void CompUnitAST::print(int indent) const {
    cout << "CompUnit {" << endl;
    ++indent;
    print_indent(indent);
    func_def->print(indent);
    cout << "}" << endl;
}

void FuncDefAST::print(int indent) const {
    cout << "FuncDef { " << endl;
    ++indent;
    print_indent(indent);
    func_type->print(indent);
    cout << ", " << ident << ", " ;
    block->print(indent);
    print_indent(--indent);
    cout << " }" << endl;
}

void FuncTypeAST::print(int indent) const {
    cout << "FuncType :" << type;
}

void BlockAST::print(int indent) const {
    cout << "Block {" << endl;
    ++indent;
    print_indent(indent);
    block_item->print(indent);
    print_indent(--indent);
    cout << "}" << endl;
}

void StmtAST::print(int indent) const {
    cout << re << " " << number << endl;
}