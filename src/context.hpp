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
    Environment& getAt(int scope) {
        //cout<<"getAt("<<scope<<")"<<endl;
        ActivationRecord* ar = &scoped.get(scope);
        return scoped.get(scoped.size() - 1 - scope).env;
    }
    void putAt(string id, Object m, int scope) {
        //cout<<"putAt("<<scope<<") - "<<id<<" "<<m<<endl;
        scoped.get(scope).env.emplace(id, m);
        ActivationRecord* ar = &scoped.get(scoped.size() - 1 - scope);
        ar->env[id] = m;
    }
};

#endif