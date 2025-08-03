#include "dfa.h"

void initDFA(DFA* dfa, int numstates) {
    dfa->numstates = 0;
    dfa->states = malloc(sizeof(DFAState*)*numstates+1);
}

void addState(DFA* dfa, DFAState* state) {
    dfa->states[state->label] = state;
}

bool findInCharClass(char* ccl, char input_symbol) {
    for (char *sp = ccl; *sp; sp++) {
        if (*sp == input_symbol)
            return true;
    }
    return false;
}

Set* calculateNextStatesPositions(DFAState* curr_state, char input_symbol, re_ast** ast_node_table) {
    Set* next_states = createSet(nonleaves+1);
    for (int i = 0; i < curr_state->positions->n; i++) {
        int t = curr_state->positions->members[i];
        if ((ast_node_table[t]->token.symbol == RE_CCL && findInCharClass(ast_node_table[t]->token.ccl, input_symbol)) || 
            (ast_node_table[t]->token.symbol != RE_CCL && (ast_node_table[t]->token.ch == input_symbol || ast_node_table[t]->token.symbol == RE_PERIOD))) {
            next_states = setUnion(next_states, ast_node_table[t]->followpos);
        }
    }
    return next_states;
}

int findStateByPositions(DFA* dfa, Set* next_states) {
    for (int i = 1; i <= dfa->numstates; i++) {
        if (setsEqual(next_states, dfa->states[i]->positions)) {
            return i;
        }
    }
    return -1;
}

int nextStateNum(DFA* dfa) {
    return ++dfa->numstates;
}

DFA buildDFA(re_ast* ast, char* re, re_ast** ast_node_table) {
    int found;
    int max_table_size = (strlen(re)+5);
    char* alphabet = malloc(sizeof(char)*max_table_size); 
    if (alphabet == NULL) {
        printf("Malloc failed :(\n");
        exit(EXIT_FAILURE);
    }
    DFA dfa;
    StateQueue fq;
    initAlphabet(alphabet, re);
    initDFA(&dfa,numleaves+1);
    addState(&dfa, createState(nextStateNum(&dfa), copySet(ast->firstpos)));
    initQueue(&fq);
    enQueue(&fq,  dfa.states[1]);
    while (!emptyQueue(&fq)) {
        DFAState* curr_state = deQueue(&fq);
        for (char *input_symbol = alphabet; *input_symbol; input_symbol++) {
#ifdef DEBUG
            printf("Input Symbol: %c\n", *input_symbol);
#endif
            Set* next_states = calculateNextStatesPositions(curr_state, *input_symbol, ast_node_table);
            if (!isSetEmpty(next_states)) {
                if ((found = findStateByPositions(&dfa, next_states)) > -1) {
                    curr_state->transitions = addTransition(curr_state->transitions, dfa.states[found]->label, *input_symbol);
                    curr_state->transition_count += 1;
                    freeSet(next_states);
#ifdef DEBUG                    
                    printf("State Already Exists, Adding Transition:  %d - (%c) - %d\n", curr_state->label, *input_symbol, dfa.states[found]->label);
#endif
                } else {
                    DFAState* new_state = createState(nextStateNum(&dfa), copySet(next_states)); 
                    addState(&dfa, new_state);
                    curr_state->transitions = addTransition(curr_state->transitions, new_state->label, *input_symbol);
                    curr_state->transition_count += 1;
                    enQueue(&fq, new_state);
#ifdef DEBUG
                    printf("Adding new state: %d, Transition: %d - (%c) - %d\n", new_state->label, curr_state->label, *input_symbol, new_state->label);
#endif
                }
            } else {
                free(next_states);
            }
        }
    }
    for (int i = 1; i <= dfa.numstates; i++) {
        dfa.states[i] = markAcceptState(dfa.states[i], ast_node_table);
    }
    return dfa;
}

DFAState* markAcceptState(DFAState* next_state, re_ast** ast_node_table) {
    for (int j = 0; j < next_state->positions->n; j++) {
        int pos = next_state->positions->members[j];
        if (ast_node_table[pos]->token.ch == '#')  {      
            next_state->is_accepting = true; 
            int leaf_id = ast_node_table[pos]->tk_token_id;
            if (next_state->token_id == -1 || leaf_id < next_state->token_id) {
                next_state->token_id = leaf_id;
            }
        }
    }
    return next_state;
}

void printDFA(DFA dfa) {
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: ", i);
        printSet(dfa.states[i]->positions);
    }
    int mintc = 0, maxtc = 0, ttc = 0;
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: \n", i);
        int tc = printTransitions(dfa.states[i]->transitions, i);
        printf("\n");
        if (mintc == 0 || tc < mintc) mintc = tc;
        if (maxtc == 0 || tc > maxtc) maxtc = tc;
        ttc += tc;
        printf("\nTransition Count: %d\n", tc);
    }
    printf("Lowest transition count: %d\n", mintc);
    printf("Highest Trnaisiton count: %d\n", maxtc);
    printf("Num states: %d\n",dfa.numstates);
    printf("Total transitions: %d\n", ttc);
    printf("Possible Transitions: %d\n",(dfa.numstates+1)*128);
}

int symbolIsInAlphabet(char* str, int n, char c) {
    for (int i = 0; i < n; i++) {
        if (str[i] == c)
            return i;
    }
    return -1;
}


void initAlphabet(char* alphabet,  char* re) {
    int k = 0, p = 0;
#ifdef DEBUG
    printf("Getting alphabet from %s\n", re);
#endif
    for (int i = 0; re[i]; i++) {
        if (re[i] != '#' && (is_char(re[i]) || is_digit(re[i]) || is_special(re[i])) && symbolIsInAlphabet(alphabet, k, re[i]) == -1)
            alphabet[k++] = re[i];
    }
#ifdef DEBUG
    printf("Alphabet: %s \n", alphabet);
#endif
}

