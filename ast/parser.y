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
extern int yylineno;

%}

// return value
%parse-param { unique_ptr<BaseAST> &ast }

// defination of yylval
%union {
    string *str_val;
    int int_val;
	float float_val;
    BaseAST *ast_val;
}

%token <str_val> IDENT
%token INT FLOAT VOID RETURN CONST IF ELSE WHILE BREAK CONTINUE
%token <int_val> INT_CONST
%token <float_val> FLOAT_CONST

// none terminal type
%type <ast_val> FuncDef FuncType Block Stmt Decl CompUnit ConstDecl VarDecl BType ConstDef ConstExp
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
//		comp_unit->decl = unique_ptr<BaseAST>($2);
//		comp_unit->next = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	}
	| CompUnit FuncDef {
		auto comp_unit = make_unique<CompUnit_FuncDef>();
//		comp_unit->func_def = unique_ptr<BaseAST>($2);
//		comp_unit->next = unique_ptr<BaseAST>($1);
		ast = move(comp_unit);
	} 
	;

Decl
	: ConstDecl { } 
	| VarDecl { }
	; 

ConstDecl
	: CONST BType ConstDef ';' { }
	;

BType
	: INT { }
	| FLOAT { }
	;

ConstDef
	: IDENT ConstExp_Wrap '=' ConstInitVal { }
	;

ConstExp_Wrap
	: '[' ConstExp ']' { }
	| '[' ConstExp ']' ConstExp_Wrap { }
	;

ConstInitVal
	: ConstExp { }
	| '{''}' { }
	| '{' ConstInitVal '}' { }
	| '{' ConstInitVal ConstInitVal_Wrap '}' { }
	;

ConstInitVal_Wrap
	: ',' ConstInitVal { }
	| ',' ConstInitVal ConstInitVal_Wrap { }
	;

VarDecl
	: BType VarDef ';' { }
	;

VarDef
	: IDENT { }
	| IDENT '=' InitVal { }
	| IDENT ConstExp_Wrap { }
	| IDENT ConstExp_Wrap '=' InitVal { }
	;

InitVal
	: Exp { }
	| '{''}' { }
	| '{' InitVal '}' { }
	| '{' InitVal InitVal_Wrap '}' { }
	;

InitVal_Wrap
	: ',' InitVal { }
	| ',' InitVal InitVal_Wrap { }
	;

FuncDef
	: FuncType '(' ')' Block {
		auto ast = new FuncDefAST();
		ast->func_type = unique_ptr<BaseAST>($1);
		ast->block = unique_ptr<BaseAST>($4);
		$$ = ast;
	}
	| FuncType '(' FuncFParams ')' Block { }
	;

FuncType
	: INT IDENT {
		auto ast = new FuncTypeAST();
		ast->type = *unique_ptr<string>(new string("int"));
		$$ = ast;
	}
	| FLOAT IDENT {
		auto ast = new FuncTypeAST();
		ast->type = *unique_ptr<string>(new string("float"));
		$$ = ast;
	}
	| VOID IDENT {
		auto ast = new FuncTypeAST();
		ast->type = *unique_ptr<string>(new string("void"));
		$$ = ast;
	}
	;

FuncFParams
	: FuncFParam { }
	| FuncFParam FuncFParams_Wrap { }
	;

FuncFParams_Wrap
	: ',' FuncFParam { }
	| ',' FuncFParam FuncFParams_Wrap { }
	;

FuncFParam
	: BType IDENT { }
	| BType IDENT '[' ']' { }
	| BType IDENT '[' ']' Exp_Wrap { }
	;

Exp_Wrap
	: '[' Exp ']' { }
	| '[' Exp ']' Exp_Wrap { }
	;

Block
	: '{' BlockItem_Wrap '}' {
		// TODO update
		auto ast = new BlockAST();
//		ast->block_item = unique_ptr<BaseAST>($2);
		$$ = ast;
	}
	;

BlockItem_Wrap
	: BlockItem { }
	| BlockItem BlockItem_Wrap { }
	;

BlockItem
	: Decl { }
	| Stmt { }
	;

Stmt
	: LVal '=' Exp ';'{ }
	| Exp ';'{ }
	| Block { }
	| IF '(' Cond ')' Stmt { }
	| IF '(' Cond ')' Stmt ELSE Stmt { }
	| WHILE '(' Cond ')' Stmt { }
	| BREAK ';' { }
	| CONTINUE ';' { }
	| RETURN Exp ';' { }
	| RETURN ';' { }
	/* | RETURN Number ';' {
		auto ast = new StmtAST();
		ast->re = *unique_ptr<string>(new string("return"));
		ast->number = $2;
		$$ = ast;
	} */
	;

Exp
	: AddExp { }
	;

Cond 
	: LOrExp { }
	;

LVal
	: IDENT { }
	| IDENT Exp_Wrap { }
	;

PrimaryExp
	: '(' Exp ')' { }
	| LVal { }
	| Number { }
	;

Number
	: INT_CONST { $$ = $1; }
	| FLOAT_CONST { $$ = $1; }
	;

UnaryExp
	: PrimaryExp { }
	| IDENT '(' ')' { }
	| IDENT '(' FuncRParams ')' { }
	| UnaryOp UnaryExp
	;

UnaryOp
	: '-' { }
	| '+' { }
	| '!' { }
	;

FuncRParams
	: Exp { }
	| Exp FuncRParams_Wrap { }
	;

FuncRParams_Wrap
	: ',' Exp { }
	| ',' Exp FuncRParams_Wrap { }

MulExp
	: UnaryExp { }
	| MulExp '*' UnaryExp { }
	| MulExp '/' UnaryExp { }
	| MulExp '%' UnaryExp { }
	;

AddExp
	: MulExp { }
	| AddExp '+' MulExp { }
	| AddExp '-' MulExp { }
	;

RelExp
	: AddExp { }
	| RelExp '<' AddExp { }
	| RelExp '>' AddExp { }
	| RelExp '<' '=' AddExp { }
	| RelExp '>' '=' AddExp { }
	;

EqExp
	: RelExp { }
	| EqExp '=' '=' RelExp { }
	| EqExp '!' '=' RelExp { }
	;

LAndExp
	: EqExp { }
	| LAndExp '&' '&' EqExp { }
	;

LOrExp
	: LAndExp { }
	| LOrExp '|' '|' LAndExp { }
	;

ConstExp
	: AddExp { }
	;

%%

// error process function, second parameter is error message
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
	cerr << "error: " << s << " at line " << yylineno << endl;
}
