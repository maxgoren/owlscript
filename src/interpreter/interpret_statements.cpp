#include "ast_interpreter.hpp"

Object ASTInterpreter::performWhileStatement(astnode* node) {
    enter("while statement");
    Object m;
    while (evalExpression(node->child[0]).boolval) {
        exec(node->child[1]);
    }
    leave();
    return m;
}
Object ASTInterpreter::performForEachStatement(astnode* node) {
    Object lstObject = evalExpression(node->child[0]->next);
    if (typeOf(lstObject) != AS_LIST) {
        cout<<"Foreach only for list types."<<endl;
        return makeNilObject();
    }
    LambdaObj* lmbd = makeLambdaObj(node->child[1], node->child[0]);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(lstObject)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        evalFunctionExpr(lmbd, t);
    }
    return lstObject;
}
Object ASTInterpreter::performForStatement(astnode* node) {   
    Object m;
    astnode* precon = node->child[0];
    astnode* testcon = node->child[0]->next;
    astnode* postExpr = node->child[0]->next->next;
    astnode* loopBody = node->child[1];
    if (precon->type == STMT_NODE) {
        m = execStatement(precon);
    } else {
        m = evalExpression(precon);
    }
    while (evalExpression(testcon).boolval) {
        exec(loopBody);
        m = evalExpression(postExpr);
    }
    leave();
    return m;
}

Object ASTInterpreter::performDefStatement(astnode* node) {
    enter("def statement");
    string id = getNameAndScopeFromNode(node).name;
    int scope = getNameAndScopeFromNode(node).scope;
    Object m = performCreateLambda(node);
    gc.add(m.objval);
    updateContext(id, m, scope);
    leave();
    return m;
}

Object ASTInterpreter::performIfStatement(astnode* node) {
    enter("If statement");
    Object m;
    bool test = evalExpression(node->child[0]).boolval;
    if (test) {
        m = exec(node->child[1]);
    } else if (!test && node->child[2] != nullptr) {
        m = exec(node->child[2]);
    }
    leave();
    return m;
}

Object ASTInterpreter::performPrintStatement(astnode* node) {
    Object m = exec(node->child[0]);
    cout<<toString(m);
    if (node->attributes.strval == "println") cout<<endl;
    return m;
}

Object ASTInterpreter::performStructDefStatement(astnode* node) {
    string id = node->attributes.strval;
    StructObj* sobj = makeStructObj();
    for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
        string vname = getAttributes(it->child[0]).strval;
        sobj->bindings[vname] = makeNilObject();
    }
    sobj->blessed = false;
    cxt.globals[id] = makeStructObject(sobj);
    gc.add(cxt.globals[id].objval);
    return makeNilObject();
}
 
Object ASTInterpreter::performBlockStatement(astnode* node) {
    cxt.openScope();
    exec(node->child[0]);
    cxt.closeScope();
    if (cxt.scoped.size() <= 1)
        gc.run(cxt);
    return makeNilObject();
}

Object ASTInterpreter::performLetStatement(astnode* node) {
    Object m;
    astnode* t = node->child[0];
    if (t != nullptr && t->child[0]) {
        if (isExprType(t->child[0], ID_EXPR)) {
            string id = getAttributes(t->child[0]).strval;
            if (!cxt.scoped.empty()) {
                declareInContext(t, cxt.scoped.top(), id);
            } else {
                declareInContext(t, cxt.globals, id);
            }
        }
    }
    return m;
}
