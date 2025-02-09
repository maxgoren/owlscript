#ifndef context_hpp
#define context_hpp
#include <iostream>
#include <unordered_map>
#include <list>
#include "stack.hpp"
#include "object.hpp"
using namespace std;


typedef unordered_map<string, Object> Environment;

typedef IndexedStack<Environment> CallStack;

struct Context {
    bool hasMain;
    Environment globals;
    CallStack scoped;
    Context(bool isMain = false) {
        hasMain = isMain;
    }
    Environment& getAt(int depth) {
        //cout<<"getAt("<<scope<<")"<<endl;
        return scoped.get(scoped.size() - 1 - depth);
    }
    void putAt(string id, Object m, int depth) {
        //cout<<"putAt("<<scope<<") - "<<id<<" "<<m<<endl;
        scoped.get(scoped.size() - 1 - depth)[id] = m;
    }
    void openScope() {
        scoped.push(Environment());
    }
    void openScope(Environment& env) {
        scoped.push(env);
    }
    void closeScope() {
        scoped.pop();
    }
};

#endif