#include "intset.h"

void initSet(Set* set, int maxsize) {
    set->maxN = maxsize;
    set->n = 0;
    set->members = malloc(sizeof(int)*maxsize);
}

Set* createSet(int size) {
    Set* s = malloc(sizeof(Set));
    initSet(s, size);
    return s;
}

void freeSet(Set* set) {
    if (set == NULL)
        return;
    free(set->members);
    free(set);
}

Set* resizeSet(Set* set, int newsize) {
    int* nm = malloc(sizeof(int)*newsize);
    for (int i = 0; i < set->n; i++) {
        nm[i] = set->members[i];
    }
    free(set->members);
    set->members = nm;
    set->maxN = newsize;
    return set;
}

Set* copySet(Set* a) {
    Set* copy = createSet(a->maxN);
    for (int i = 0; i < a->n; i++) {
        copy->members[copy->n++] = a->members[i];
    }
    return copy;
}

bool isSetEmpty(Set* s) {
    return s->n == 0;
}


Set* setUnion(Set* a, Set* b) {
    Set* c = createSet(a->n + b->n + 1);
    int i = 0, j = 0; 
    while (i < a->n && j < b->n) {
        if (a->members[i] < b->members[j]) {
            setAdd(c, a->members[i++]);
        } else if (a->members[i] > b->members[j]) {
            setAdd(c, b->members[j++]);
        } else {
            setAdd(c, a->members[i++]);
        }
    }
    while (i < a->n) setAdd(c, a->members[i++]);
    while (j < b->n) setAdd(c, b->members[j++]);
    return c;
}

int setContains(Set* set, int value) {
    int l = 0, r = set->n-1;
    while (l <= r) {
        int m = (l+r)/2;
        if (value < set->members[m]) r = m - 1;
        else if (value > set->members[m]) l = m + 1;
        else return m;
    }
    return -1;
}

void setAdd(Set* set, int value) {
    int i = setContains(set, value);
    if (i == -1) {
        int j = set->n;
        while (j > 0 && set->members[j-1] > value) {
            set->members[j] = set->members[j-1];
            j--;
        }
        set->members[j] = value;
        set->n++;
    }
}

bool setsEqual(Set* a, Set* b) {
    if (a->n != b->n)
        return false;
    for (int j = 0; j < b->n; j++) {
        if (a->members[j] != b->members[j])
            return false;
    }
    return true;
}

void printSet(Set* set) {
    for (int i = 0; i < set->n; i++) {
        printf("%d ", set->members[i]);
    }
    printf("\n");
}