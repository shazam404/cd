%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

int line_num = 1;
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
WHITESPACE  [ \t]+

%%

"/*"            {
                    int c;
                    while ((c = input()) != 0) {
                        if (c == '\n') line_num++;
                        if (c == '*') {
                            if ((c = input()) == '/') break;
                            else unput(c);
                        }
                    }
                }

"//".*          { }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }
"do"            { return DO; }
"int"           { return INT; }
"float"         { return FLOAT_TYPE; }
"char"          { return CHAR; }
"void"          { return VOID; }
"return"        { return RETURN; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"printf"        { return PRINTF; }
"scanf"         { return SCANF; }

{IDENTIFIER}    { yylval.str = strdup(yytext); return IDENTIFIER; }
{INTEGER}       { yylval.num = atoi(yytext); return INT_CONST; }
{FLOAT}         { yylval.fnum = atof(yytext); return FLOAT_CONST; }
\"([^\"\n])*\"  { yylval.str = strdup(yytext); return STRING_LITERAL; }
\'([^'\n])\'    { yylval.ch = yytext[1]; return CHAR_CONST; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"%"             { return MODULO; }
"="             { return ASSIGN; }
"=="            { return EQ; }
"!="            { return NE; }
"<"             { return LT; }
"<="            { return LE; }
">"             { return GT; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }
"++"            { return INCREMENT; }
"--"            { return DECREMENT; }
"+="            { return PLUS_ASSIGN; }
"-="            { return MINUS_ASSIGN; }
"*="            { return MULT_ASSIGN; }
"/="            { return DIV_ASSIGN; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
";"             { return SEMICOLON; }
","             { return COMMA; }

{WHITESPACE}    {  }
"\n"            { line_num++; }

.               { printf("Lexical error: Unrecognized character '%s' at line %d\n", yytext, line_num); }

%%
