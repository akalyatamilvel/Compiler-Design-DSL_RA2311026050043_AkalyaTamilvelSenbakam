%{
/*  parser.y  –  Bison grammar for the DSL  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Root of the AST – filled in by the top-level rule */
ASTNode *ast_root = NULL;

extern int  yylex(void);
extern int  line_num;
void yyerror(const char *msg);
%}

/* ─────────────────────────────────────────────
   Semantic value union
   ───────────────────────────────────────────── */
%union {
    int      ival;
    char    *sval;
    ASTNode *node;
}

/* ─────────────────────────────────────────────
   Token declarations
   ───────────────────────────────────────────── */
%token <ival> NUMBER
%token <sval> IDENTIFIER
%token ASSIGN SEMI LPAREN RPAREN
%token PLUS MINUS TIMES DIVIDE

/* ─────────────────────────────────────────────
   Operator precedence (lowest → highest)
   ───────────────────────────────────────────── */
%left  PLUS MINUS
%left  TIMES DIVIDE
%right UMINUS

/* ─────────────────────────────────────────────
   Non-terminal types
   ───────────────────────────────────────────── */
%type <node> program stmtlist stmt expr

%%

/* ─────────────────────────────────────────────
   Grammar rules
   ───────────────────────────────────────────── */

program
    : stmtlist          { ast_root = $1; }
    ;

stmtlist
    : stmt              { $$ = make_stmtlist($1, NULL); }
    | stmt stmtlist     { $$ = make_stmtlist($1, $2);   }
    ;

stmt
    : IDENTIFIER ASSIGN expr SEMI
        {
            ASTNode *id = make_identifier($1);
            free($1);
            $$ = make_assign(id, $3);
        }
    ;

expr
    : expr PLUS   expr  { $$ = make_binop('+', $1, $3); }
    | expr MINUS  expr  { $$ = make_binop('-', $1, $3); }
    | expr TIMES  expr  { $$ = make_binop('*', $1, $3); }
    | expr DIVIDE expr  { $$ = make_binop('/', $1, $3); }
    | MINUS expr %prec UMINUS
        {
            /* Represent unary minus as (0 - expr) */
            $$ = make_binop('-', make_number(0), $2);
        }
    | LPAREN expr RPAREN { $$ = $2; }
    | NUMBER              { $$ = make_number($1);        }
    | IDENTIFIER
        {
            $$ = make_identifier($1);
            free($1);
        }
    ;

%%

/* ─────────────────────────────────────────────
   Error handler
   ───────────────────────────────────────────── */
void yyerror(const char *msg) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_num, msg);
    exit(1);
}
