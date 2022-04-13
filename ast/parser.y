%code requires {
	#include <memory>
	#include <string>
	#include "ast.hpp"
	using namespace std;
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"


using namespace std;
// declear lex function and error process function
int yylex();
void yyerror(unique_ptr<BaseAST> &ast, const char *s);


%}

// return value
%parse-param { unique_ptr<BaseAST> &ast }

// defination of yylval
%union {
    string *str_val;
    int int_val;
    BaseAST *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

// none terminal type
%type <ast_val> FuncDef FuncType Block Stmt Decl CompUnit
%type <int_val> Number

%%

CompUnit
	: FuncDef {
		auto comp_unit = make_unique<CompUnit_FuncDef>();
		comp_unit->func_def = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	}
	| Decl {
		auto comp_unit = make_unique<CompUnit_Decl>();
		comp_unit->decl = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	}
	| CompUnit Decl {
		auto comp_unit = make_unique<CompUnit_Decl>();
		comp_unit->decl = unique_ptr<BaseAST>($2);
		comp_unit->next = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	}
	| CompUnit FuncDef {
		auto comp_unit = make_unique<CompUnit_FuncDef>();
		comp_unit->func_def = unique_ptr<BaseAST>($2);
		comp_unit->next = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	}
	;

Decl
	: INT {
		auto ast = new FuncTypeAST();
		ast->type = *unique_ptr<string>(new string("int"));
		$$ = ast;
	}
	; 

FuncDef
	: FuncType IDENT '(' ')' Block {
		auto ast = new FuncDefAST();
		ast->func_type = unique_ptr<BaseAST>($1);
		ast->ident = *unique_ptr<string>($2);
		ast->block = unique_ptr<BaseAST>($5);
		$$ = ast;
	}
	;

FuncType
	: INT {
		auto ast = new FuncTypeAST();
		ast->type = *unique_ptr<string>(new string("int"));
		$$ = ast;
	}
	;

Block
	: '{' Stmt '}' {
		auto ast = new BlockAST();
		ast->block_item = unique_ptr<BaseAST>($2);
		$$ = ast;
	}
	;

Stmt
	: RETURN Number ';' {
		auto ast = new StmtAST();
		ast->re = *unique_ptr<string>(new string("return"));
		ast->number = $2;
		$$ = ast;
	}
	;

Number
	: INT_CONST {
		$$ = $1;
	}
	;

%%

// error process function, second parameter is error message
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
	cerr << "error: " << s << endl;
}
