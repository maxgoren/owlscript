#ifndef gc_hpp
#define gc_hpp
#include <vector>
#include <memory>
#include "constpool.hpp"
#include "stackitem.hpp"
#include "instruction.hpp"
using namespace std;

class GarbageCollector {
    private:
        void markObject(GCItem*& curr);
        void markItem(StackItem* si) ;
        void markAR(ActivationRecord* callframe);
        void sweep() ;
        void markOpStack(StackItem ops[], int sp);
        void markConstPool(ConstPool* constPool);
        void markRoots(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool);
        unsigned int GC_LIMIT;
    public:
        GarbageCollector();
        bool ready();
        void run(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool);
};

GarbageCollector::GarbageCollector() {
    GC_LIMIT = 512 * sizeof(ActivationRecord);
}

bool GarbageCollector::ready() {
    return (alloc.getLiveList().size() * sizeof(ActivationRecord)) > GC_LIMIT;
}

void GarbageCollector::run(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool) {
    markRoots(callstk, opstk, sp, constPool);
    sweep();
    GC_LIMIT *= 2;
}

void GarbageCollector::markObject(GCItem*& curr) {
    if (curr == nullptr)
        return;
    if (curr != nullptr && curr->marked == false) {
        curr->marked = true;
        if (curr->type == LIST && curr->list != nullptr) {
            for (auto & it : *curr->list) {
                markItem(&it);
            }
        } else if (curr->type == CLASS && curr->object != nullptr) {
            for (auto & it : curr->object->fields) {
                markItem(&it.second);
            }
        } else if (curr->type == CLOSURE && curr->closure != nullptr) {
            markAR(curr->closure->env);
        } else if (curr->type == FUNCTION && curr->func != nullptr) {
            cout<<"Marked function "<<curr->func->name<<endl;
        }
    }
}
void GarbageCollector::markItem(StackItem* si) {
    if (si->type == OBJECT) {
        markObject(si->objval);
    }
}
void GarbageCollector::markAR(ActivationRecord* callframe) {
    ActivationRecord* ar = callframe;
    if (ar != nullptr && !ar->marked) {
        ar->marked = true;
        for (int i = 0; i < 255; i++) {
            markItem(&ar->locals[i]);
        }
        markAR(ar->access);
        markAR(ar->control);
    }
}
void GarbageCollector::sweep() {
    unordered_set<GCObject*> nextGen;
    int far = 0, fri = 0, ltn = 0;
    for (auto & it : alloc.getLiveList()) {
        if (it->marked) {
            it->marked = false;
            nextGen.insert(it);
        } else {
            if (it->isAR) {
                freeAR((ActivationRecord*)it);
            } else {
                alloc.free((GCItem*)it);
            }
        }
    }
    alloc.getLiveList().swap(nextGen);
}
void GarbageCollector::markOpStack(StackItem ops[], int sp) {
    for (int i = sp; i >= 0; i--) {
        markItem(&ops[i]);
    }
    int i = 1;
}
void GarbageCollector::markConstPool(ConstPool* constPool) {
    for (int i = 0; i < constPool->maxN; i++) {
        if (constPool->data[i].type == OBJECT && constPool->data[i].objval->marked == false) { 
            constPool->data[i].objval->marked = true;
            if (constPool->data[i].objval->type == CLOSURE && constPool->data[i].objval->closure->env != nullptr) {
                markAR(constPool->data[i].objval->closure->env);
            }
        }
    }
}
void GarbageCollector::markRoots(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool) { 
    markOpStack(opstk, sp);
    markAR(callstk);
    markConstPool(constPool);
}


#endif