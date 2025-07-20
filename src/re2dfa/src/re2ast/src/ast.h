#ifndef ast_h
#define ast_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
#include "../../intset.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct re_ast_ {
    int type;
    int number;
    int tk_token_id;
    REToken token;
    Set* firstpos;
    Set* lastpos;
    Set* followpos;
    struct re_ast_* left;
    struct re_ast_* right;
} re_ast;

re_ast* makeNode(int type, REToken tk);

void printAST(re_ast* node, int d);

void freeTree(re_ast* node);

re_ast* cloneTree(re_ast* node);

#ifdef __cplusplus
}
#endif

#endif