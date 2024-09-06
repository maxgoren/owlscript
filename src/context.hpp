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
};

#endif