#include "context.hpp"

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

ActivationRecord* Context::staticAt(int distance) {
    ActivationRecord* curr = current;
    while (distance > 0 && curr != nullptr) {
        curr = curr->controlLink;
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

void Context::remove(string name) {
    if (current->bindings.find(name) != current->bindings.end()) {
        current->bindings.erase(current->bindings.find(name));
    }
}

Object& Context::get(string name, int depth) {
    if (depth == GLOBAL_SCOPE_DEPTH) {
        return globals->bindings[name];
    }
    return enclosingAt(depth)->bindings[name];
}

Object& Context::getReference(string name, int depth) {
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
    if (current == globals) {
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
