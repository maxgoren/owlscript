#ifndef dfastate_h
#define dfastate_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "intset.h"
#include "transition.h"

typedef struct {
    int label;
    int token_id;
    bool is_accepting;
    int transition_count;
    Transition* transitions;
    Set* positions;
} DFAState;

typedef struct qnode_ {
    DFAState* state;
    struct qnode_* next;
} qnode;

typedef struct {
    qnode* head;
    qnode* tail;
    int count;
} StateQueue;

void initDState(DFAState* state, int statenum);
DFAState* createState(int statenum, Set* positions);
qnode* makeQueueNode(DFAState* state);
void initQueue(StateQueue* q);
bool emptyQueue(StateQueue* q);
void enQueue(StateQueue* q, DFAState* state);
DFAState* deQueue(StateQueue* q);
DFAState* peekQueue(StateQueue* q);
#ifdef __cplusplus
}
#endif
#endif