#ifndef dfa_h
#define dfa_h
#include <stdlib.h>
#include <stdio.h>
#include "intset.h"
#include "dfastate.h"
#include "followpos.h"
#include "transition.h"
#include "re2ast/src/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    DFAState** states;
    int numstates;
} DFA;


void initDFA(DFA* dfa, int numstates);
void initAlphabet(char* alphabet, char* re);
void addState(DFA* dfa, DFAState* state);
int nextStateNum(DFA* dfa);
Set* calculateNextStatesPositions(DFAState* curr_state, char input_symbol, re_ast** ast_node_table);
int findStateByPositions(DFA* dfa, Set* next_states);
int symbolIsInAlphabet(char* str, int n, char c);
DFA buildDFA(re_ast* ast, char* re, re_ast** ast_node_table);
DFAState* markAcceptState(DFAState* state, re_ast** ast_node_table);
void printDFA(DFA dfa);

#ifdef __cplusplus
}
#endif

#endif