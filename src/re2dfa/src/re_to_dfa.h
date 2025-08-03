#ifndef re_to_dfa_h
#define re_to_dfa_h
#include "re2ast/src/parser.h"
#include "followpos.h"
#include "dfa.h"
#ifdef __cplusplus
extern "C" {
#endif

char* augmentRE(char* orig);
DFA ast2dfa(char* re, re_ast* ast, re_ast*** ast_node_table);
void cleanup(DFA* dfa, re_ast* ast);

#ifdef __cplusplus
}
#endif

#endif