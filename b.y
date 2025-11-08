%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int line_num;
int yylex();
void yyerror(const char *s);
%}

%union {
    int num;
    float fnum;
    char ch;
    char *str;
}

%token <str> IDENTIFIER STRING_LITERAL
%token <num> INT_CONST
%token <fnum> FLOAT_CONST
%token <ch> CHAR_CONST

%token IF ELSE WHILE FOR DO INT FLOAT_TYPE CHAR VOID RETURN BREAK CONTINUE PRINTF SCANF

%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULT_ASSIGN DIV_ASSIGN
%token EQ NE LT LE GT GE
%token AND OR NOT
%token INCREMENT DECREMENT

%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA

%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULT_ASSIGN DIV_ASSIGN
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT INCREMENT DECREMENT UNARY_MINUS
%left LPAREN RPAREN LBRACKET RBRACKET

%start program

%%

program:
    | program external_declaration
    ;

external_declaration:
    function_definition
    | declaration_stmt
    | statement
    ;

function_definition:
    type_specifier IDENTIFIER LPAREN parameter_list RPAREN compound_stmt
    { printf("Parsed function definition: %s\n", $2); }
    | type_specifier IDENTIFIER LPAREN RPAREN compound_stmt
    { printf("Parsed function definition: %s (no parameters)\n", $2); }
    ;

parameter_list:
    parameter
    | parameter_list COMMA parameter
    ;

parameter:
    type_specifier IDENTIFIER
    { printf("  - Parameter: %s\n", $2); }
    | type_specifier
    ;

statement_list:
    | statement_list statement
    ;

statement:
    expression_stmt
    | declaration_stmt
    | conditional_stmt
    | loop_stmt
    | jump_stmt
    | function_call_stmt
    | compound_stmt
    ;

compound_stmt:
    LBRACE statement_list RBRACE
    { printf("Parsed compound statement (block)\n"); }
    | LBRACE RBRACE
    { printf("Parsed empty compound statement\n"); }
    ;

expression_stmt:
    expression SEMICOLON
    { printf("Parsed expression statement\n"); }
    | SEMICOLON
    ;

declaration_stmt:
    type_specifier declarator_list SEMICOLON
    { printf("Parsed declaration statement\n"); }
    ;

type_specifier:
    INT
    | FLOAT_TYPE
    | CHAR
    | VOID
    ;

declarator_list:
    declarator
    | declarator_list COMMA declarator
    ;

declarator:
    IDENTIFIER
    { printf("    - Declared variable: %s\n", $1); }
    | IDENTIFIER ASSIGN expression
    { printf("    - Declared and initialized variable: %s\n", $1); }
    | IDENTIFIER LBRACKET INT_CONST RBRACKET
    { printf("    - Declared array: %s[%d]\n", $1, $3); }
    ;

conditional_stmt:
    IF LPAREN expression RPAREN statement
    { printf("Parsed if statement\n"); }
    | IF LPAREN expression RPAREN statement ELSE statement
    { printf("Parsed if-else statement\n"); }
    ;

loop_stmt:
    while_stmt
    | for_stmt
    | do_while_stmt
    ;

while_stmt:
    WHILE LPAREN expression RPAREN statement
    { printf("Parsed while loop\n"); }
    ;

for_stmt:
    FOR LPAREN expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RPAREN statement
    { printf("Parsed for loop\n"); }
    | FOR LPAREN declaration_stmt expression_opt SEMICOLON expression_opt RPAREN statement
    { printf("Parsed for loop with declaration\n"); }
    ;

do_while_stmt:
    DO statement WHILE LPAREN expression RPAREN SEMICOLON
    { printf("Parsed do-while loop\n"); }
    ;

expression_opt:
    | expression
    ;

jump_stmt:
    RETURN expression SEMICOLON
    { printf("Parsed return statement with value\n"); }
    | RETURN SEMICOLON
    { printf("Parsed return statement\n"); }
    | BREAK SEMICOLON
    { printf("Parsed break statement\n"); }
    | CONTINUE SEMICOLON
    { printf("Parsed continue statement\n"); }
    ;

function_call_stmt:
    function_call SEMICOLON
    { printf("Parsed function call statement\n"); }
    ;

function_call:
    IDENTIFIER LPAREN argument_list RPAREN
    { printf("    - Called function: %s\n", $1); }
    | IDENTIFIER LPAREN RPAREN
    { printf("    - Called function: %s (no arguments)\n", $1); }
    | PRINTF LPAREN argument_list RPAREN
    { printf("    - Called printf\n"); }
    | SCANF LPAREN argument_list RPAREN
    { printf("    - Called scanf\n"); }
    ;

