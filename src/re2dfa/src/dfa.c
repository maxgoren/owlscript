#include "dfa.h"

Transition* makeTransition(int from, int to, char ch, Transition* set) {
    Transition* nt = malloc(sizeof(Transition));
    nt->from = from;
    nt->to = to;
    nt->ch = ch;
    nt->next = set;
    return nt;
}

void initDFA(DFA* dfa, int numstates) {
    dfa->numstates = 0;
    dfa->states = malloc(sizeof(DFAState*)*numstates+1);
    dfa->dtrans = malloc(sizeof(Transition*)*numstates+1);
    for (int i = 0; i < numstates;i++) {
        dfa->dtrans[i] = NULL;
    }
}

void addState(DFA* dfa, DFAState* state) {
    dfa->states[state->label] = state;
}

Transition* addTransition(Transition* trans, int from, int to, char ch) {
    for (Transition* it = trans; it != NULL; it = it->next) {
        if (it->from == from && it->to == to && it->ch == ch)
            return trans;
    }
    return makeTransition(from, to, ch, trans);
}

bool findInCharClass(char* ccl, char input_symbol) {
    for (char *sp = ccl; *sp; sp++) {
        if (*sp == input_symbol)
            return true;
    }
    return false;
}

Set* calculateNextStatesPositions(DFAState* curr_state, char input_symbol) {
    Set* next_states = createSet(nonleaves+1);
    for (int i = 0; i < curr_state->positions->n; i++) {
        int t = curr_state->positions->members[i];
#ifdef DEBUG
        if (ast_node_table[t]->token.symbol == RE_CCL) {
            printf("%c in %s?", input_symbol, ast_node_table[t]->token.ccl);
        } else {
            printf("%c == %c?",  ast_node_table[t]->token.ch, input_symbol);
        }
#endif
        if ((ast_node_table[t]->token.symbol == RE_CCL && findInCharClass(ast_node_table[t]->token.ccl, input_symbol)) || (ast_node_table[t]->token.symbol != RE_CCL && ast_node_table[t]->token.ch == input_symbol || ast_node_table[t]->token.symbol == RE_PERIOD)) {
#ifdef DEBUG
            printf(" Yep. Adding Set: %d\n",t);
#endif
            next_states = setUnion(next_states, ast_node_table[t]->followpos);
        } else {
#ifdef DEBUG
            printf("Nope.\n");
#endif
        }
    }
    return next_states;
}

int findStateByPositions(DFA* dfa, Set* next_states) {
#ifdef DEBUG
    printf("\nChecking if next_state exists... ");
#endif
    for (int i = 1; i <= dfa->numstates; i++) {
        if (setsEqual(next_states, dfa->states[i]->positions)) {
#ifdef DEBUG
            //printf("Yep. ");
#endif
            return i;
        }
    }
    return -1;
}

int nextStateNum(DFA* dfa) {
    return ++dfa->numstates;
}

DFA buildDFA(re_ast* ast, char* re) {
    int found;
    int max_table_size = (strlen(re)+5);
    char* alphabet = malloc(sizeof(char)*max_table_size); 
    if (alphabet == NULL) {
        printf("Malloc failed :(\n");
        exit(EXIT_FAILURE);
    }
    DFA dfa;
    StateQueue fq;
    initAlphabet(ast, alphabet, re);
    initDFA(&dfa,numleaves+1);
    addState(&dfa, createState(nextStateNum(&dfa), copySet(ast->firstpos)));

    initQueue(&fq);
    enQueue(&fq,  dfa.states[1]);
    while (!emptyQueue(&fq)) {
        DFAState* curr_state = deQueue(&fq);
#ifdef DEBUG
        printf("Current State: %d\n", curr_state->label);
#endif
        for (char *input_symbol = alphabet; *input_symbol; input_symbol++) {
#ifdef DEBUG
            printf("Input Symbol: %c\n", *input_symbol);
#endif
            Set* next_states = calculateNextStatesPositions(curr_state, *input_symbol);
            if (!isSetEmpty(next_states)) {
                if ((found = findStateByPositions(&dfa, next_states)) > -1) {
                    dfa.dtrans[curr_state->label] = addTransition(dfa.dtrans[curr_state->label], curr_state->label, dfa.states[found]->label, *input_symbol);
#ifdef DEBUG                    
                    printf("State Already Exists, Adding Transition:  %d - (%c) - %d\n", curr_state->label, *input_symbol, dfa.states[found]->label);
#endif
                    freeSet(next_states);
                } else {
                    DFAState* new_state = createState(nextStateNum(&dfa), copySet(next_states)); 
                    addState(&dfa, new_state);
                    dfa.dtrans[curr_state->label] = addTransition(dfa.dtrans[curr_state->label], curr_state->label, new_state->label, *input_symbol);
                    enQueue(&fq, new_state);
#ifdef DEBUG
                    printf("Adding new state: %d, Transition: %d - (%c) - %d\n", new_state->label, curr_state->label, *input_symbol, new_state->label);
#endif
                }
            } else {
#ifdef DEBUG
                printf("\nNext states list was empty.\n");
#endif
                free(next_states);
            }
        }
#ifdef DEBUG
        printf("\n----------------------------------------\n");
#endif
    }
    for (int i = 1; i <= dfa.numstates; i++) {
        DFAState* next_state = dfa.states[i];
        for (int j = 0; j < next_state->positions->n; j++) {
            int pos = next_state->positions->members[j];
            if (ast_node_table[pos]->token.ch == '#')  {      
                next_state->is_accepting = true; 
                int leaf_id = ast_node_table[pos]->tk_token_id;
                if (next_state->token_id == -1 || leaf_id < next_state->token_id) {
                    next_state->token_id = leaf_id;
#ifdef DEBUG
                    printf("Marked %d as accepting statefor leaf id %d.\n", i, leaf_id);
#endif
                }
            }
        }
    }
    return dfa;
}

void printDFA(DFA dfa) {
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: ", i);
        printSet(dfa.states[i]->positions);
    }
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: \n", i);
        for (Transition* it = dfa.dtrans[i]; it != NULL; it = it->next) {
            printf("%d - (%c) - %d\n", it->from, it->ch, it->to);
        }
        printf("\n");
    }
}

int symbolIsInAlphabet(char* str, int n, char c) {
    for (int i = 0; i < n; i++) {
        if (str[i] == c)
            return i;
    }
    return -1;
}


void initAlphabet(re_ast* ast, char* alphabet,  char* re) {
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
