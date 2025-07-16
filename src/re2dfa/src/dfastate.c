#include "dfastate.h"

void initDState(DFAState* state, int statenum) {
    state->label = statenum;
    state->positions = NULL;
    state->is_accepting = false;
    state->token_id = -1;
}

DFAState* createState(int statenum, Set* positions) {
    DFAState* ns = malloc(sizeof(DFAState));
    initDState(ns, statenum);
    ns->positions = positions;
    return ns;
}

qnode* makeQueueNode(DFAState* state) {
    qnode* qn = malloc(sizeof(qnode));
    qn->state = state;
    qn->next = NULL;
    return qn;
}

void initQueue(StateQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

bool emptyQueue(StateQueue* q) {
    return q->count == 0;
}

void enQueue(StateQueue* q, DFAState* state) {
    qnode* qn = makeQueueNode(state);
    if (emptyQueue(q)) {
        q->head = qn;
    } else {
        q->tail->next = qn;
    }
    q->tail = qn;
    q->count++;
}

DFAState* peekQueue(StateQueue* q) {
    return q->head->state;
}

DFAState* deQueue(StateQueue* q) {
    DFAState* ret = peekQueue(q);
    qnode* tmp = q->head;
    q->head = q->head->next;
    tmp->next = NULL; q->count--;
    free(tmp);
    return ret;
}