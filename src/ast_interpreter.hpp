#ifndef ast_interpreter_hpp
#define ast_interpreter_hpp
#include <iostream>
#include <unordered_map>
#include <stack>
#include "lex.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "context.hpp"
#include "gc.hpp"
using namespace std;

void saveFile(Object& m);

class ASTInterpreter {
    public:
        ASTInterpreter(bool trace = false);
        Object execAST(astnode* node);
        Context& getContext();
    private:
        GC gc;
        Context cxt;
        bool traceEval;
        int recDepth;
        bool bailout;
        void addToContext(string id, Object m, int scope);
        Object getConstValue(astnode* node);
        Object getObjectByID(string id, int scope);
        pair<string,int> getNameAndScopeFromNode(astnode* node);
        void resolveObjForExpression(astnode* node, string& id, Object& m);
        
        ListNode* merge(ListNode* a, ListNode* b, LambdaObj* compfunc);
        ListNode* mergesort(ListNode* list, LambdaObj* compfunc);
        
        Object exec(astnode* node);
        Object eval(astnode* node);
        Object evalRelop(astnode* node, Object& lhn, Object& rhn);
        Object evalBinOp(astnode* node, Object& lhn, Object& rhn);
        Object evalUnaryOp(astnode* node);
        Object evalStringOp(Symbol op, Object& lhn, Object& rhn);
        Object execStatement(astnode* node);
        Object execExpression(astnode* node);
        Object evalMetaExpression(astnode* node);
        Object execRegularExpr(astnode* node);
        Object executeFunction(LambdaObj* lambdaObj, astnode* args);
        Object performAssignment(astnode* node);
        Object performIfStatement(astnode* node);
        Object performForStatement(astnode* node);
        Object performDefStatement(astnode* node);
        Object performLetStatement(astnode* node);
        Object performCreateLambda(astnode* node);
        Object performFunctionCall(astnode* node);
        Object getObjectByReference(astnode* node);
        Object performMakeReference(astnode* node);
        Object performBlockStatement(astnode* node);
        Object performWhileStatement(astnode* node);
        Object performPrintStatement(astnode* node);

        Object performBlessExpression(astnode* node);
        Object performStructDefStatement(astnode* node);
        Object performSubscriptAssignment(astnode* node, astnode* expr, string& id, int& scope);
        Object performListAssignment(astnode* node, astnode* expr, Object& m);
        Object performStructFieldAssignment(astnode* node, astnode* expr, Object& m);
        Object performSubscriptStringAccess(astnode* node, Object m);
        Object performSubscriptListAccess(astnode* node, Object m);
        Object performSubscriptStructAccess(astnode* node, string id, Object m);
        Object performFileOpenExpression(astnode* node);
        Object execSubscriptExpression(astnode* node);
        Object performRangeExpression(astnode* node);
        Object performListComprehension(astnode* node);
        //List operations
        Object execCreateUnNamedList(astnode* node);
        Object execListExpression(astnode* node);
        Object execIsEmptyList(astnode* node);
        Object execAppendList(astnode* node);
        Object execPopBackList(astnode* node);
        Object execSortList(astnode* node);
        Object execPushList(astnode* node);
        Object execPopList(astnode* node);
        Object execLength(astnode* node);
        Object execFilter(astnode* node);
        Object execFirst(astnode* node);
        Symbol getSymbol(Object m);
        Object execRest(astnode* node);
        Object execMap(astnode* node);
        void enter(string s);
        void say(string s);
        void leave();
};

#endif