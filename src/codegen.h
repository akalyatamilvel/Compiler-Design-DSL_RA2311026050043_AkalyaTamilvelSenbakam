#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

/* Emit three-address code to the given file (use stdout if NULL) */
void codegen(ASTNode *root, FILE *out);

#endif
