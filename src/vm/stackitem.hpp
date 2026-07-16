#ifndef stackitem_hpp
#define stackitem_hpp
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <deque>
#include "alloc.hpp"
#include "heapitem.hpp"
using namespace std;


enum SIType {
    NIL, INTEGER, NUMBER, BOOLEAN, OBJECT
 };

struct StackItem {
    int type;
    union {
        int intval;
        bool boolval;
        double numval;
        GCItem* objval;
    };
    string toString();
    StackItem(int value);
    StackItem(double value);
    StackItem(bool balue);
    StackItem(string value);
    StackItem(Closure* c);
    StackItem(deque<StackItem>* l);
    StackItem(ClassObject* o);
    StackItem(GCItem* i);
    StackItem();
    StackItem(const StackItem& si);
    StackItem& operator=(const StackItem& si);
    bool lessThan(StackItem& si);
    bool equals(StackItem& rhs);
    StackItem& add(StackItem& rhs);
    StackItem& sub(StackItem& rhs);
    StackItem& mul(StackItem& rhs);
    StackItem& div(StackItem& rhs);
    StackItem& mod(StackItem& rhs);
};

struct BlockScope;

struct ClassObject  {
    string name;
    int cpIdx;
    unordered_map<string, StackItem> fields;
    bool instantiated;
    BlockScope* scope;
    ClassObject(string n = "",  BlockScope* s = nullptr) {
        name = n;
        scope = s;
    }
};

string classToString(ClassObject* obj);
void freeClass(ClassObject* obj);
string listToString(deque<StackItem>* list);


#endif