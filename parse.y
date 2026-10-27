%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

/* External functions declared in codegen.c */
extern void init_code();
extern void end_code();
extern void gen_assign(char *id, char *val);
extern char* gen_arith(char *op, char *op1, char *op2);
extern char* gen_relop(char *op, char *op1, char *op2);
extern void gen_if_start(char *cond, int label_id);
extern void gen_if_else(int label_id);
extern void gen_if_end(int label_id);
extern void gen_while_start(int label_id);
extern void gen_while_cond(char *cond, int label_id);
extern void gen_while_end(int label_id);

int label_counter = 0;
%}

%union {
    char *str;
    int lbl;
}

%token <str> ID NUMBER
%token IF ELSE WHILE
%token EQ NE LE GE LT GT
%token ASSIGN ADD SUB MUL DIV AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE SEMI

%type <str> expr rel_expr
%type <lbl> if_stmt while_stmt

%left OR
%left AND
%right NOT
%nonassoc EQ NE LE GE LT GT
%left ADD SUB
%left MUL DIV

%%

program:
    stmt_list { end_code(); }
    ;

stmt_list:
    stmt_list stmt
    | stmt
    ;

stmt:
    assign_stmt
    | if_stmt
    | while_stmt
    | compound_stmt
    ;

compound_stmt:
    LBRACE stmt_list RBRACE
    ;

assign_stmt:
    ID ASSIGN expr SEMI { gen_assign($1, $3); free($1); free($3); }
    ;

if_stmt:
    IF LPAREN rel_expr RPAREN {
        $<lbl>$ = ++label_counter;
        gen_if_start($3, $<lbl>$);
        free($3);
    } stmt {
        gen_if_else($<lbl>5);
    } ELSE stmt {
        gen_if_end($<lbl>5);
    }
    | IF LPAREN rel_expr RPAREN {
        $<lbl>$ = ++label_counter;
        gen_if_start($3, $<lbl>$);
        free($3);
    } stmt {
        gen_if_end($<lbl>5);
    }
    ;

while_stmt:
    WHILE {
        $<lbl>$ = ++label_counter;
        gen_while_start($<lbl>$);
    } LPAREN rel_expr RPAREN {
        gen_while_cond($4, $<lbl>2);
        free($4);
    } stmt {
        gen_while_end($<lbl>2);
    }
    ;

expr:
    expr ADD expr    { $$ = gen_arith("ADD", $1, $3); free($1); free($3); }
    | expr SUB expr  { $$ = gen_arith("SUB", $1, $3); free($1); free($3); }
    | expr MUL expr  { $$ = gen_arith("MUL", $1, $3); free($1); free($3); }
    | expr DIV expr  { $$ = gen_arith("DIV", $1, $3); free($1); free($3); }
    | LPAREN expr RPAREN { $$ = $2; }
    | ID             { $$ = $1; }
    | NUMBER         { $$ = $1; }
    ;

rel_expr:
    expr EQ expr     { $$ = gen_relop("JE", $1, $3); free($1); free($3); }
    | expr NE expr   { $$ = gen_relop("JNE", $1, $3); free($1); free($3); }
    | expr LT expr   { $$ = gen_relop("JL", $1, $3); free($1); free($3); }
    | expr LE expr   { $$ = gen_relop("JLE", $1, $3); free($1); free($3); }
    | expr GT expr   { $$ = gen_relop("JG", $1, $3); free($1); free($3); }
    | expr GE expr   { $$ = gen_relop("JGE", $1, $3); free($1); free($3); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}

int main() {
    init_code();
    yyparse();
    return 0;
}
