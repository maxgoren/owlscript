#ifndef allocator_hpp
#define allocator_hpp
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "stack.hpp"
#include "object.hpp"
#include "activationrecord.hpp"
using namespace std;


class Allocator {
    private:
        int NEXT_GC_LIMIT;
        unordered_set<GCObject*> liveObjects;
        bool isCollectable(Object& m);
        void markObject(Object& obj);
        void mark(ActivationRecord* callStack, IndexedStack<Object>& rtStack);
        void sweep();
        void destroyList(List* list);
        void destroyStruct(Struct* obj);
        void destroyObject(GCObject* obj);
        void registerObject(GCObject* obj);
    public:
        Allocator();
        Object makeString(string val);
        Object makeList(List* list);
        Object makeFunction(Function* func);
        Object makeStruct(Struct* st);
        void rungc(ActivationRecord* callStack, IndexedStack<Object>& rtStack);
        int liveCount();
        int nextGC();
};



#endif
