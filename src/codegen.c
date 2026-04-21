#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"

/* ─────────────────────────────────────────────
   Temporary variable counter
   ───────────────────────────────────────────── */
static int tmp_count = 0;

static char *new_temp(void) {
    /* Returns a string like "t0", "t1", … (caller must free) */
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", tmp_count++);
    return strdup(buf);
}

/* ─────────────────────────────────────────────
   Recursive code generator
   Returns the name of the variable/temp holding
   the result of this node (caller must free).
   ───────────────────────────────────────────── */
static char *gen_expr(ASTNode *node, FILE *out) {
    if (!node) return strdup("??");

    switch (node->type) {

        case NODE_NUMBER: {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", node->ival);
            return strdup(buf);
        }

        case NODE_IDENTIFIER:
            return strdup(node->sval);

        case NODE_BINOP: {
            char *lhs = gen_expr(node->left,  out);
            char *rhs = gen_expr(node->right, out);
            char *tmp = new_temp();
            fprintf(out, "    %s = %s %c %s\n", tmp, lhs, node->op, rhs);
            free(lhs);
            free(rhs);
            return tmp;
        }

        default:
            return strdup("??");
    }
}

static void gen_stmt(ASTNode *node, FILE *out) {
    if (!node) return;
    if (node->type != NODE_ASSIGN) return;

    char *result = gen_expr(node->right, out);
    fprintf(out, "    %s = %s\n", node->left->sval, result);
    free(result);
}

static void gen_stmtlist(ASTNode *node, FILE *out) {
    if (!node) return;
    if (node->type == NODE_STMTLIST) {
        gen_stmt(node->left, out);
        gen_stmtlist(node->right, out);
    }
}

/* ─────────────────────────────────────────────
   Public entry point
   ───────────────────────────────────────────── */
void codegen(ASTNode *root, FILE *out) {
    if (!out) out = stdout;
    tmp_count = 0;

    fprintf(out, "; ── Three-Address Code ───────────────────────\n");
    fprintf(out, "begin:\n");
    gen_stmtlist(root, out);
    fprintf(out, "end:\n");
    fprintf(out, "; ─────────────────────────────────────────────\n");
}
