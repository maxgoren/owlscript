#include <stdio.h>
#include <stdlib.h>
#include "match.h"

bool simulateDFA(DFA dfa, char* text, re_ast** ast_node_table) {
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
        }
        state = next;
    }
#ifdef DEBUG
    printf("Final State: %d\n", state->label);
#endif
    return state->is_accepting;
}


bool matchDFA(char* re, char *text) {
    if (all_literals(re)) {
        return kmpSearch(re, text);
    }
    re = augmentRE(re);
    re_ast** ast_node_table;
    re_ast* ast = re2ast(re);
    DFA dfa = re2dfa(re, ast, &ast_node_table);
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
    bool ans = simulateDFA(dfa, text, ast_node_table);
    cleanup(&dfa, ast);
    free(ast_node_table);
    return ans;
}

bool all_literals(char* text) {
    for (char *sp = text; *sp; sp++) {
        if (*sp == '(' || *sp == '[' || *sp == '|' || *sp == '*' || *sp == '+' || *sp == '?' || *sp == ']' || *sp == ')')
            if (*(sp-1) != '\\')
                return false;
    }
    return true;
}

char* buildSkip(char* pattern, int len) {
    char* skip = (char*)malloc(sizeof(char)*len);
    skip[0] = -1;
    for (int i = 0, j = -1; i < len; i++, j++, skip[i] = j)
        while ((j >= 0) && (pattern[i] != pattern[j]))
            j = skip[j];
    return skip;
}

bool kmpSearch(char* pattern, char* text) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    char* skip = buildSkip(pattern, pattern_len);
    int i, j;
    for (i = 0, j = 0; i < text_len && j < pattern_len; i++, j++) {
        while ((j >= 0) && (text[i] != pattern[j]))
            j = skip[j];
    }
    return j == pattern_len;
}