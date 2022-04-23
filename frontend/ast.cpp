//
// Created by tcn on 22-4-11.
//

#include "ast.hpp"

void Decl::print(int indent) const {
    cout << Name << "\t" << Var_name << "\t";
    print_type(Decl_type);
    if (this->Exp != nullptr) {
        cout << endl;
        Exp->print(++indent);
    }
}

void Stmt::print(int indent) const {
    cout << Name << endl;
    switch (Stmt_type) {
        case If:
            print_indent(indent);
            cout << "Condition" << endl;
            Condition->print(++indent);
            cout << endl;
            print_indent(--indent);
            cout << "StmtBlock" << endl;
            for(const auto & Block : Blocks){
                print_indent(++indent);
                Block->print(indent);
                --indent;
                cout << endl;
            }
            break;
        case While:
            break;
        case Break:
            break;
        case Continue:
            break;
        case Return:
            break;
        case Expression:
            break;
    }
}

void Exp::print(int indent) const {
    if (Operator.empty()) Left_exp->print(indent);
    else {
        print_indent(indent);
        cout << Name << "\t" << Operator << endl;
        Left_exp->print(++indent);
        cout << endl;
        Right_exp->print(indent);
    }
}

void FinalExp::print(int indent) const {
    print_indent(indent);
    cout << Name << "\t" << Number;
}

void Func::print(int indent) const {
    cout << Name << "\t" << Func_name << "\t";
    print_type(Func_type);
    cout << endl;
    for(const auto & Block : Blocks){
        print_indent(indent);
        Block->print(++indent);
        cout << endl;
    }

}

void CompUnit::print(int indent) const {
    for (const auto & Unit : CompUnits) {
        Unit->print(indent);
        cout << endl;
    }
}

void Variable::print(int indent) const {

}
