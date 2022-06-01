%code requires {
	#include <memory>
	#include <string>
	#include "ast.hpp"
	#include "myScanner.hpp"
	using namespace std;
}

%locations
// return value
%parse-param { shared_ptr<BaseAST> &ast }
%parse-param { myScanner &scanner }

%code {

	#define YYDEBUG 1
	#include <iostream>
	#include <memory>
	#include <string>
	#include <vector>
	#include "myScanner.hpp"
	#include "ast.hpp"
	#define parser_class_name {myScanner}

	using namespace std;
	// declear lex function and error process function
	void yyerror(shared_ptr<BaseAST> &ast, myScanner &scanner, const char *s);
	extern int yylineno;
	extern int yycolumn;

	#undef yylex
	#define yylex scanner.yylex

	#undef yylineno
	#define yylineno scanner.lineno()

}

// defination of yylval
%union {
    string *str_val;
    int int_val;
	float float_val;
    BaseAST *ast_val;
}

%token <str_val> IDENT STRING
%token INT FLOAT STRUCT VOID RETURN CONST IF ELSE WHILE BREAK CONTINUE PRINTF SCANF
%token NE EQ LT GT LE GE
%token <int_val> INT_CONST
%token <float_val> FLOAT_CONST

// none terminal type
%type <ast_val> InitVal_Wrap VarDecl_Wrap StructDecl FuncFParams_Wrap FuncRParams_Wrap FuncFParams FuncFParam Exp_Wrap FuncRParams LVal LOrExp LAndExp EqExp RelExp Cond AddExp MulExp PrimaryExp UnaryExp Exp FuncDef FuncType Block Stmt Decl CompUnit ConstDecl VarDecl BType ConstDef BlockItem_Wrap BlockItem VarDef Number InitVal

%%

