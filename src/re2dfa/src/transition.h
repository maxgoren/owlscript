#ifndef transition_h
#define transition_h
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>

typedef struct Transition_ {
    int from;
    char ch;
    int to;
    int height;
    struct Transition_* left;
    struct Transition_* right;
} Transition;

Transition* makeTransition(int from, int to, char ch);
Transition* addTransition(Transition* trans, int from, int to, char ch);
Transition* findTransition(Transition* head, char ch);
void cleanTransTree(Transition* node);
int printTransitions(Transition* node);
#ifdef __cplusplus
}
#endif

#endif