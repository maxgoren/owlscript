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
    Environment globals;
    CallStack scoped;
    Environment& getAt(int depth) {
        //cout<<"getAt("<<scope<<")"<<endl;
        return scoped.get(scoped.size() - 1 - depth);
    }
    void putAt(string id, Object m, int depth) {
        //cout<<"putAt("<<scope<<") - "<<id<<" "<<m<<endl;
        scoped.get(scoped.size() - 1 - depth)[id] = m;
    }
};

#endif