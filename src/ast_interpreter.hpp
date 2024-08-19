#ifndef ast_interpreter_hpp
#define ast_interpreter_hpp
#include <iostream>
#include <unordered_map>
#include <stack>
#include "lex.hpp"
#include "ast.hpp"
#include "object.hpp"
using namespace std;

struct Context {
    unordered_map<string, Object> globals;
    stack<unordered_map<string, Object>> scoped;
};


class ASTInterpreter {
    public:
        ASTInterpreter(bool trace = false);
        Object execAST(astnode* node);
    private:
        Context cxt;
        bool traceEval;
        int recDepth;
        Object getConstValue(astnode* node);
        Object getObjectByID(string id);
        void addToContext(string id, Object m);

        Object exec(astnode* node);
        Object execStatement(astnode* node);
        Object execExpression(astnode* node);

        Object performWhileStatement(astnode* node);
        Object performForStatement(astnode* node);
        Object performDefStatement(astnode* node);
        Object performIfStatement(astnode* node);
        Object performPrintStatement(astnode* node);
        Object performStructStatement(astnode* node);

        Object evalRelop(astnode* node, Object& lhn, Object& rhn);
        Object evalBinOp(astnode* node, Object& lhn, Object& rhn);
        Object evalUnaryOp(astnode* node);
        Object evalStringOp(Symbol op, Object& lhn, Object& rhn);
        Object eval(astnode* node);
        Object performAssignment(astnode* node);
        Object performCreateLambda(astnode* node);
        Object performFunctionCall(astnode* node);
        Object performBlessExpression(astnode* node);
        Object executeFunction(LambdaObj* lambdaObj, astnode* args);

//List operations
        void resolveListForExpression(astnode* node, string& id, Object& m);
        Object execListExpression(astnode* node);
        Object execSubscriptExpression(astnode* node);
        Object execCreateUnNamedList(astnode* node);
        Object execAppendList(astnode* node);
        Object execPushList(astnode* node);
        Object execPopList(astnode* node);
        Object execLength(astnode* node);
        Object execIsEmptyList(astnode* node);
        Object execSortList(astnode* node);
        Object execFirst(astnode* node);
        Object execRest(astnode* node);
        Symbol getSymbol(Object m);
        Object execMap(astnode* node);
        void enter(string s);
        void say(string s);
        void leave();
};

#endif