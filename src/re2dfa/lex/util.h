#ifndef util_h
#define util_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"

char *slurp_file(const char *filename);
void serialize_dfa_state(DFAState* state, int from, FILE* fd);
void dfa2matrix(DFA* d);
void dfa2json(DFA* d);

#ifdef __cplusplus
}
#endif

#endif