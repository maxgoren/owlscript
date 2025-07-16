#ifndef match_h
#define match_h
#ifdef __cplusplus
extern "C" {
#endif
#include "../src/re_to_dfa.h"

bool simulateDFA(DFA dfa, char* text);
bool matchDFA(char* re, char *text);

#ifdef __cplusplus
}
#endif
#endif