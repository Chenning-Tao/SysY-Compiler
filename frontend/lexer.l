%option noyywrap
%option nounput
%option noinput
%option yylineno

%{

#include <cstdlib>
#include <cstring>
#include "parser.tab.hpp"

using namespace std;

%}

/* TODO: add negative number */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*$

Identifier    [a-zA-Z_][a-zA-Z0-9_]*
String        \"(\\.|[^"\\])*\"

Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+
Float         [0-9]+\.[0-9]*

%%

{WhiteSpace}    { /* ignore */ }
{LineComment}   { /* ignore */ }

"int"           { return INT; }
"float"         { return FLOAT; }
"return"        { return RETURN; }
"const"         { return CONST; }
"void"          { return VOID; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"printf"        { return PRINTF; }
"scanf"         { return SCANF; }
"!="            { return NE; }
"=="            { return EQ; }
"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }
{String}        { char* tmp = new char[yyleng - 1]; strcpy(tmp, yytext + 1); tmp[yyleng-2] = '\0'; yylval.str_val = new string(tmp); return STRING; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Float}         { yylval.float_val = strtod(yytext, nullptr); return FLOAT_CONST; }

.               { return yytext[0]; }

%%