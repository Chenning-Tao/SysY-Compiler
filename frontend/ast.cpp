//
// Created by tcn on 22-4-11.
//

#include "ast.hpp"

void Decl::print(int indent) const {
    cout << Name << "\t";
    print_type(Decl_type);
    cout << "\t";
    Var->print(indent);
    if (this->Exp != nullptr) {
        cout << endl;
        print_indent(++indent);
        Exp->print(indent);
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
            for(const auto & Block : First_block){
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
        case Assign:
            print_indent(++indent);
            LVal->print(indent);
            cout << endl;
            print_indent(indent);
            RVal->print(indent);
            break;
    }
}

void Exp::print(int indent) const {
    // TODO: used for func para, find a better way
    if (Name == "fake") cout << "[" "]";
    else {
        if (Operator.empty()) Left_exp->print(indent);
        else {
            cout << Name << "\t" << Operator << endl;
            print_indent(++indent);
            Left_exp->print(indent);
            cout << endl;
            print_indent(indent);
            Right_exp->print(indent);
        }
    }
}

void FinalExp::print(int indent) const {
    cout << Name << "\t" << Number;
}

void Func::print(int indent) const {
    cout << Name << "\t";
    Prototype->print(indent);
    cout << endl;
    for(const auto & Block : Blocks){
        print_indent(++indent);
        Block->print(indent);
        --indent;
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
    if (Length.empty()) cout << "Var\t" <<  Var_name;
    else {
        cout << "Array\t" << Var_name;
        for(const auto & i : Length){
            cout << endl;
            print_indent(++indent);
            i->print(indent);
            --indent;
        }
    }
}

void FuncPrototype::print(int indent) const {
    cout << Func_name << "\t";
    // TODO: identify call and declare
    print_type(Func_type);
    if (Params.empty()) return;
    cout << endl;
    for (const auto & Param : Params){
        print_indent(++indent);
        Param->print(indent);
        --indent;
    }
}
