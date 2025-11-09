%{
#include <stdio.h>
#include <string.h>
extern int line_num;
extern char *yytext;
int yylex();
void yyerror(const char *s);

char symtab[100][20];
char types[100][10];
int lines[100];
int symcount = 0;
char *current_type;

void add_symbol(char *name, char *type, int lineno) {
    for(int i = 0; i < symcount; i++) {
        if(strcmp(symtab[i], name) == 0) return;
    }
    strcpy(symtab[symcount], name);
    strcpy(types[symcount], type);
    lines[symcount] = lineno;
    symcount++;
}

void print_symtab() {
    printf("\nSymbol Table:\n");
    printf("%-15s %-10s %-10s\n", "Name", "Type", "Line");
    printf("----------------------------------------\n");
    for(int i = 0; i < symcount; i++) {
        printf("%-15s %-10s %-10d\n", symtab[i], types[i], lines[i]);
    }
}
%}

%union {
    char *str;
}

%token IF ELSE WHILE FOR INT FLOAT
%token <str> ID
%token NUM FNUM
%token PLUS MINUS MUL DIV ASSIGN
%token EQ NE LT GT LE GE INC DEC

%type <str> type

%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV

%%

program: stmts { print_symtab(); } ;

stmts: | stmts stmt ;

stmt: expr ';' { printf("Expression\n"); }
    | decl
    | IF '(' expr ')' stmt { printf("IF\n"); }
    | IF '(' expr ')' stmt ELSE stmt { printf("IF-ELSE\n"); }
    | WHILE '(' expr ')' stmt { printf("WHILE\n"); }
    | FOR '(' expr ';' expr ';' expr ')' stmt { printf("FOR\n"); }
    | '{' stmts '}' { printf("Block\n"); }
    ;

decl: type { current_type = $1; } vars ';' { printf("Declaration\n"); } ;

type: INT { $$ = strdup("int"); } 
    | FLOAT { $$ = strdup("float"); } ;

vars: ID { add_symbol($1, current_type, line_num); }
    | vars ',' ID { add_symbol($3, current_type, line_num); } ;

expr: ID
    | NUM
    | FNUM
    | ID ASSIGN expr
    | expr PLUS expr
    | expr MINUS expr
    | expr MUL expr
    | expr DIV expr
    | expr EQ expr
    | expr NE expr
    | expr LT expr
    | expr GT expr
    | expr LE expr
    | expr GE expr
    | ID INC
    | ID DEC
    | INC ID
    | DEC ID
    | '(' expr ')'
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", line_num, s);
}

int main() {
    printf("C Parser\n");
    if (yyparse() == 0) printf("Success\n");
    else printf("Failed\n");
    return 0;
}
