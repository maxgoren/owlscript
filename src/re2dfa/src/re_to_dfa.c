#include "re_to_dfa.h"
#include "re2ast/src/ast.h"

void cleanup(DFA* dfa, re_ast* ast) {
    freeTree(ast);
    for (int i = 1; i <= dfa->numstates; i++) {
        freeSet(dfa->states[i]->positions);
        free(dfa->states[i]);
        cleanTransTree(dfa->states[i]->transitions);
    }
    free(dfa->states);
}

char* augmentRE(char* orig) {
    if (orig == NULL) return orig;
    char* fixed = malloc(sizeof(char)*strlen(orig)+4);
    sprintf(fixed, "(%s)#", orig);
    return fixed;
}

DFA ast2dfa(char* re, re_ast* ast, re_ast*** ast_node_table) {
    computeFollowPos(ast, ast_node_table);
    char* no_concat = toString(in2post(tokenize(re)));
    DFA dfa = buildDFA(ast, no_concat, *ast_node_table);
    return dfa;
}