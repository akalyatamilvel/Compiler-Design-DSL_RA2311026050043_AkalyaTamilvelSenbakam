#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

/* Declared by Bison-generated parser */
extern int    yyparse(void);
extern FILE  *yyin;
extern ASTNode *ast_root;

/* ─────────────────────────────────────────────
   Banner helper
   ───────────────────────────────────────────── */
static void banner(const char *title) {
    printf("\n┌─────────────────────────────────────────┐\n");
    printf("│  %-39s │\n", title);
    printf("└─────────────────────────────────────────┘\n");
}

int main(int argc, char *argv[]) {
    /* ── 0. Open source file ─────────────────── */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.dsl>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror(argv[1]);
        return 1;
    }

    printf("DSL Compiler  –  source: %s\n", argv[1]);

    /* ── 1. Lexing + Parsing ─────────────────── */
    banner("Phase 1 & 2 : Lexing / Parsing");
    yyparse();                       /* builds ast_root */
    fclose(yyin);
    printf("[Parser] AST constructed successfully\n");

    /* ── 2. Print AST ────────────────────────── */
    banner("Phase 3 : Abstract Syntax Tree");
    print_ast(ast_root, 0);

    /* ── 3. Semantic analysis ────────────────── */
    banner("Phase 4 : Semantic Analysis");
    int sem_errors = semantic_check(ast_root);
    if (sem_errors) {
        free_ast(ast_root);
        return 1;
    }

    /* ── 4. Code generation ──────────────────── */
    banner("Phase 5 : Intermediate Code Generation");
    codegen(ast_root, stdout);

    /* ── 5. Optionally write codegen to file ─── */
    if (argc >= 3) {
        FILE *f = fopen(argv[2], "w");
        if (f) {
            codegen(ast_root, f);
            fclose(f);
            printf("\n[CodeGen] Output also written to: %s\n", argv[2]);
        }
    }

    /* ── Cleanup ─────────────────────────────── */
    free_ast(ast_root);
    printf("\n[Done] Compilation finished successfully.\n\n");
    return 0;
}
