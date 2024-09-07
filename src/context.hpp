#ifndef context_hpp
#define context_hpp
#include <iostream>
#include <unordered_map>
#include <list>
#include "stack.hpp"
#include "object.hpp"
using namespace std;


typedef unordered_map<string, Object> Environment;

struct ActivationRecord {
    Environment env;
    ActivationRecord* staticLink;
    ActivationRecord* dynamicLink;
};

typedef InspectableStack<ActivationRecord> CallStack;

struct Context {
    Environment globals;
    CallStack scoped;
    Environment& getAt(int depth) {
        //cout<<"getAt("<<scope<<")"<<endl;
        ActivationRecord* ar = &scoped.get(depth);
        return scoped.get(scoped.size() - 1 - depth).env;
    }
    void putAt(string id, Object m, int depth) {
        //cout<<"putAt("<<scope<<") - "<<id<<" "<<m<<endl;
        ActivationRecord* ar = &scoped.get(scoped.size() - 1 - depth);
        ar->env[id] = m;
    }
};

#endif