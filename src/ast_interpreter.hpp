#ifndef ast_interpreter_hpp
#define ast_interpreter_hpp
#include <iostream>
#include <unordered_map>
#include <stack>
#include "lex.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "context.hpp"
using namespace std;


class ASTInterpreter {
    public:
        ASTInterpreter(bool trace = false);
        Object execAST(astnode* node);
        Context& getContext();
    private:
        Context cxt;
        bool traceEval;
        int recDepth;
        
        void addToContext(string id, Object m, int scope);
        Object getConstValue(astnode* node);
        Object getObjectByID(string id, int scope);
        pair<string,int> getNameAndScopeFromNode(astnode* node);
        void resolveObjForExpression(astnode* node, string& id, Object& m);
        
        
        Object exec(astnode* node);
        Object eval(astnode* node);
        Object evalRelop(astnode* node, Object& lhn, Object& rhn);
        Object evalBinOp(astnode* node, Object& lhn, Object& rhn);
        Object evalUnaryOp(astnode* node);
        Object evalStringOp(Symbol op, Object& lhn, Object& rhn);
        Object execStatement(astnode* node);
        Object execExpression(astnode* node);
        Object executeFunction(LambdaObj* lambdaObj, astnode* args);
        Object performAssignment(astnode* node);
        Object performIfStatement(astnode* node);
        Object performForStatement(astnode* node);
        Object performDefStatement(astnode* node);
        Object performLetStatement(astnode* node);
        Object performCreateLambda(astnode* node);
        Object performFunctionCall(astnode* node);
        Object performBlockStatement(astnode* node);
        Object performWhileStatement(astnode* node);
        Object performPrintStatement(astnode* node);
        Object performListAssignment(astnode* node, Object& m);
        Object performBlessExpression(astnode* node);
        Object performStructDefStatement(astnode* node);
        Object performSubscriptAssignment(astnode* node, string id, int scope);
        Object performStructFieldAssignment(astnode* node, Object& m);
        //List operations
        Object execSubscriptExpression(astnode* node);
        Object execCreateUnNamedList(astnode* node);
        Object execListExpression(astnode* node);
        Object execIsEmptyList(astnode* node);
        Object execAppendList(astnode* node);
        Object execSortList(astnode* node);
        Object execPushList(astnode* node);
        Object execPopList(astnode* node);
        Object execLength(astnode* node);
        Object execFirst(astnode* node);
        Symbol getSymbol(Object m);
        Object execRest(astnode* node);
        Object execMap(astnode* node);
        void enter(string s);
        void say(string s);
        void leave();
};

#endif