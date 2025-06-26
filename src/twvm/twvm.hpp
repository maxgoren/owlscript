#ifndef twvm_hpp
#define twvm_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../allocator.hpp"
#include "../ast.hpp"
#include "context.hpp"
#include "../object.hpp"
#include "../regex/patternmatcher.hpp"
using namespace std;

const int MAJOR_VERSION = 0;
const int MINOR_VERSION = 61;

class TWVM {
    private:
        int depth;
        void enter(string s);
        void leave(string s);
        void say(string s);
        bool breakloop;
        bool bailout;
        bool loud;
        Context cxt;
        void push(Object info);
        Object pop();
        Object& peek(int spaces);
        /* statement handlers */
        void defineFunction(astnode* node);
        void defineStruct(astnode* node);
        void letStatement(astnode* node);
        void ifStatement(astnode* node);
        void foreachStatement(astnode* node);
        void whileStatement(astnode* node);
        void breakStatement(astnode* node);
        void printStatement(astnode* node);
        void blockStatement(astnode* node);
        void returnStatement(astnode* node) ;
        void expressionStatement(astnode* node);
        /* Expression Handlers */
        bool checkFunctionArity(astnode* params, astnode* args);
        Object resolveFunction(astnode* node);
        void getType(astnode* node);
        void constExpr(astnode* node);
        void rangeExpression(astnode* node);
        void idExpr(astnode* node);
        void unaryOperation(astnode* node);
        void binaryOperation(astnode* node);
        void assignExpr(astnode* node);
        void subscriptAssignment(astnode* node);
        void subscriptExpression(astnode* node);
        void functionCall(astnode* node);
        void lambdaExpression(astnode* node);
        bool evalFunctionArguments(astnode* args, astnode* params, ActivationRecord*& env);
        void funcExpression(Function* func, astnode* params);
        void doAppendList(astnode* node);
        void doPushList(astnode* node);
        void getListSize(astnode* node);
        void getListEmpty(astnode* node);
        void getFirstListElement(astnode* node);
        void getRestOfList(astnode* node);
        Symbol getSymbol(Object m);
        void doMap(astnode* node);
        void doFilter(astnode* node);
        void doReduce(astnode* node);
        void doCompare(ListNode* front, ListNode* back, Function* cmp);
        ListNode* mergesort(ListNode* head, Function* cmp);
        ListNode* merge(ListNode* front, ListNode* back, Function* cmp);
        void doSort(astnode* node);
        void doReverse(astnode* node);
        void makeAnonymousList(astnode* node);
        void listComprehension(astnode* node);
        void regularExpression(astnode* node);
        void blessExpression(astnode* node);
        void booleanOperation(astnode* node);
        void ternaryConditional(astnode* node);
        void referenceExpression(astnode* node);
        void fileExpression(astnode* node);
        void listExpression(astnode* node);
        void evalExpr(astnode* node);
        void evalStmt(astnode* node);
    public:
        TWVM(bool debug = false);
        void exec(astnode* node);
        Context& context();
};


#endif