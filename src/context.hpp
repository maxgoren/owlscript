#ifndef context_hpp
#define context_hpp
#include <iostream>
#include "allocator.hpp"
#include "activationrecord.hpp"
#include "stack.hpp"
using namespace std;

const int GLOBAL_SCOPE_DEPTH = -1;

class Context {
    private:
        unordered_map<string, Struct*> objects;
        ActivationRecord* globals;
        ActivationRecord* current;
        Object nilObject;
        Allocator alloc;
        IndexedStack<Object> operands;
        ActivationRecord* enclosingAt(int distance);
    public:
        Context();
        ActivationRecord*& getCallStack();
        IndexedStack<Object>& getOperandStack();
        void addStructType(Struct* st);
        Struct* getInstanceType(string name);
        void openScope();
        void openScope(ActivationRecord* scope);
        void closeScope();
        Object& get(string name, int depth);
        void put(string name, int depth, Object info);
        void insert(string name, Object info);
        bool existsInScope(string name);
        Allocator& getAlloc();
        Object& nil();
};

Context::Context() {
    globals = new ActivationRecord(nullptr);
    current = globals;
    nilObject = makeNil();
}

ActivationRecord* Context::enclosingAt(int distance) {
    ActivationRecord* curr = current;
    while (distance > 0 && curr != nullptr) {
        curr = curr->accessLink;
        distance--;
    }
    if (distance > 0 || curr == nullptr) {
        cout<<"Hmm... Something wrong with scope distance."<<endl;
    }
    return curr;
}

void Context::insert(string name, Object info) {
    current->bindings[name] = info;
}

Object& Context::get(string name, int depth) {
    if (depth == GLOBAL_SCOPE_DEPTH) {
        return globals->bindings[name];
    }
    return enclosingAt(depth)->bindings[name];
}

void Context::put(string name, int depth, Object info) {
    if (depth == GLOBAL_SCOPE_DEPTH) {
        globals->bindings[name] = info;
    } else {
        enclosingAt(depth)->bindings[name] = info;
    }
    if (alloc.liveCount() > alloc.nextGC()) {
        alloc.rungc(current, operands);
    }
}

bool Context::existsInScope(string name) {
    return current->bindings.find(name) != current->bindings.end();
}

Allocator& Context::getAlloc() {
    return alloc;
}

Object& Context::nil() {
    return nilObject;
}

void Context::openScope() {
    ActivationRecord* sf = new ActivationRecord(current, current);
    current = sf;
}

void Context::openScope(ActivationRecord* scope) {
    current = scope;
}

void Context::closeScope() {
    if (current != globals) {
        current = current->controlLink;
    }
    if (alloc.liveCount() > alloc.nextGC()) {
        alloc.rungc(current, operands);
    }
}

ActivationRecord*& Context::getCallStack() {
    return current;
}

IndexedStack<Object>& Context::getOperandStack() {
    return operands;
}

void Context::addStructType(Struct* st) {
    objects[st->typeName] = st;
}

Struct* Context::getInstanceType(string name) {
    return objects[name];
}



#endif