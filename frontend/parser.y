%code requires {
	#include <memory>
	#include <string>
	#include "ast.hpp"
	using namespace std;
}

%{

#define YYDEBUG 1
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
%type <ast_val> Exp_Wrap FuncRParams LVal LOrExp LAndExp EqExp RelExp Cond AddExp MulExp PrimaryExp UnaryExp Exp FuncDef FuncType Block Stmt Decl CompUnit ConstDecl VarDecl BType ConstDef BlockItem_Wrap BlockItem VarDef Number InitVal

%%

CompUnit
	: FuncDef {
		auto func = new CompUnit();
		func->Name = "CompUnits";
		func->AST_type = COMPUNIT;
		func->CompUnits.push_back(unique_ptr<BaseAST>($1));
		ast = unique_ptr<CompUnit>(func);
		$$ = func;
	}
	| Decl {
		auto decl = new CompUnit();
		decl->Name = "CompUnits";
		decl->AST_type = COMPUNIT;
		decl->CompUnits.push_back(unique_ptr<BaseAST>($1));
		ast = unique_ptr<CompUnit>(decl);
		$$ = decl;
	}
	| CompUnit Decl {
		auto comp_unit = new CompUnit();
		comp_unit->Name = "CompUnits";
		comp_unit->AST_type = COMPUNIT;
		comp_unit->CompUnits = move(reinterpret_cast<CompUnit*>$1->CompUnits);
		comp_unit->CompUnits.push_back(unique_ptr<BaseAST>($2));
		ast = unique_ptr<CompUnit>(comp_unit);
		$$ = comp_unit;
	}
	| CompUnit FuncDef {
		auto comp_unit = new CompUnit();
		comp_unit->Name = "CompUnits";
		comp_unit->AST_type = COMPUNIT;
		comp_unit->CompUnits = move(reinterpret_cast<CompUnit*>$1->CompUnits);
		comp_unit->CompUnits.push_back(unique_ptr<BaseAST>($2));
		ast = unique_ptr<CompUnit>(comp_unit);
		$$ = comp_unit;
	} 
	;

Decl
	: ConstDecl { 
		$1->Name = "ConstDecl";
		$1->AST_type = DECL;
		$$ = $1; 
	} 
	| VarDecl { 
		$1->Name = "VarDecl"; 
		$1->AST_type = DECL;
		$$ = $1; 
	}
	; 

// TODO: add rule for const value
ConstDecl
	: CONST BType ConstDef ';' { }
	;

BType
	: INT { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		$$ = ast;
	}
	| FLOAT { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		$$ = ast;
	}
	;

ConstDef
	: IDENT Exp_Wrap '=' ConstInitVal { }
	;

ConstInitVal
	: Exp { }
	| '{''}' { }
	| '{' ConstInitVal '}' { }
	| '{' ConstInitVal ConstInitVal_Wrap '}' { }
	;

ConstInitVal_Wrap
	: ',' ConstInitVal { }
	| ',' ConstInitVal ConstInitVal_Wrap { }
	;

VarDecl : VarDef ';' { $$ = $1; };

// use this to solve shift/reduce conflict
// TODO: find a better way
VarDef
	: INT IDENT { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = vector<unique_ptr<BaseAST>>();
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| INT IDENT '=' InitVal { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = vector<unique_ptr<BaseAST>>();
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = unique_ptr<BaseAST>($4);
		$$ = ast;
	}
	| INT IDENT Exp_Wrap { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = move(reinterpret_cast<Variable*>$3->Length);
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| INT IDENT Exp_Wrap '=' InitVal { }
	| FLOAT IDENT { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = vector<unique_ptr<BaseAST>>();
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| FLOAT IDENT '=' InitVal { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = vector<unique_ptr<BaseAST>>();
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = unique_ptr<BaseAST>($4);
		$$ = ast;
	}
	| FLOAT IDENT Exp_Wrap { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *unique_ptr<string>($2);
		var_ast->Length = move(reinterpret_cast<Variable*>$3->Length);
		ast->Var = unique_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| FLOAT IDENT Exp_Wrap '=' InitVal { }
	;

// TODO: add rule for array init
InitVal
	: Exp { $$ = $1; }
	| '{''}' { }
	| '{' InitVal '}' { }
	| '{' InitVal InitVal_Wrap '}' { }
	;

InitVal_Wrap
	: ',' InitVal { }
	| ',' InitVal InitVal_Wrap { }
	;

// TODO: add rule to pass function parameters
FuncDef
	: FuncType '(' ')' Block {
		auto ast = new Func();
		ast->Name = "FuncDef";
		ast->AST_type = FUNC;
		ast->Prototype = unique_ptr<BaseAST>($1);
		ast->Blocks = move(reinterpret_cast<Func*>$4->Blocks);
		$$ = ast;
	}
	| FuncType '(' FuncFParams ')' Block { }
	;

FuncType
	: INT IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *unique_ptr<string>($2);
		ast->Func_type = Int;
		$$ = ast;
	}
	| FLOAT IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *unique_ptr<string>($2);
		ast->Func_type = Float;
		$$ = ast;
	}
	| VOID IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *unique_ptr<string>($2);
		ast->Func_type = Void;
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
	: '[' Exp ']' { 
		auto ast = new Variable();
		ast->Length = vector<unique_ptr<BaseAST>>();
		ast->Length.push_back(unique_ptr<BaseAST>($2));
		$$ = ast;
	}
	| '[' Exp ']' Exp_Wrap { 
		auto ast = reinterpret_cast<Variable*>$4;
		ast->Length.insert(ast->Length.begin(), unique_ptr<BaseAST>($2));
		$$ = move(ast);
	}
	;

Block : '{' BlockItem_Wrap '}' { $$ = $2; } ;

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
	: Decl { $$ = $1; }
	| Stmt { $$ = $1; } ;

// TODO: add rule for while/break/continue
Stmt
	: LVal '=' Exp ';'{ 
		auto ast = new Stmt();
		ast->Name = "BinaryOpStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Assign;
		ast->LVal = unique_ptr<BaseAST>($1);
		ast->RVal = unique_ptr<BaseAST>($3);
		$$ = ast;
	}
	| Exp ';'{ }
	| Block { $$ = $1; }
	| IF '(' Cond ')' Stmt {  
		auto ast = new Stmt();
		ast->Name = "IfStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = If;
		ast->Condition = unique_ptr<BaseAST>($3);
		ast->First_block = move(reinterpret_cast<Func*>$5->Blocks);
		$$ = ast;
	}
	| IF '(' Cond ')' Stmt ELSE Stmt { }
	| WHILE '(' Cond ')' Stmt { 
	// $$ = newast5(maketext("Stmt"), $1,$2,$3,$4,$5);
		auto ast = new Stmt();
		ast->Name = "WhileStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = While;
		ast->Condition = unique_ptr<BaseAST>($3);
		ast->First_block = move(reinterpret_cast<Func*>$5->Blocks);
		$$ = ast;
	
	}
	| BREAK ';' { }
	| CONTINUE ';' { }
	| RETURN Exp ';' { 
		auto ast = new Stmt();
		ast->Name = "ReturnStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Return;
		// put return Exp in RVal
		ast->RVal = unique_ptr<BaseAST>($2);
		$$ = ast;
	}
	| RETURN ';' { 
		auto ast = new Stmt();
		ast->Name = "ReturnStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Return;
		// put return Exp in RVal
		ast->RVal = nullptr;
		$$ = ast;
	}
	;

Exp : AddExp { $$ = $1; };

Cond : LOrExp { $$ = $1; } ;

// TODO: add rule for array
LVal
	: IDENT { 
		auto ast = new Variable();
		ast->AST_type = VARIABLE;
		ast->Var_name = *unique_ptr<string>($1);
		ast->Length = vector<unique_ptr<BaseAST>>();
		$$ = ast;
	}
	| IDENT Exp_Wrap { 
		auto ast = new Variable();
		ast->AST_type = VARIABLE;
		ast->Var_name = *unique_ptr<string>($1);
		ast->Length = move(reinterpret_cast<Variable*>$2->Length);
		$$ = ast;
	}
	;

PrimaryExp
	: '(' Exp ')' { }
	| LVal { $$ = $1; }
	| Number { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "Number";
		ast->Left_exp = unique_ptr<BaseAST>($1);
		ast->Right_exp = nullptr;
		ast->Operator = "";
		$$ = ast;
	}
	;

Number
	: INT_CONST { 
		auto ast = new FinalExp();
		ast->Name = "IntConst";
		ast->AST_type = FINALEXP;
		ast->Exp_type = Int;
		ast->Number = $1;
		$$ = ast;
	}
	| FLOAT_CONST { 
		auto ast = new FinalExp();
		ast->Name = "FloatConst";
		ast->AST_type = FINALEXP;
		ast->Exp_type = Float;
		ast->Number = $1;
		$$ = ast; 
	}
	;

UnaryExp
	: PrimaryExp { $$ = $1;}
	| IDENT '(' ')' { 
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Name = "FuncCall";
		ast->Func_name = *unique_ptr<string>($1);
		$$ = ast;
	}
	| IDENT '(' FuncRParams ')' { 
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Name = "FuncCall";
		ast->Func_name = *unique_ptr<string>($1);
		ast->Params = move(reinterpret_cast<FuncPrototype*>$3->Params);
		$$ = ast;
	}
	| UnaryOp UnaryExp
	;

UnaryOp
	: '-' { }
	| '+' { }
	| '!' { }
	;

FuncRParams
	: Exp { 
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Params.push_back(unique_ptr<BaseAST>($1));
		$$ = ast;
	}
	| Exp FuncRParams_Wrap { }
	;

FuncRParams_Wrap
	: ',' Exp { }
	| ',' Exp FuncRParams_Wrap { }

MulExp
	: UnaryExp { $$ = $1; }
	| MulExp '*' UnaryExp { }
	| MulExp '/' UnaryExp { }
	| MulExp '%' UnaryExp { }
	;

AddExp
	: MulExp { $$ = $1; }
	| AddExp '+' MulExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "AddExp";
		ast->Left_exp = unique_ptr<BaseAST>($1);
		ast->Right_exp = unique_ptr<BaseAST>($3);
		ast->Operator = "+";
		$$ = ast;
	}
	| AddExp '-' MulExp { }
	;

RelExp
	: AddExp { $$ = $1; }
	| RelExp '<' AddExp { }
	| RelExp '>' AddExp { }
	| RelExp '<' '=' AddExp { }
	| RelExp '>' '=' AddExp { }
	;

EqExp
	: RelExp { $$ = $1; }
	| EqExp '=' '=' RelExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "EqExp";
		ast->Left_exp = unique_ptr<BaseAST>($1);
		ast->Operator = "==";
		ast->Right_exp = unique_ptr<BaseAST>($4);
		$$ = ast;
	}
	| EqExp '!' '=' RelExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "EqExp";
		ast->Left_exp = unique_ptr<BaseAST>($1);
		ast->Operator = "!=";
		ast->Right_exp = unique_ptr<BaseAST>($4);
		$$ = ast;
	}
	;

LAndExp
	: EqExp { $$ = $1; }
	| LAndExp '&' '&' EqExp { }
	;

LOrExp
	: LAndExp { $$ = $1; }
	| LOrExp '|' '|' LAndExp { }
	;

%%

// error process function, second parameter is error message
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
	cerr << "error: " << s << " at line " << yylineno << endl;
}
