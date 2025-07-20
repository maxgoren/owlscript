#ifndef parser_h
#define parser_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
#include "ast.h"

char* addConCat(char* str);

bool leftAssoc(char c);

int precedence(char c);

bool isOp(REToken c);

REToken* in2post(REToken* input);

re_ast* cloneTree(re_ast* node);

re_ast* makeTree(REToken* tokens);

re_ast* re2ast(char* regex);
#ifdef __cplusplus
}
#endif
#endif