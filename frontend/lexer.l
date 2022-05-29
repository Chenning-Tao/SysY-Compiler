%option noyywrap
%option nounput
%option noinput
%option yylineno
%option c++
%option yyclass="myScanner"

%{

#include <cstdlib>
#include <cstring>
#include "parser.tab.hpp"
#include "myScanner.hpp"
#include <cstring>
#include "macro.hpp"

#undef YY_DECL
#define YY_DECL int myScanner::yylex()

// extern int yyinput();
// extern void yyunput (int c, char * yy_bp );

using namespace std;

macro macro_list;

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

"#define" {
    int c = yyinput();
    string a, b;
    string var_list;
    bool flag = false;
    while((c = yyinput()) != '\n' && c != EOF) {
        if(c == ' ') flag = true;
        else if(flag == false) {
            if (c == '('){
                while((c = yyinput()) != ')') {
                    var_list += c;
                }
            }
            else a += c;
        }
        else b += c;
    }
    bool success = true;
    if(var_list.empty()) success = macro_list.add_macro(a, b);
    else success = macro_list.add_macro(a, b, var_list);
    if(!success) {
        cerr << "macro " << a << " has been defined" << endl;
        exit(1);
    }
}

"/*" {
        int c;
        while((c = yyinput()) != 0){
            if(c == '*'){
                if((c = yyinput()) == '/') break;
                else unput(c);
            }
        }
     }

{Identifier}    {
    int type = macro_list.function(yytext);
    cout << type;
    string s;
    if(type == 1) s = macro_list.get_macro(yytext);
    else if(type == 2) {
        int c = yyinput();
        string var_list;
        while((c = yyinput()) != ')') var_list += c;
        s = macro_list.get_macro(yytext, var_list);
    }
    else {
        yylval.str_val = new string(yytext); 
        return IDENT;
    }
    for(int i = s.size()-1; i >= 0; --i) unput(s[i]);
}
{String}        { char* tmp = new char[yyleng - 1]; strcpy(tmp, yytext + 1); tmp[yyleng-2] = '\0'; yylval.str_val = new string(tmp); return STRING; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Float}         { yylval.float_val = strtod(yytext, nullptr); return FLOAT_CONST; }

.               { return yytext[0]; }

%%