argument_list:
    expression
    | argument_list COMMA expression
    ;

expression:
    assignment_expr
    ;

assignment_expr:
    conditional_expr
    | unary_expr ASSIGN assignment_expr
    { printf("    - Assignment operation (=)\n"); }
    | unary_expr PLUS_ASSIGN assignment_expr
    { printf("    - Compound assignment (+=)\n"); }
    | unary_expr MINUS_ASSIGN assignment_expr
    { printf("    - Compound assignment (-=)\n"); }
    | unary_expr MULT_ASSIGN assignment_expr
    { printf("    - Compound assignment (*=)\n"); }
    | unary_expr DIV_ASSIGN assignment_expr
    { printf("    - Compound assignment (/=)\n"); }
    ;

conditional_expr:
    logical_or_expr
    ;

logical_or_expr:
    logical_and_expr
    | logical_or_expr OR logical_and_expr
    { printf("    - Logical OR operation (||)\n"); }
    ;

logical_and_expr:
    equality_expr
    | logical_and_expr AND equality_expr
    { printf("    - Logical AND operation (&&)\n"); }
    ;

equality_expr:
    relational_expr
    | equality_expr EQ relational_expr
    { printf("    - Equality check (==)\n"); }
    | equality_expr NE relational_expr
    { printf("    - Inequality check (!=)\n"); }
    ;

relational_expr:
    additive_expr
    | relational_expr LT additive_expr
    { printf("    - Relational operation (<)\n"); }
    | relational_expr LE additive_expr
    { printf("    - Relational operation (<=)\n"); }
    | relational_expr GT additive_expr
    { printf("    - Relational operation (>)\n"); }
    | relational_expr GE additive_expr
    { printf("    - Relational operation (>=)\n"); }
    ;

additive_expr:
    multiplicative_expr
    | additive_expr PLUS multiplicative_expr
    { printf("    - Addition operation (+)\n"); }
    | additive_expr MINUS multiplicative_expr
    { printf("    - Subtraction operation (-)\n"); }
    ;

multiplicative_expr:
    unary_expr
    | multiplicative_expr MULTIPLY unary_expr
    { printf("    - Multiplication operation (*)\n"); }
    | multiplicative_expr DIVIDE unary_expr
    { printf("    - Division operation (/)\n"); }
    | multiplicative_expr MODULO unary_expr
    { printf("    - Modulo operation (%%)\n"); }
    ;

unary_expr:
    postfix_expr
    | INCREMENT unary_expr
    { printf("    - Pre-increment (++)\n"); }
    | DECREMENT unary_expr
    { printf("    - Pre-decrement (--)\n"); }
    | MINUS unary_expr %prec UNARY_MINUS
    { printf("    - Unary minus (-)\n"); }
    | NOT unary_expr
    { printf("    - Logical NOT (!)\n"); }
    ;

postfix_expr:
    primary_expr
    | postfix_expr INCREMENT
    { printf("    - Post-increment (++)\n"); }
    | postfix_expr DECREMENT
    { printf("    - Post-decrement (--)\n"); }
    | postfix_expr LBRACKET expression RBRACKET
    { printf("    - Array subscript []\n"); }
    | function_call
    ;

primary_expr:
    IDENTIFIER
    { printf("    - Identifier: %s\n", $1); }
    | INT_CONST
    { printf("    - Integer constant: %d\n", $1); }
    | FLOAT_CONST
    { printf("    - Float constant: %f\n", $1); }
    | CHAR_CONST
    { printf("    - Char constant: '%c'\n", $1); }
    | STRING_LITERAL
    { printf("    - String literal: %s\n", $1); }
    | LPAREN expression RPAREN
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", line_num, s);
}

int main(int argc, char **argv) {
    printf("===============================================\n");
    printf("        C Code Parser - Detailed Output\n");
    printf("===============================================\n");
    printf("Enter C code snippets (Press Ctrl+D to finish):\n\n");
    
    if (yyparse() == 0) {
        printf("\n===============================================\n");
        printf("Program parsed successfully!\n");
        printf("===============================================\n");
        return 0;
    } else {
        printf("\n===============================================\n");
        printf("✗ Parsing failed!\n");
        printf("===============================================\n");
        return 1;
    }
}
