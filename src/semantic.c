#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"

/* ─────────────────────────────────────────────
   Simple symbol table (fixed-size hash map)
   ───────────────────────────────────────────── */
#define SYM_CAP 256

typedef struct Symbol {
    char          *name;
    struct Symbol *next;
} Symbol;

static Symbol *table[SYM_CAP];
static int     error_count = 0;

static unsigned int sym_hash(const char *s) {
    unsigned int h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h % SYM_CAP;
}

static int sym_exists(const char *name) {
    Symbol *s = table[sym_hash(name)];
    while (s) {
        if (strcmp(s->name, name) == 0) return 1;
        s = s->next;
    }
    return 0;
}

static void sym_insert(const char *name) {
    unsigned int h = sym_hash(name);
    Symbol *s = malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->next = table[h];
    table[h] = s;
}

static void sym_free_all(void) {
    for (int i = 0; i < SYM_CAP; i++) {
        Symbol *s = table[i];
        while (s) {
            Symbol *tmp = s->next;
            free(s->name);
            free(s);
            s = tmp;
        }
        table[i] = NULL;
    }
}

/* ─────────────────────────────────────────────
   Recursive checkers
   ───────────────────────────────────────────── */
static void check_expr(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_NUMBER:
            break;
        case NODE_IDENTIFIER:
            if (!sym_exists(node->sval)) {
                fprintf(stderr,
                    "Semantic error: variable '%s' used before assignment\n",
                    node->sval);
                error_count++;
            }
            break;
        case NODE_BINOP:
            check_expr(node->left);
            check_expr(node->right);
            break;
        case NODE_ASSIGN:
        case NODE_STMTLIST:
            /* should not appear as an expression */
            break;
    }
}

static void check_stmt(ASTNode *node) {
    if (!node) return;
    if (node->type == NODE_ASSIGN) {
        /* Check RHS first, then define LHS */
        check_expr(node->right);
        sym_insert(node->left->sval);
    }
}

static void check_stmtlist(ASTNode *node) {
    if (!node) return;
    if (node->type == NODE_STMTLIST) {
        check_stmt(node->left);
        check_stmtlist(node->right);
    }
}

/* ─────────────────────────────────────────────
   Public entry point
   ───────────────────────────────────────────── */
int semantic_check(ASTNode *root) {
    memset(table, 0, sizeof(table));
    error_count = 0;

    check_stmtlist(root);

    sym_free_all();

    if (error_count == 0) {
        printf("[Semantic] OK – no errors found\n");
    } else {
        fprintf(stderr, "[Semantic] %d error(s) found\n", error_count);
    }
    return error_count;
}
