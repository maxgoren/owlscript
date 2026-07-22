#ifndef callframe_hpp
#define callframe_hpp
#include <iostream>
#include "stackitem.hpp"
#include "instruction.hpp"
#include "gcobject.hpp"

static const int MAX_LOCAL = 255;

struct ActivationRecord : GCObject {
    int cp_index;
    int ret_addr;
    int num_locals;
    StackItem* locals;
    ActivationRecord* control;
    ActivationRecord* access;
    ActivationRecord(int nl = 10, int idx = -1, int ra = 0, ActivationRecord* calling = nullptr, ActivationRecord* defining = nullptr) {
        cp_index = idx;
        ret_addr = ra;
        control = calling;
        access = defining;
        isAR = true;
        num_locals = nl;
        locals = new StackItem[nl];
        alloc.registerObject(this);
    }
    ActivationRecord(const ActivationRecord& ar) {
        cp_index = ar.cp_index;
        ret_addr = ar.ret_addr;
        control = ar.control;
        access = ar.access;
        isAR = true;
        marked = ar.marked;
        num_locals = ar.num_locals;
        locals = new StackItem[num_locals];
        for (int i = 0; i < num_locals; i++)
            locals[i] = ar.locals[i];
    }
    ~ActivationRecord() {
        delete [] locals;
    }
    ActivationRecord& operator=(const ActivationRecord& ar) {
        if (this == &ar)
            return *this;
        cp_index = ar.cp_index;
        ret_addr = ar.ret_addr;
        control = ar.control;
        access = ar.access;
        isAR = true;
        marked = ar.marked;
        num_locals = ar.num_locals;
        locals = new StackItem[num_locals];
        for (int i = 0; i < num_locals; i++)
            locals[i] = ar.locals[i];
        return *this;
    }
};

void freeAR(ActivationRecord* to);

#endif