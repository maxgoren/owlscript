#ifndef re_to_dfa_h
#define re_to_dfa_h
#include "../re2ast/src/parser.h"
#include "followpos.h"
#include "dfa.h"
#ifdef __cplusplus
extern "C" {
#endif

char* augmentRE(char* orig);
DFA re2dfa(char* re, re_ast* ast);
bool simulateDFA(DFA dfa, char* text);
bool matchDFA(char* re, char *text);
void cleanup(DFA* dfa, re_ast* ast);

#ifdef __cplusplus
}
#endif

#endif