%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap

%%

[ \t\r\n]+          ; /* ignore whitespace */

"if"                { return IF; }
"else"              { return ELSE; }
"while"             { return WHILE; }

"=="                { return EQ; }
"!="                { return NE; }
"<="                { return LE; }
">="                { return GE; }
"<"                 { return LT; }
">"                 { return GT; }

"="                 { return ASSIGN; }
"+"                 { return ADD; }
"-"                 { return SUB; }
"*"                 { return MUL; }
"/"                 { return DIV; }

"&&"                { return AND; }
"||"                { return OR; }
"!"                 { return NOT; }

"("                 { return LPAREN; }
")"                 { return RPAREN; }
"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
";"                 { return SEMI; }

[0-9]+              { yylval.str = strdup(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return ID; }

.                   { printf("Lexical Error: Unknown character %s\n", yytext); }

%%
