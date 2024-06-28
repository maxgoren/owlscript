#ifndef interpreter_hpp
#define interpreter_hpp
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "../object/object.hpp"
#include "../closure/closure.hpp"
#include "../parser/parser.hpp"
#include "../memstore/memstore.hpp"
#include "callstack.hpp"
using namespace std;


class Interpreter {
    private:
        //for tracing
        bool loud;
        int recDepth;
        int step;
        void enter(string s);
        void leave(string s);
        void leave();
        void say(string s);
        //runtime environment
        unordered_set<StoreAs> dontEval;  //types eval() should not try to evaluate.
        unordered_set<string> builtIns;   //name of 'built in' procedures
        unordered_map<string, int> st;   //global level symbol table
        unordered_map<string, Procedure*> procedures;
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
        Object* evalRelOp(TOKENS op, double left, double right);
        bool isRelOp(TOKENS token);
        //Procedure and Function related methods
        void defineFunction(ASTNode* node);
        Object* callBuiltIn(ASTNode* node);
        ActivationRecord prepareActivationRecord(ASTNode* node);
        Object* runClosure(ASTNode* node, Object* obj);
        Object* runProcedure(ASTNode* node);
        Object* procedureCall(ASTNode* node);
        Object* lambdaExpr(ASTNode* node);
        void returnStmt(ASTNode* node);
        //control statements
        void ifStmt(ASTNode* node);
        void loopStmt(ASTNode* node);
        void assignStmt(ASTNode* node);
        void statement(ASTNode* node);
        void letStmt(ASTNode* node);
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
        bool scopeIsGlobal();
    public:
        Interpreter();
        Object* run(ASTNode* node);
        void setLoud(bool isloud);
        void resetRecDepth();
        void memstats();
};

#endif