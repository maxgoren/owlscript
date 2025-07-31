#include "allocator.hpp"

Allocator::Allocator() {
    NEXT_GC_LIMIT = 150;
}

int Allocator::nextGC() {
    return NEXT_GC_LIMIT;
}
int Allocator::liveCount() {
    return liveObjects.size();
}

bool Allocator::isCollectable(Object& m) {
    switch (m.type) {
        case AS_FUNC:
        case AS_LIST:
        case AS_STRING:
        case AS_STRUCT:
            return m.data.gcobj != nullptr;
        default:
            break;
    }
    return false;
}

void Allocator::registerObject(GCObject* object) {
    object->marked = false;
    liveObjects.insert(object);
}

Object Allocator::makeString(string val) {
    Object m;
    m.type = AS_STRING;
    m.data.gcobj = new GCObject(new string(val));   
    m.data.gcobj->marked = false;
    registerObject(m.data.gcobj);
    return m;
}

Object Allocator::makeFunction(Function* func) {
    Object m;
    m.type = AS_FUNC;
    m.data.gcobj = new GCObject(func);
    registerObject(m.data.gcobj);
    return m;
}

Object Allocator::makeStruct(Struct* st) {
    Object m; 
    m.type = AS_STRUCT;
    m.data.gcobj = new GCObject(st);
    registerObject(m.data.gcobj);
    return m;
}

Object Allocator::makeList(List* list) {
    Object m;
    m.type = AS_LIST;
    m.data.gcobj = new GCObject(list);
    registerObject(m.data.gcobj);
    return m;
}

void Allocator::rungc(ActivationRecord* callStack, IndexedStack<Object>& rtStack) {
    mark(callStack, rtStack);
    sweep();
    NEXT_GC_LIMIT += (NEXT_GC_LIMIT/2);
}

void Allocator::markObject(Object& obj) {
    IndexedStack<Object*> sf;
    sf.push(&obj);
    while (!sf.empty()) {
        Object object = *sf.pop();
        object.data.gcobj->marked = true;
        if (object.type == AS_LIST) {
            for (ListNode* ln = getList(object)->head; ln != nullptr; ln = ln->next) {
                if (isCollectable(ln->info) && ln->info.data.gcobj->marked == false)
                    sf.push(&ln->info);
            }
        } else if (object.type == AS_STRUCT && getStruct(object)->blessed) {
            for (auto & m : getStruct(object)->fields) {
                if (isCollectable(m.second) && m.second.data.gcobj->marked == false)
                    sf.push(&m.second);
            }
        }
    }
}

void Allocator::destroyList(List* list) {
    if (list == nullptr) return;
    while (list->head != nullptr) {
        ListNode* x = list->head;
        list->head = list->head->next;
        if (isCollectable(x->info))
            destroyObject(x->info.data.gcobj);
        delete x;
    }
    delete list;
}

void Allocator::destroyStruct(Struct* sobj) {
    if (sobj == nullptr) return;
    for (auto & m : sobj->fields) {
        if (isCollectable(m.second))
            destroyObject(m.second.data.gcobj);
    }
    delete sobj;
}

void Allocator::destroyFunc(Function* func) {
    if (func == nullptr) return;
    delete func;
}

void Allocator::destroyObject(GCObject* x) {
    switch (x->type) {
        case GC_STRING: { delete x->strval;  delete x; } break;
        case GC_LIST:   { destroyList(x->listval); delete x; } break;
        case GC_STRUCT: { destroyStruct(x->structval); delete x; } break;
        //case GC_FUNC:   { destroyFunc(x->funcval); delete x; } break;
    }
}

void Allocator::mark(ActivationRecord* callStack, IndexedStack<Object>& rtStack) {
    for (int i = 0; i < rtStack.size(); i++) {
        if (isCollectable(rtStack.get(i)))
            markObject(rtStack.get(i));
    }
    for (ActivationRecord* z = callStack; z != nullptr; z = z->controlLink) {
        for (auto & m : z->bindings) {
            if (isCollectable(m.second) && m.second.data.gcobj->marked == false) {
                markObject(m.second);
            }
            ActivationRecord* x = z->accessLink;
            while (x != nullptr) {
                for (auto m : x->bindings) {
                    if (isCollectable(m.second) && m.second.data.gcobj->marked == false)
                        markObject(m.second);
                }
                x = x->accessLink;
            }
        }
    }
}

void Allocator::sweep() {
    int collected = 0;
    unordered_set<GCObject*> kill;
    unordered_set<GCObject*> next;
    for (auto & m : liveObjects) {
        if (m->marked == false) {
            auto x = m;
            kill.insert(x);
        } else {
            m->marked = false;
            next.insert(m);
        }
    }
    for (auto & m : kill) {
        destroyObject(m);
        collected++;
    }
    liveObjects = next;
    //cout<<collected<<" items collected, "<<liveObjects.size()<<" objects survive."<<endl;
}