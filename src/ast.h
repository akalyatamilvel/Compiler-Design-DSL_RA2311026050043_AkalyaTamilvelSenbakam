#ifndef AST_H
#define AST_H

/* ─────────────────────────────────────────────
   AST Node Types
   ───────────────────────────────────────────── */
typedef enum {
    NODE_NUMBER,      /* integer literal          */
    NODE_IDENTIFIER,  /* variable name            */
    NODE_ASSIGN,      /* x = expr                 */
    NODE_BINOP,       /* expr OP expr             */
    NODE_STMTLIST     /* list of statements       */
} NodeType;

/* ─────────────────────────────────────────────
   AST Node Structure
   ───────────────────────────────────────────── */
typedef struct ASTNode {
    NodeType type;

    /* Leaf values */
    int      ival;          /* NODE_NUMBER      */
    char    *sval;          /* NODE_IDENTIFIER  */
    char     op;            /* NODE_BINOP       */

    /* Children */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;   /* sibling in STMTLIST */
} ASTNode;

/* ─────────────────────────────────────────────
   Constructor helpers
   ───────────────────────────────────────────── */
ASTNode *make_number(int value);
ASTNode *make_identifier(const char *name);
ASTNode *make_assign(ASTNode *ident, ASTNode *expr);
ASTNode *make_binop(char op, ASTNode *left, ASTNode *right);
ASTNode *make_stmtlist(ASTNode *stmt, ASTNode *rest);

/* Debug print */
void print_ast(ASTNode *node, int depth);

/* Free tree */
void free_ast(ASTNode *node);

#endif /* AST_H */
