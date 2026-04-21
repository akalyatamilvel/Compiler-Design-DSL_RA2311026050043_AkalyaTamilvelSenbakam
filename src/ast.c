#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ─────────────────────────────────────────────
   Internal allocator
   ───────────────────────────────────────────── */
static ASTNode *new_node(NodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->type = type;
    return n;
}

/* ─────────────────────────────────────────────
   Constructors
   ───────────────────────────────────────────── */
ASTNode *make_number(int value) {
    ASTNode *n = new_node(NODE_NUMBER);
    n->ival = value;
    return n;
}

ASTNode *make_identifier(const char *name) {
    ASTNode *n = new_node(NODE_IDENTIFIER);
    n->sval = strdup(name);
    return n;
}

ASTNode *make_assign(ASTNode *ident, ASTNode *expr) {
    ASTNode *n = new_node(NODE_ASSIGN);
    n->left  = ident;
    n->right = expr;
    return n;
}

ASTNode *make_binop(char op, ASTNode *left, ASTNode *right) {
    ASTNode *n = new_node(NODE_BINOP);
    n->op    = op;
    n->left  = left;
    n->right = right;
    return n;
}

ASTNode *make_stmtlist(ASTNode *stmt, ASTNode *rest) {
    /* Link stmt → rest via ->next and wrap in a STMTLIST node */
    ASTNode *n = new_node(NODE_STMTLIST);
    n->left = stmt;
    n->right = rest;   /* may be NULL */
    return n;
}

/* ─────────────────────────────────────────────
   Debug printer
   ───────────────────────────────────────────── */
void print_ast(ASTNode *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth * 2; i++) putchar(' ');

    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER(%d)\n", node->ival);
            break;
        case NODE_IDENTIFIER:
            printf("IDENT(%s)\n", node->sval);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN\n");
            print_ast(node->left,  depth + 1);
            print_ast(node->right, depth + 1);
            break;
        case NODE_BINOP:
            printf("BINOP('%c')\n", node->op);
            print_ast(node->left,  depth + 1);
            print_ast(node->right, depth + 1);
            break;
        case NODE_STMTLIST:
            printf("STMTLIST\n");
            print_ast(node->left,  depth + 1);
            print_ast(node->right, depth + 1);
            break;
    }
}

/* ─────────────────────────────────────────────
   Memory cleanup
   ───────────────────────────────────────────── */
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->sval) free(node->sval);
    free(node);
}
