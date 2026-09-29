#ifndef closure_hpp
#define closure_hpp
#include "callframe.hpp"
using namespace std;

struct BlockScope;

struct Function : GCObject {
    string name;
    int start_ip;
    BlockScope* scope;
    Function(string n = "<none>", int sip = -1) : name(n), start_ip(sip) { }
    Function(const Function& f) {
        name = f.name;
        start_ip  = f.start_ip;
    }
    Function& operator=(const Function& f) {
        if (this != &f) {
            name = f.name;
            start_ip  = f.start_ip;
        }
        return *this;
    }
};

struct Closure {
    Function func;
    ActivationRecord* env;
    Closure(Function f, ActivationRecord* e) : func(f), env(e) { }
    Closure(Function f) : func(f), env(nullptr) { }
    Closure(const Closure& c) {
        func = c.func;
        env = c.env;
    }
    ~Closure() {
       
    }
    Closure& operator=(const Closure& c) {
        if (this != &c) {
            func = c.func;
            env = c.env;
        }
        return *this;
    }
};

string closureToString(Closure* closure);
void freeClosure(Closure* cl);

#endif