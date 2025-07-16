#include "re_to_dfa.h"
#include "../re2ast/src/ast.h"

void cleanup(DFA* dfa, re_ast* ast) {
    freeTree(ast);
    for (int i = 1; i <= dfa->numstates; i++) {
        freeSet(dfa->states[i]->positions);
        free(dfa->states[i]);
        Transition* head = dfa->dtrans[i];
        while (head != NULL) {
            Transition* x = head;
            head = head->next;
            free(x);
        }
    }
    free(dfa->states);
    free(dfa->dtrans);
}

char* augmentRE(char* orig) {
    if (orig == NULL) return orig;
    char* fixed = malloc(sizeof(char)*strlen(orig)+4);
    sprintf(fixed, "(%s)#", orig);
    return fixed;
}

DFA re2dfa(char* re, re_ast* ast) {
    computeFollowPos(ast);
    char* no_concat = toString(in2post(tokenize(re)));
    DFA dfa = buildDFA(ast, no_concat);
    return dfa;
}