#ifndef interpreter_hpp
#define interpreter_hpp
#include <iostream>
#include <map>
#include <set>
#include "../object/object.hpp"
#include "../closure/closure.hpp"
#include "../parser/parser.hpp"
#include "../memstore/memstore.hpp"
using namespace std;


struct Procedure {
    string name;
    ASTNode* paramList;
    ASTNode* functionBody;
};

struct ActivationRecord {
    Procedure* function;
    map<string, int> env;
    Object* returnValue;
    ActivationRecord* staticLink;
    ActivationRecord();
};


class CallStack {
    private:
        ActivationRecord* stack[255];
        int p;
    public:
        CallStack() {
            p = 0;
        }
        bool empty() {
            return p == 0;
        }
        int size() {
            return p;
        }
        void push(ActivationRecord* ar) {
            stack[p++] = ar;
        }
        void pop() {
            ActivationRecord* t = top();
            p--;
            delete t;
        }
        ActivationRecord* top() {
            return stack[p-1];
        }
};

class Interpreter {
    private:
        //for tracing
        bool loud;
        int recDepth;
        void enter(string s);
        void leave(string s);
        void leave();
        void say(string s);
        //runtime environment
        set<StoreAs> dontEval;  //types eval() should not try to evaluate.
        set<string> builtIns;   //name of 'built in' procedures
        map<string, int> st;   //global level symbol table
        map<string, Procedure*> procedures;
        CallStack callStack; //for managing procedure calls
        bool stopProcedure; //for bailing out of recursion
        MemStore memStore;  //for storing objects
    private:
        //variable related
        int getAddress(string name);
        Object* getVariableValue(ASTNode* node);
        //expression evaluation
        Object* eval(ASTNode* node);
        Object* expression(ASTNode* node);
        //Procedure and Function related methods
        void defineFunction(ASTNode* node);
        Object* callBuiltIn(ASTNode* node);
        ActivationRecord* prepareActivationRecord(ASTNode* node);
        Object* runClosure(ASTNode* node, Object* obj);
        Object* procedureCall(ASTNode* node);
        Object* lambdaExpr(ASTNode* node);
        void returnStmt(ASTNode* node);
        //control statements
        void ifStmt(ASTNode* node);
        void loopStmt(ASTNode* node);
        void assignStmt(ASTNode* node);
        void statement(ASTNode* node);
        //I/O related
        void printStmt(ASTNode* node);
        void readStmt(ASTNode* node);
        //List related methods
        Object* mapExpr(ASTNode* node);
        Object* listExpr(ASTNode* node);
        Object* carExpr(ASTNode* node);
        Object* cdrExpr(ASTNode* node);
        Object* getListItem(ASTNode* node, Object* list);
        Object* sortList(ASTNode* node);
        Object* listSize(ASTNode* node); 
        void popList(ASTNode* node);
        void pushList(ASTNode* node);
        void appendList(ASTNode* node);
    public:
        Interpreter();
        void run(ASTNode* node);
        void setLoud(bool isloud);
        void resetRecDepth();
};

#endif