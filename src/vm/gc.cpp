#include "gc.hpp"

GarbageCollector::GarbageCollector() {
    GC_LIMIT = 128 * sizeof(ActivationRecord);
}

bool GarbageCollector::ready() {
    return (alloc.getLiveList().size() * sizeof(ActivationRecord)) > GC_LIMIT;
}

void GarbageCollector::run(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool) {
    markRoots(callstk, opstk, sp, constPool);
    sweep();
    GC_LIMIT *= 2;
}

void GarbageCollector::markObject(GCObject* cur) {
    if (cur == nullptr || cur == NULL)
        return;
    cur->marked = true;
    if (cur->kind == ITEM) {
        GCItem* curr = (GCItem*)cur;
        if (curr != nullptr && curr->marked == false) {
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
            }
        }
    } else if (cur->kind == AR) {
        markAR((ActivationRecord*) cur);
    }
}
void GarbageCollector::markItem(StackItem* si) {
    if (si->type == OBJECT && si->objval != nullptr) {
        markObject(si->objval);
    }
}
void GarbageCollector::markAR(ActivationRecord* ar) {
    if (ar != nullptr && !ar->marked) {
        ar->marked = true;
        for (int i = 0; i < ar->num_locals; i++) {
            markItem(&(ar->locals[i]));
        }
        markAR(ar->access);
        if (ar->access != ar->control)
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
            switch (it->kind) {
                case AR:   //freeAR((ActivationRecord*)it); break;
                case ITEM: //alloc.free((GCItem*)it); break;
                default: break;
            }
        }
    }
    cout<<alloc.getLiveList().size()<<" -> "<<nextGen.size()<<endl;
    alloc.getLiveList().swap(nextGen);
}
void GarbageCollector::markOpStack(StackItem ops[], int sp) {
    for (int i = sp; i > 0; i--) {
        markItem(&ops[i]);
    }
}
void GarbageCollector::markConstPool(ConstPool* constPool) {
    for (int i = 0; i < constPool->maxN; i++) {
       markItem(&(constPool->data[i]));
    }
}
void GarbageCollector::markRoots(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool) { 
    markOpStack(opstk, sp);
    markAR(callstk);
    markConstPool(constPool);
}