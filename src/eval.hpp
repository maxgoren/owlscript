#ifndef eval_hpp
#define eval_hpp
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

void enter(string s);
void say(string s);
void leave();

Object execAST(astnode* node, bool loud);
Object execute(astnode* node);
Object execStatement(astnode* node);
Object execExpression(astnode* node);
Object performWhileStatement(astnode* node);
Object performForStatement(astnode* node);
Object performDefStatement(astnode* node);
Object performIfStatement(astnode* node);
Object performPrintStatement(astnode* node);

Object evalRelop(astnode* node, Object& lhn, Object& rhn);
Object evalBinOp(astnode* node, Object& lhn, Object& rhn);
Object evalUnaryOp(astnode* node);
Object evalStringOp(Symbol op, Object& lhn, Object& rhn);
Object eval(astnode* node);
Object getConstValue(astnode* node);
Object getObjectByID(string id);
void addToContext(string id, Object m);
Object execAssignment(astnode* node);
Object execFunction(astnode* node);
Object execLambda(astnode* node);
Object executeLambda(LambdaObj* lambdaObj, astnode* args);

//List operations
void resolveListForExpression(astnode* node, string& id, Object& m);
Object execCreateUnNamedList(astnode* node);
Object execAppend(astnode* node);
Object execPush(astnode* node);
Object execPop(astnode* node);
Object execLength(astnode* node);
Object execEmpty(astnode* node);
Object execSubscriptExpression(astnode* node);
Object execSort(astnode* node);
Object execFirst(astnode* node);
Object execRest(astnode* node);
Object execMap(astnode* node);
Object execListExpression(astnode* node);


#endif