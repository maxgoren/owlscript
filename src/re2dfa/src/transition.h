#ifndef transition_h
#define transition_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Transition_ {
    char ch;
    bool wc_period;
    int to;
    int height;
    struct Transition_* left;
    struct Transition_* right;
} Transition;

Transition* makeTransition(int to, char ch);
Transition* addTransition(Transition* trans, int to, char ch);
Transition* findTransition(Transition* head, char ch);
void cleanTransTree(Transition* node);
int printTransitions(Transition* node, int src);

//For maintaining transition lists as an AVL tree
int height(Transition* node);
int max(int a, int b);
Transition* rotL(Transition* node);
Transition* rotR(Transition* node);
int balanceFactor(Transition* node);
Transition* balanceAVL(Transition* trans);

#ifdef __cplusplus
}
#endif

#endif