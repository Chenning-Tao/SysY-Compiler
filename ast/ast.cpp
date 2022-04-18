//
// Created by tcn on 22-4-11.
//

#include "ast.hpp"

void Decl::print(int indent) const {
    cout << Name << "\t" << Var_name << " ";
    print_type(Decl_type);
}

void Stat::print(int indent) const {

}

void Exp::print(int indent) const {

}

void FinalExp::print(int indent) const {

}

void Func::print(int indent) const {
    cout << Name << "\t" << Func_name << " ";
    print_type(Func_type);
    cout << endl;
    ++indent;
    print_indent(indent);
    Block->print(indent);
}
