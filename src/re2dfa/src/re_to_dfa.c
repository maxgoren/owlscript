#include "re_to_dfa.h"
#include "../re2ast/src/ast.h"
bool simulateDFA(DFA dfa, char* text) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", state->label, *sp);
#endif
        DFAState* next = NULL;
        for (Transition* it = dfa.dtrans[state->label]; it != NULL; it = it->next) {
            if (*sp == it->ch || ast_node_table[state->label]->token.symbol == RE_PERIOD) {
                next = dfa.states[it->to];
                break;
            }
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