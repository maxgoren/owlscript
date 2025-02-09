#ifndef ast_interpreter_hpp
#define ast_interpreter_hpp
#include <iostream>
#include <unordered_map>
#include <stack>
#include "../lex.hpp"
#include "../ast.hpp"
#include "../object.hpp"
#include "../context.hpp"
#include "../gc.hpp"
using namespace std;

void saveFile(Object& m);

struct NameAndScope {
    string name;
    int scope;
    NameAndScope(string n = "nil", int sc = -1) : name(n), scope(sc) { }
};

class ASTInterpreter {
    public:
        ASTInterpreter(bool trace = false);
        Object execAST(astnode* node);
        Context& getContext();
    private:
        GC gc;
        Context cxt;
        IndexedStack<Object> rtStack;
        bool traceEval;
        int recDepth;
        bool bailout;
        void updateContext(string id, Object m, int scope);
        void resolveObjForExpression(astnode* node, string& id, Object& m);
        Object declareInContext(astnode* t, Environment& env, string id);
        Object getConstValue(astnode* node);
        Object getObjectByID(string id, int scope);
        NameAndScope getNameAndScopeFromNode(astnode* node);
        
        ListNode* merge(ListNode* a, ListNode* b, LambdaObj* compfunc);
        ListNode* mergesort(ListNode* list, LambdaObj* compfunc);
        
        Object exec(astnode* node);
        Object execStatement(astnode* node);
        Object evalExpression(astnode* node);
        Object eval(astnode* node);
        Object evalRelop(astnode* node, Object& lhn, Object& rhn);
        Object evalBinOp(astnode* node, Object& lhn, Object& rhn);
        Object evalUnaryOp(astnode* node);
        Object evalStringOp(Symbol op, Object& lhn, Object& rhn);
        Object evalMetaExpression(astnode* node);
        Object evalRegularExpr(astnode* node);
        Object evalFunctionExpr(LambdaObj* lambdaObj, astnode* args);
        Object evalBlessExpression(astnode* node);
        Object evalRangeExpression(astnode* node);
        Object evalListExpression(astnode* node);
        Object evalSubscriptExpression(astnode* node);
        Object evalAssignmentExpression(astnode* node);
        void prepareEnvForFunctionCall(LambdaObj* lobj, astnode* args, Environment& env);
        void cleanUpAfterFunctionCall(LambdaObj* lobj, astnode* args);
        Object performIfStatement(astnode* node);
        Object performForStatement(astnode* node);
        Object performDefStatement(astnode* node);
        Object performLetStatement(astnode* node);
        Object performCreateLambda(astnode* node);
        Object performFunctionCall(astnode* node);
        Object performBlockStatement(astnode* node);
        Object performWhileStatement(astnode* node);
        Object performPrintStatement(astnode* node);
        Object performStructDefStatement(astnode* node);
        Object performForEachStatement(astnode* node);
        Object performSubscriptAssignment(astnode* node, astnode* expr, string& id, int& scope);
        Object performListAssignment(astnode* node, astnode* expr, Object& m);
        Object performListAccess(astnode* node, Object m);
        Object performStructFieldAssignment(astnode* node, astnode* expr, Object& m);
        Object performStructFieldAccess(astnode* node, string id, Object m);
        Object performSubscriptStringAccess(astnode* node, Object m);
        Object performSubscriptStringAssignment(astnode* node, astnode* expr, Object& m);
        Object performFileOpenExpression(astnode* node);
        Object performMetaExpression(astnode* node);
        //List operations
        Object performListComprehension(astnode* node);
        Object performCreateUnNamedList(astnode* node);
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