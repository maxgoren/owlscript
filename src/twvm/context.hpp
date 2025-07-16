#ifndef context_hpp
#define context_hpp
#include <iostream>
#include "../allocator.hpp"
#include "../activationrecord.hpp"
#include "../stack.hpp"
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
        ActivationRecord* staticAt(int distance);
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
        bool exists(string name, int depth);
        Object& getReference(string name, int depth);
        void put(string name, int depth, Object info);
        void insert(string name, Object info);
        void remove(string name);
        bool existsInScope(string name);
        Allocator& getAlloc();
        Object& nil();
};



#endif