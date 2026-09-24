#ifndef alloc_hpp
#define alloc_hpp
#include <iostream>
#include <unordered_set>
#include <deque>
#include "heapitem.hpp"
#include "stackitem.hpp"
using namespace std;


class GCAllocator {
    private:
        friend class GarbageCollector;
        unordered_set<GCObject*> live_items;
        deque<GCItem*> free_list;
        GCItem* next() {
            GCItem* x = nullptr;
            if (free_list.empty()) {
                x = new GCItem();
            } else {
                x = free_list.front();
                free_list.pop_front();
            }
            x->marked = false;
            x->kind = ITEM;
            return x;
        }
    public:
        GCAllocator() {

        }
        void free(GCItem* item) {
            if (item == nullptr)
                return;
            switch (item->type) {
                case STRING: {
                    if (item->strval)
                        delete item->strval;
                } break;
                case LIST: {
                    if (item->list)
                        delete item->list;
                } break;
                case CLOSURE: {
                    freeClosure(item->closure);
                } break;
                case CLASS: {
                    freeClass(item->object);
                } break;
            };
            item->type = NILPTR;
            free_list.push_back(item);
        }
        GCItem* alloc(string* s) {
            GCItem* x = next();
            x->type = STRING;
            x->strval = s;
            registerObject(x);
            return x;
        }
        GCItem* alloc(Closure* c) {
            GCItem* x = next();
            x->type = CLOSURE;
            x->closure = c;
            registerObject(x);
            return x;
        }
        GCItem* alloc(deque<StackItem>* l) {
            GCItem* x = next();
            x->type = LIST;
            x->list = l;
            registerObject(x);
            return x;
        }
        GCItem* alloc(ClassObject* l) {
            GCItem* x = next();
            x->type = CLASS;
            x->object = l;
            registerObject(x);
            return x;
        }
        void registerObject(GCObject* obj) {
            live_items.insert(obj);
        }
        unordered_set<GCObject*>& getLiveList() {
            return live_items;
        }
};
extern GCAllocator alloc;
#endif