#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* Run semantic analysis; returns 0 on success, non-zero on error */
int semantic_check(ASTNode *root);

#endif
