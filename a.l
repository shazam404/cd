%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
int line_num = 1;
%}

%option noyywrap

%%

"/*"([^*]|\*+[^*/])*\*+"/"  { }
"//".*                      { }

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"int"       { return INT; }
"float"     { return FLOAT; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }
[0-9]+                  { return NUM; }
[0-9]+\.[0-9]+          { return FNUM; }

"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MUL; }
"/"     { return DIV; }
"="     { return ASSIGN; }
"=="    { return EQ; }
"!="    { return NE; }
"<"     { return LT; }
">"     { return GT; }
"<="    { return LE; }
">="    { return GE; }
"++"    { return INC; }
"--"    { return DEC; }

"("     { return '('; }
")"     { return ')'; }
"{"     { return '{'; }
"}"     { return '}'; }
";"     { return ';'; }
","     { return ','; }

[ \t]+  { }
\n      { line_num++; }
.       { }

%%
