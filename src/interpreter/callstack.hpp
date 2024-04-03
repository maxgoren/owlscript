#ifndef callstack_hpp
#define callstack_hpp

struct Procedure {
    string name;
    ASTNode* paramList;
    ASTNode* functionBody;
};

struct ActivationRecord {
    Procedure* function;
    unordered_map<string, int> env;
    Object* returnValue;
    ActivationRecord* staticLink;
    ActivationRecord();
};


class CallStack {
    private:
        ActivationRecord** stack;
        int p;
        int max;
        void grow();
    public:
        CallStack();
        ~CallStack();
        bool empty();
        int size();
        void push(ActivationRecord* ar);
        void pop();
        ActivationRecord* top();
};

#endif