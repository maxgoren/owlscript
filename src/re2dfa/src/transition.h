#ifndef transition_h
#define transition_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>

typedef struct Transition_ {
    char ch;
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

//For maintaining Transitions as an AVL tree
int height(Transition* node);
int max(int a, int b);
Transition* rotL(Transition* node);
Transition* rotR(Transition* node);
int balanceFactor(Transition* node);

#ifdef __cplusplus
}
#endif

#endif