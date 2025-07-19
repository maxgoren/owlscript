#include <stdio.h>
#include <stdlib.h>
#include "match.h"

bool simulateDFA(DFA dfa, char* text) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", state->label, *sp);
#endif
        DFAState* next = NULL;
        Transition* it = findTransition(dfa.dtrans[state->label], *sp);
        if (it != NULL) {
            if (*sp == it->ch || ast_node_table[state->label]->token.symbol == RE_PERIOD) {
                next = dfa.states[it->to];
            }
        } else if (ast_node_table[state->label]->token.symbol == RE_PERIOD) {
            it = findTransition(dfa.dtrans[state->label], '.');
            if (it != NULL)
                next = dfa.states[it->to];
        }    
        if (!next) {
#ifdef DEBUG
            printf("Out of transitions, No match.\n");
#endif
            return false;
        } else {
            if (next->is_accepting) {
                printf("Ayo, Match found gang.\n");
            }
        }
        state = next;
    }
#ifdef DEBUG
    printf("Final State: %d\n", state->label);
#endif
    return state->is_accepting;
}


bool matchDFA(char* re, char *text) {
    re = augmentRE(re);
    re_ast* ast = re2ast(re);
    DFA dfa = re2dfa(re, ast);
#ifdef DEBUG
    printf("AST: \n");
    printAST(ast, 1); 
    printf("Followpos Table: \n");   
    for (int i = 1; i < numleaves+1; i++) {
        printf("%d: ", i);
        printSet(ast_node_table[i]->followpos);
    }
    printf("DFA: \n");
    printDFA(dfa);
#endif
    bool ans = simulateDFA(dfa, text);
    cleanup(&dfa, ast);
    return ans;
}
