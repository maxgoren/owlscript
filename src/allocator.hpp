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

class LiveObjectSets {
    private:
        unordered_map<string, GCObject*> stringSet;
        unordered_set<GCObject*> listSet;
        unordered_set<GCObject*> structSet;
        unordered_set<GCObject*> funcSet;
    public:
        LiveObjectSets() {

        }   
        void insert(GCObject* obj) {
            switch (obj->type) {
                case GC_FUNC:   funcSet.insert(obj); break;
                case GC_LIST:   listSet.insert(obj); break;
                case GC_STRING: stringSet.emplace(*(obj->strval), obj); break;
                case GC_STRUCT: structSet.insert(obj); break;
                default:
                    break;
            }
        }
        int size() {
            return stringSet.size() + listSet.size() + structSet.size() + funcSet.size();
        }
        bool stringExists(string str) {
            return stringSet.find(str) != stringSet.end();
        }
        GCObject* getString(string str) {
            return stringSet[str];
        }
        unordered_set<GCObject*> sweepListSet() {
            unordered_set<GCObject*> kill;
            unordered_set<GCObject*> next;
            for (auto & m : listSet) {
                if (m->marked == false) {
                    auto x = m;
                    kill.insert(x);
                } else {
                    m->marked = false;
                    next.insert(m);
                }
            }
            listSet = next;
            return kill;
        }     
        unordered_set<GCObject*> sweepStructSet() {
            unordered_set<GCObject*> kill;
            unordered_set<GCObject*> next;
            for (auto & m : structSet) {
                if (m->marked == false) {
                    auto x = m;
                    kill.insert(x);
                } else {
                    m->marked = false;
                    next.insert(m);
                }
            }
            structSet = next;
            return kill;
        }
        unordered_set<GCObject*> sweepFuncSet() {
            unordered_set<GCObject*> kill;
            unordered_set<GCObject*> next;
            for (auto & m : funcSet) {
                if (m->marked == false) {
                    auto x = m;
                    kill.insert(x);
                } else {
                    m->marked = false;
                    next.insert(m);
                }
            }
            funcSet = next;
            return kill;
        }
        unordered_set<GCObject*> sweepStringSet() {
            unordered_set<GCObject*> kill;
            unordered_map<string, GCObject*> next;
            for (auto & m : stringSet) {
                if (m.second->marked == false) {
                    auto x = m.second;
                    kill.insert(x);
                } else {
                    m.second->marked = false;
                    next.emplace(m.first, m.second);
                }
            }
            stringSet = next;
            return kill;
        } 
};

class Allocator {
    private:
        int NEXT_GC_LIMIT;
        LiveObjectSets liveObjectSets;
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
