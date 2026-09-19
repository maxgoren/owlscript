#ifndef heapitem_hpp
#define heapitem_hpp
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <deque>
#include "gcobject.hpp"
using namespace std;

enum HeapItemType {
    STRING, FUNCTION, CLOSURE, LIST, DICT, CLASS, REF, NILPTR
};

struct StackItem;
struct ClassObject;
struct Closure;
struct Function;

string closureToString(Closure* cl);
string listToString(deque<StackItem>* list);
string classToString(ClassObject* obj);
string dictToString(unordered_map<string,StackItem>* dict);

void freeClosure(Closure* cl);
void freeClass(ClassObject* obj);
void freeFunction(Function* f);

struct GCItem : GCObject {
    HeapItemType type;
    union {
        string* strval;
        Function* func;
        Closure* closure;
        deque<StackItem>* list;
        ClassObject* object;
        StackItem* reference;
        unordered_map<string,StackItem>* dict;
    };
    GCItem(unordered_map<string,StackItem>* d) : GCObject(ITEM), type(DICT), dict(d) { }
    GCItem(string* s) : GCObject(ITEM), type(STRING), strval(s) { }
    GCItem(Function* f) : GCObject(ITEM), type(FUNCTION), func(f) { }
    GCItem(Closure* c) : GCObject(ITEM), type(CLOSURE), closure(c) { }
    GCItem(deque<StackItem>* l) : GCObject(ITEM), type(LIST), list(l) { }
    GCItem(ClassObject* o) : GCObject(ITEM), type(CLASS), object(o) { } 
    GCItem(StackItem* r) : GCObject(ITEM), type(REF), reference(r) { }
    GCItem() : GCObject(ITEM), type(NILPTR) { }
    GCItem(const GCItem& si) {
        switch (si.type) {
            case STRING: strval = si.strval; break;
            case FUNCTION: func = si.func; break;
            case CLOSURE: closure = si.closure; break;
            case LIST: list = si.list; break;
            case CLASS: object = si.object; break;
            case REF: reference = si.reference; break;
            case DICT: dict = si.dict; break;
        }
        type = si.type;
        kind = si.kind;
        marked = si.marked;
    }
    GCItem& operator=(const GCItem& si) {
        if (this != &si) {
            switch (si.type) {
                case STRING: strval = si.strval; break;
                case FUNCTION: func = si.func; break;
                case CLOSURE: closure = si.closure; break;
                case LIST: list = si.list; break;
                case CLASS: object = si.object; break;
                case REF: reference = si.reference; break;
                case DICT: dict = si.dict; break;
            }
            type = si.type;
            kind = si.kind;
            marked = si.marked;
        }
        return *this;
    }
    string toString() {
        switch (type) {
            case STRING: return *(strval);
            case FUNCTION: return "(func)";
            case CLOSURE: return closureToString(closure);
            case LIST: return listToString(list);
   //         case DICT: return dictToString(dict);
            case CLASS: return "(class)" + classToString(object);
            case REF: return "(reference)";
        }
        return "(nil)";
    }
    bool equals(GCItem* rhs) {
        if (type != rhs->type)
            return false;
        switch (type) {
            case STRING: return *strval == *rhs->strval;
            case FUNCTION: return func == rhs->func;
            case LIST: return listToString(list) == listToString(rhs->list);
            case CLASS: return object == rhs->object;
            case CLOSURE: return closure == rhs->closure;
            case REF:   return false;
        }
        return false;
    }
};

#endif