CompUnit
	: FuncDef {
		auto func = new CompUnit();
		func->Name = "CompUnits";
		func->AST_type = COMPUNIT;
		func->CompUnits.push_back(shared_ptr<BaseAST>($1));
		ast = shared_ptr<CompUnit>(func);
		$$ = func;
	}
	| Decl {
		auto decl = new CompUnit();
		decl->Name = "CompUnits";
		decl->AST_type = COMPUNIT;
		decl->CompUnits.push_back(shared_ptr<BaseAST>($1));
		ast = shared_ptr<CompUnit>(decl);
		$$ = decl;
	}
	| CompUnit Decl {
		auto comp_unit = new CompUnit();
		comp_unit->Name = "CompUnits";
		comp_unit->AST_type = COMPUNIT;
		comp_unit->CompUnits = move(reinterpret_cast<CompUnit*>$1->CompUnits);
		comp_unit->CompUnits.push_back(shared_ptr<BaseAST>($2));
		ast = shared_ptr<CompUnit>(comp_unit);
		$$ = comp_unit;
	}
	| CompUnit FuncDef {
		auto comp_unit = new CompUnit();
		comp_unit->Name = "CompUnits";
		comp_unit->AST_type = COMPUNIT;
		comp_unit->CompUnits = move(reinterpret_cast<CompUnit*>$1->CompUnits);
		comp_unit->CompUnits.push_back(shared_ptr<BaseAST>($2));
		ast = shared_ptr<CompUnit>(comp_unit);
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
	| StructDecl {
		$1->Name = "StructDecl";
		$1->AST_type = DECL;
		$$ = $1;
	}
	; 

// TODO: add rule for const value
ConstDecl
	: CONST BType ConstDef ';' { }
	;

StructDecl
	: STRUCT IDENT '{' VarDecl_Wrap '}' ';' { 
		auto ast = new Decl();
		ast->Decl_type = Struct;
		ast->Struct_name = *shared_ptr<string>($2);
		ast->Member = move(reinterpret_cast<Decl*>$4->Member);
		$$ = ast;
	}
	| STRUCT IDENT IDENT '=' InitVal ';' {
		auto ast = new Decl();
		ast->Decl_type = Struct;
		ast->Struct_name = *shared_ptr<string>($2);
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($3);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = shared_ptr<BaseAST>($5);
		$$ = ast;
	}
	;

VarDecl_Wrap
	: VarDecl { 
		auto ast = new Decl();
		ast->Member.push_back(shared_ptr<BaseAST>($1));
		$$ = ast;
	}
	| VarDecl_Wrap VarDecl { 
		auto ast = reinterpret_cast<Decl*>$1;
		ast->Member.push_back(shared_ptr<BaseAST>($2));
		$$ = ast;
	}
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
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| INT IDENT '=' InitVal { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = shared_ptr<BaseAST>($4);
		$$ = ast;
	}
	| INT IDENT Exp_Wrap { 
		auto ast = new Decl();
		ast->Decl_type = Int;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = move(reinterpret_cast<Variable*>$3->Length);
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| INT IDENT Exp_Wrap '=' InitVal { }
	| FLOAT IDENT { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| FLOAT IDENT '=' InitVal { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = shared_ptr<BaseAST>($4);
		$$ = ast;
	}
	| FLOAT IDENT Exp_Wrap { 
		auto ast = new Decl();
		ast->Decl_type = Float;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = move(reinterpret_cast<Variable*>$3->Length);
		ast->Var = shared_ptr<BaseAST>(var_ast);
		ast->Exp = nullptr;
		$$ = ast;
	}
	| FLOAT IDENT Exp_Wrap '=' InitVal { }
	;

// TODO: add rule for array init
InitVal
	: Exp { $$ = $1; }
	| '{''}' { }
	| '{' InitVal '}' { $$ = $2; }
	| '{' InitVal InitVal_Wrap '}' { 
		auto ast = reinterpret_cast<ExpList*>$3;
		ast->AST_type = EXPLIST;
		ast->Exps.insert(ast->Exps.begin(), shared_ptr<BaseAST>($2));
		$$ = ast;
	}
	;

InitVal_Wrap
	: ',' InitVal { 
		auto ast = new ExpList();
		ast->Exps.push_back(shared_ptr<BaseAST>($2));
		$$ = ast;
	}
	| ',' InitVal InitVal_Wrap { 
		auto ast = reinterpret_cast<ExpList*>$3;
		ast->Exps.insert(ast->Exps.begin(), shared_ptr<BaseAST>($2));
		$$ = ast;
	}
	;

FuncDef
	: FuncType '(' ')' Block {
		auto ast = new Func();
		ast->Name = "FuncDef";
		ast->AST_type = FUNC;
		ast->Prototype = shared_ptr<BaseAST>($1);
		ast->Blocks = move(reinterpret_cast<Func*>$4->Blocks);
		$$ = ast;
	}
	| FuncType '(' FuncFParams ')' Block { 
		auto ast = new Func();
		ast->Name = "FuncDef";
		ast->AST_type = FUNC;
		auto func_prototype = move(reinterpret_cast<FuncPrototype*>$1);
		func_prototype->Params = move(reinterpret_cast<FuncPrototype*>$3->Params);
		ast->Prototype = shared_ptr<BaseAST>(func_prototype);
		ast->Blocks = move(reinterpret_cast<Func*>$5->Blocks);
		$$ = ast;
	}
	;

FuncType
	: INT IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *shared_ptr<string>($2);
		ast->Func_type = Int;
		$$ = ast;
	}
	| FLOAT IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *shared_ptr<string>($2);
		ast->Func_type = Float;
		$$ = ast;
	}
	| VOID IDENT {
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Func_name = *shared_ptr<string>($2);
		ast->Func_type = Void;
		$$ = ast;
	}
	;

FuncFParams
	: FuncFParam { $$ = $1; }
	| FuncFParam FuncFParams_Wrap { 
		auto ast = reinterpret_cast<FuncPrototype*>$2;
		ast->Params.insert(ast->Params.begin(), reinterpret_cast<FuncPrototype*>$1->Params[0]);
		$$ = ast;
	}
	;

FuncFParams_Wrap
	: ',' FuncFParam { $$ = $2; }
	| ',' FuncFParam FuncFParams_Wrap { 
		auto ast = reinterpret_cast<FuncPrototype*>$3;
		ast->Params.insert(ast->Params.begin(), reinterpret_cast<FuncPrototype*>$2->Params[0]);
		$$ = ast;
	}
	;

FuncFParam
	: BType IDENT { 
		auto ast = new FuncPrototype();
		auto decl = new Decl();
		decl->Decl_type = reinterpret_cast<Decl*>$1->Decl_type;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		decl->Var = shared_ptr<BaseAST>(var_ast);
		decl->Exp = nullptr;
		ast->Params = vector<shared_ptr<BaseAST>>();
		ast->Params.push_back(shared_ptr<BaseAST>(decl));
		$$ = ast;
	}
	| BType IDENT '[' ']' { 
		auto ast = new FuncPrototype();
		auto decl = new Decl();
		decl->Decl_type = reinterpret_cast<Decl*>$1->Decl_type;
		auto var_ast = new Variable();
		var_ast->Var_name = *shared_ptr<string>($2);
		var_ast->Length = vector<shared_ptr<BaseAST>>();
		auto fake_exp = new Exp();
		fake_exp->AST_type = EXP;
		fake_exp->Name = "fake";
		var_ast->Length.push_back(shared_ptr<BaseAST>(fake_exp));
		decl->Var = shared_ptr<BaseAST>(var_ast);
		decl->Exp = nullptr;
		ast->Params = vector<shared_ptr<BaseAST>>();
		ast->Params.push_back(shared_ptr<BaseAST>(decl));
		$$ = ast;
	}
	| BType IDENT '[' ']' Exp_Wrap { }
	;

Exp_Wrap
	: '[' Exp ']' { 
		auto ast = new Variable();
		ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Length.push_back(shared_ptr<BaseAST>($2));
		$$ = ast;
	}
	| '[' Exp ']' Exp_Wrap { 
		auto ast = reinterpret_cast<Variable*>$4;
		ast->Length.insert(ast->Length.begin(), shared_ptr<BaseAST>($2));
		$$ = move(ast);
	}
	;

Block : '{' BlockItem_Wrap '}' { $$ = $2; } ;

BlockItem_Wrap
	: BlockItem { 
		auto ast = new Func();
		ast->Blocks.push_back(shared_ptr<BaseAST>($1));
		$$ = ast;
	}
	| BlockItem BlockItem_Wrap { 
		auto ast = reinterpret_cast<Func*>$2;
		ast->Blocks.insert(ast->Blocks.begin(), shared_ptr<BaseAST>($1));
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
		ast->LVal = shared_ptr<BaseAST>($1);
		ast->RVal = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	| PRINTF '(' STRING ')' ';' {
		auto ast = new Stmt();
		ast->Name = "PrintfStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Printf;
		ast->IO = *shared_ptr<string>($3);
		ast->First_block = vector<shared_ptr<BaseAST>>();
		$$ = ast;
	}
	| PRINTF '(' STRING ',' FuncRParams ')' ';' {
		auto ast = new Stmt();
		ast->Name = "PrintfStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Printf;
		ast->IO = *shared_ptr<string>($3);
		ast->First_block = move(reinterpret_cast<FuncPrototype*>$5->Params);
		$$ = ast;
	}
	| SCANF '(' STRING ',' FuncRParams ')' ';' {
		auto ast = new Stmt();
		ast->Name = "ScanfStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Scanf;
		ast->IO = *shared_ptr<string>($3);
		ast->First_block = move(reinterpret_cast<FuncPrototype*>$5->Params);
		$$ = ast;
	}
	| Exp ';'{ 
		auto ast = new Stmt();
		ast->Name = "ExpStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Expression;
		ast->RVal = shared_ptr<BaseAST>($1);
		$$ = ast;
	}
	| Block { $$ = $1; }
	| IF '(' Cond ')' Stmt {  
		auto ast = new Stmt();
		ast->Name = "IfStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = If;
		ast->Condition = shared_ptr<BaseAST>($3);
		ast->First_block = move(reinterpret_cast<Func*>$5->Blocks);
		ast->Second_block = vector<shared_ptr<BaseAST>>();
		$$ = ast;
	}
	| IF '(' Cond ')' Stmt ELSE Stmt { 
		auto ast = new Stmt();
		ast->Name = "IfElseStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = If;
		ast->Condition = shared_ptr<BaseAST>($3);
		ast->First_block = move(reinterpret_cast<Func*>$5->Blocks);
		ast->Second_block = move(reinterpret_cast<Func*>$7->Blocks);
		$$ = ast;
	}
	| WHILE '(' Cond ')' Stmt { 
		auto ast = new Stmt();
		ast->Name = "WhileStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = While;
		ast->Condition = shared_ptr<BaseAST>($3);
		ast->First_block = move(reinterpret_cast<Func*>$5->Blocks);
		$$ = ast;
	
	}
	| BREAK ';' { 
		auto ast = new Stmt();
		ast->Name = "BreakStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Break;
		ast->RVal = nullptr;
		$$ = ast;
	}
	| CONTINUE ';' {
		auto ast = new Stmt();
		ast->Name = "ContinueStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Continue;
		ast->RVal = nullptr;
		$$ = ast;
	 }
	| RETURN Exp ';' { 
		auto ast = new Stmt();
		ast->Name = "ReturnStmt";
		ast->AST_type = STMT;
		ast->Stmt_type = Return;
		// put return Exp in RVal
		ast->RVal = shared_ptr<BaseAST>($2);
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

Exp : AddExp { $$ = $1; }
	| LOrExp { $$ = $1; };

Cond : LOrExp { $$ = $1; } ;

// TODO: add rule for array
LVal
	: IDENT { 
		auto ast = new Variable();
		ast->AST_type = VARIABLE;
		ast->Var_name = *shared_ptr<string>($1);
		ast->Length = vector<shared_ptr<BaseAST>>();
		$$ = ast;
	}
	| IDENT Exp_Wrap { 
		auto ast = new Variable();
		ast->AST_type = VARIABLE;
		ast->Var_name = *shared_ptr<string>($1);
		ast->Length = move(reinterpret_cast<Variable*>$2->Length);
		$$ = ast;
	}
	| IDENT '.' IDENT {
		auto ast = new Variable();
		ast->AST_type = VARIABLE;
		ast->Var_name = *shared_ptr<string>($1);
		ast->Length = vector<shared_ptr<BaseAST>>();
		ast->Member_name = *shared_ptr<string>($3);
		$$ = ast;
	}
	; 

PrimaryExp
	: '(' Exp ')' { $$ = $2;}
	| LVal { $$ = $1; }
	| Number { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "Number";
		ast->Left_exp = shared_ptr<BaseAST>($1);
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
		ast->Func_name = *shared_ptr<string>($1);
		$$ = ast;
	}
	| IDENT '(' FuncRParams ')' { 
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Name = "FuncCall";
		ast->Func_name = *shared_ptr<string>($1);
		ast->Params = move(reinterpret_cast<FuncPrototype*>$3->Params);
		$$ = ast;
	}
	| UnaryOp UnaryExp {}
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
		ast->Params.push_back(shared_ptr<BaseAST>($1));
		$$ = ast;
	}
	| Exp FuncRParams_Wrap { 
		auto ast = reinterpret_cast<FuncPrototype*>$2;
		ast->Params.insert(ast->Params.begin(), shared_ptr<BaseAST>($1));
		$$ = ast;
	}
	;

FuncRParams_Wrap
	: ',' Exp { 
		auto ast = new FuncPrototype();
		ast->AST_type = FUNCPROTO;
		ast->Params.push_back(shared_ptr<BaseAST>($2));
		$$ = ast;
	}
	| ',' Exp FuncRParams_Wrap { 
		auto ast = reinterpret_cast<FuncPrototype*>$3;
		ast->Params.insert(ast->Params.begin(), shared_ptr<BaseAST>($2));
		$$ = ast;
	}

MulExp
	: UnaryExp { $$ = $1; }
	| MulExp '*' UnaryExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "MulExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Right_exp = shared_ptr<BaseAST>($3);
		ast->Operator = "*";
		$$ = ast;
	}
	| MulExp '/' UnaryExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "MulExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Right_exp = shared_ptr<BaseAST>($3);
		ast->Operator = "/";
		$$ = ast;
	}
	| MulExp '%' UnaryExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "MulExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Right_exp = shared_ptr<BaseAST>($3);
		ast->Operator = "%";
		$$ = ast;
	}
	;

