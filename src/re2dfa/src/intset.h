#ifndef intset_h
#define intset_h
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int* members;
    int n;
    int maxN;
} Set;

void initSet(Set* set, int maxsize);
Set* createSet(int size);
void freeSet(Set* set);
Set* resizeSet(Set* set, int newsize);
Set* copySet(Set* a);
bool isSetEmpty(Set* s);
int setContains(Set* set, int value);
void setAdd(Set* set, int value);
Set* setUnion(Set* a, Set* b);
bool setsEqual(Set* a, Set* b);
void printSet(Set* set);
#ifdef __cplusplus
}
#endif
#endif