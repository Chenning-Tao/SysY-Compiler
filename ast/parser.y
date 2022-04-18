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
#include <vector>
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
%type <ast_val> FuncDef FuncType Block Stmt Decl CompUnit ConstDecl VarDecl BType ConstDef ConstExp BlockItem_Wrap BlockItem VarDef
%type <int_val> Number

%%

CompUnit
	: FuncDef {
		auto func = make_unique<Func>();
		func->Name = "FuncDef";
		func->Func_name = reinterpret_cast<Func*>$1->Func_name;
		func->Func_type = reinterpret_cast<Func*>$1->Func_type;
		func->Params = move(reinterpret_cast<Func*>$1->Params);
		func->Blocks = move(reinterpret_cast<Func*>$1->Blocks);
		ast = move(func);
	}
	| Decl {
		// auto comp_unit = make_unique<CompUnit_Decl>();
		// comp_unit->decl = unique_ptr<BaseAST>($1);
		// ast = move(comp_unit);
	}
	| CompUnit Decl {
		// auto comp_unit = make_unique<CompUnit_Decl>();
		// ast = move(comp_unit);
	}
	| CompUnit FuncDef {
		// auto comp_unit = make_unique<CompUnit_FuncDef>();
		// ast = move(comp_unit);
	} 
	;

Decl
	: ConstDecl { } 
	| VarDecl { 
		$$ = $1;
	}
	; 

ConstDecl
	: CONST BType ConstDef ';' { }
	;

BType
	: INT { 
		auto ast = new Decl();
		ast->Decl_type = Int;
	}
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
	: BType VarDef ';' {
		auto ast = new Decl();
		ast->Name = "VarDecl";
		ast->Decl_type = reinterpret_cast<Decl*>$1->Decl_type;
		ast->Var_name = reinterpret_cast<Decl*>$2->Var_name;
		$$ = ast;
	}
	;

VarDef
	: IDENT { 
		auto ast = new Decl();
		ast->Var_name = *unique_ptr<string>($1);
		$$ = ast;
	}
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
		auto ast = new Func();
		ast->Func_name = reinterpret_cast<Func*>$1->Func_name;
		ast->Func_type = reinterpret_cast<Func*>$1->Func_type;
		ast->Params = vector<unique_ptr<BaseAST>>();
		ast->Blocks = move(reinterpret_cast<Func*>$4->Blocks);
		$$ = ast;
	}
	| FuncType '(' FuncFParams ')' Block { }
	;

FuncType
	: INT IDENT {
		auto ast = new Func();
		ast->Func_name = *unique_ptr<string>($2);
		ast->Func_type = Int;
		$$ = ast;
	}
	| FLOAT IDENT {
		// auto ast = new FuncTypeAST();
		// ast->type = *unique_ptr<string>(new string("float"));
		// $$ = ast;
	}
	| VOID IDENT {
		// auto ast = new FuncTypeAST();
		// ast->type = *unique_ptr<string>(new string("void"));
		// $$ = ast;
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
		$$ = $2;
	}
	;

BlockItem_Wrap
	: BlockItem { 
		auto ast = new Func();
		ast->Blocks.push_back(unique_ptr<BaseAST>($1));
		$$ = ast;
	}
	| BlockItem BlockItem_Wrap { 
		auto ast = reinterpret_cast<Func*>$2;
		ast->Blocks.insert(ast->Blocks.begin(), unique_ptr<BaseAST>($1));
		$$ = move(ast);
	}
	;

BlockItem
	: Decl { 
		$$ = $1;
	}
	| Stmt {
		// auto ast = new Stat();
	}
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