AddExp
	: MulExp { $$ = $1; }
	| AddExp '+' MulExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "AddExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Right_exp = shared_ptr<BaseAST>($3);
		ast->Operator = "+";
		$$ = ast;
	}
	| AddExp '-' MulExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "AddExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Right_exp = shared_ptr<BaseAST>($3);
		ast->Operator = "-";
		$$ = ast;
	}
	;

RelExp
	: AddExp { $$ = $1; }
	| RelExp LT AddExp {
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "RelExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "<";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	| RelExp GT AddExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "RelExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = ">";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	| RelExp LE AddExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "RelExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "<=";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	| RelExp GE AddExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "RelExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = ">=";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	;

EqExp
	: RelExp { $$ = $1; }
	| EqExp EQ RelExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "EqExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "==";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	| EqExp NE RelExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "EqExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "!=";
		ast->Right_exp = shared_ptr<BaseAST>($3);
		$$ = ast;
	}
	;

LAndExp
	: EqExp { $$ = $1; }
	| LAndExp '&' '&' EqExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "LAndExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "&&";
		ast->Right_exp = shared_ptr<BaseAST>($4);
		$$ = ast;
	}
	;

LOrExp
	: LAndExp { $$ = $1; }
	| LOrExp '|' '|' LAndExp { 
		auto ast = new Exp();
		ast->AST_type = EXP;
		ast->Name = "LOrExp";
		ast->Left_exp = shared_ptr<BaseAST>($1);
		ast->Operator = "||";
		ast->Right_exp = shared_ptr<BaseAST>($4);
		$$ = ast;
	}
	;

%%

// error process function, second parameter is error message
void yyerror(shared_ptr<BaseAST> &ast, myScanner &scanner, const char *s) {
	cerr << "error: " << s << " at line " << yylineno << ":" << yycolumn << endl;
}