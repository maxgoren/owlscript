#include "eval.hpp"

Context cxt;
bool traceEval = false;
int recDepth = 0;

void say(string s) {
    for (int i = 0; i < recDepth; i++)
        cout<<" ";
    cout<<s<<endl;
}

void enter(string s) {
    ++recDepth;
    if (traceEval)
        say(s);
}

void leave() {
    --recDepth;
}

Object execAST(astnode* node, bool loud) {
    recDepth = 0;
    traceEval = loud;
    return execute(node);
}

Object evalRelop(astnode* node, Object& lhn, Object& rhn) {
    enter("eval relop");
    if (comparesAsOrdinal(lhn) && comparesAsOrdinal(rhn)) {
        double lhs = getAsReal(lhn);
        double rhs = getAsReal(rhn);
        leave();
        switch (node->attributes.symbol) {
            case TK_LT: return makeBoolObject(lhs < rhs);
            case TK_GT: return makeBoolObject(lhs > rhs);
            case TK_GTE: return makeBoolObject(lhs >= rhs);
            case TK_LTE: return makeBoolObject(lhs <= rhs);
            case TK_EQU: return makeBoolObject(lhs == rhs);
            case TK_NOTEQU: return makeBoolObject(lhs != rhs);
            default:
                break;
        }
    }
    string lst = toString(lhn);
    string rst = toString(rhn);
    cout<<"Compring "<<lst<<" and "<<rst<<endl;
    return makeBoolObject(lst == rst);
}

Object evalBinOp(astnode* node, Object& lhn, Object& rhn) {
    enter("eval binary op");
    if (comparesAsOrdinal(lhn) && comparesAsOrdinal(rhn)) {
        double lhs = getAsReal(lhn);
        double rhs = getAsReal(rhn);
        //cout<<lhs<<" <op> "<<rhs<<endl;
        leave();
        switch (node->attributes.symbol) {
            case TK_ADD: return makeRealObject(lhs + rhs);
            case TK_SUB: return makeRealObject(lhs - rhs);
            case TK_MUL: return makeRealObject(lhs * rhs);
            case TK_DIV: {
                if (rhs == 0) {
                    cout<<"Error: Divide by zero."<<endl;
                    return makeNilObject();
                }
                return makeRealObject(lhs / rhs);
            }
            default: 
                break;
        }
    } else if (typeOf(lhn) == AS_STR || typeOf(rhn) == AS_STR) {
        return evalStringOp(node->attributes.symbol, lhn, rhn);
    }
    return makeIntObject(0);
}

Object eval(astnode* node) {
    if (node == nullptr)
        return makeNilObject();
    Object lhn = execExpression(node->child[0]);
    Object rhn = execExpression(node->child[1]);
    switch (node->exprType) {
        case RELOP_EXPR:  return evalRelop(node, lhn, rhn);
        case BINARYOP_EXPR: return evalBinOp(node, lhn, rhn);
        default:
            break;
    }
    return makeIntObject(0);
}

Object evalStringOp(Symbol op, Object& lhn, Object& rhn) {
    if (op != TK_ADD) {
        cout<<"Error: unsupported operation on type 'string'"<<endl;
        return makeNilObject();
    }
    string concat = toString(lhn) + toString(rhn);
    return makeStringObject(concat);
}

Object evalUnaryOp(astnode* node) {
    enter("eval unary op");
    Object m = execExpression(node->child[0]);
    switch (node->attributes.symbol) {
        case TK_SUB: {
                say("unary minus");
                if (m.type == AS_REAL) { 
                    m.realval = -(m.realval); 
                } else if (m.type == AS_INT) {
                     m.intval = -(m.intval); 
                }
            }
            leave();
            return m;
        case TK_BANG: {
                say("unary negate");
                if (m.type == AS_REAL)  m.realval = !(m.realval);
                else if (m.type == AS_INT)  m.intval = !(m.intval);
                else if (m.type == AS_BOOL) m.boolval = !(m.boolval);
            }
            leave();
            return m;
        default:
            break;
    }
    leave();
    return m;
}

Object getConstValue(astnode* node) {
    Object m;
    enter("get const value");
    if (node->attributes.symbol == TK_NUM)
        m = makeIntObject(atoi(node->attributes.strval.c_str()));
    else if (node->attributes.symbol == TK_REALNUM)
        m = makeRealObject(stod(node->attributes.strval));
    else if (node->attributes.symbol == TK_TRUE || node->attributes.symbol == TK_FALSE)
        m = makeBoolObject(node->attributes.strval == "true" ? true:false);
    else if (node->attributes.symbol == TK_ID || node->attributes.symbol == TK_STRING)
        m = makeStringObject(node->attributes.strval);
    else
        m = makeNilObject();
    leave();
    return m;
}

Object getObjectByID(string id) {
    Object m;
    enter("Get object by ID: " + id);
    if (!cxt.scoped.empty() && cxt.scoped.top().find(id) != cxt.scoped.top().end()) {
        m = cxt.scoped.top()[id];
    } else if (cxt.globals.find(id) != cxt.globals.end()) {
        m = cxt.globals[id];
    } else {
        cout<<"Unknown Identifier: "<<id<<endl;
        m = makeNilObject();
    }
    leave();
    return m;
}

void addToContext(string id, Object m) {
    if (cxt.scoped.empty()) {
        cxt.globals[id] = m;
    } else {
        cxt.scoped.top()[id] = m;
    }
}

Object execAssignment(astnode* node) {
    Object m;
    string id;
    enter("assignment");
    if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        id = node->child[0]->child[0]->attributes.strval;
        m = getObjectByID(id);
        Object subm = execExpression(node->child[0]->child[1]);
        int sub = atoi(toString(subm).c_str());
        if (sub > getList(m)->length) {
            cout<<"Error: subscript out of range"<<endl;
            leave();
            return makeNilObject();
        }
        ListNode* it = getList(m)->head;
        for (int i = 0; i < sub; i++) {
            it = it->next;
        }
        it->info = execExpression(node->child[1]);
    } else {
        id = node->child[0]->attributes.strval;
        m = execExpression(node->child[1]);
        //cout<<id<<" assigned value "<<toString(m)<<endl;
    }
    addToContext(id, m);
    leave();
    return m;
}

Object execFunction(astnode* node) {
    enter("function call");
    string id = node->attributes.strval;
    Object lmbd = getObjectByID(id);
    if (lmbd.type != AS_LAMBDA) {
        leave();
        return makeNilObject();
    }
    Object m = executeLambda(getLambda(lmbd), node->child[1]);
    addToContext(id, lmbd);
    leave();
    return m;
}

Object executeLambda(LambdaObj* lambdaObj, astnode* args) {
    enter("execute lambda");
    unordered_map<string,Object> localcxt;
    astnode* params = lambdaObj->body;
    VarList* freeVars = lambdaObj->freeVars;
    if (freeVars != nullptr) {
        //Add captured variables to local context
        for (VarList* it = freeVars; it != nullptr; it = it->next) {
            localcxt[it->key] = it->value;
        }
    }
    while (params != nullptr && args != nullptr) {
        string vname = params->attributes.strval;
        string val = args->attributes.strval;
        localcxt[vname] = execExpression(args);
        //cout<<"Added "<<vname<<" to symbol table with value "<<toString(localcxt[vname])<<endl;
        params = params->next;
        args = args->next;
    }
    cxt.scoped.push(localcxt);
    Object m = execute(lambdaObj->params);
    if (freeVars != nullptr) {
        //update any closed-over variables before exiting.
        for (VarList* it = freeVars; it != nullptr; it = it->next) {
            it->value = cxt.scoped.top()[it->key];
        }
        lambdaObj->freeVars = freeVars;
    }
    cxt.scoped.pop();
    leave();
    return m;
}

Object execLambda(astnode* node) {
    enter("create lambda");
    LambdaObj* lm = makeLambdaObj(node->child[1], node->child[0]);
    if (!cxt.scoped.empty()) {
        for (auto frv : cxt.scoped.top()) {
            lm->freeVars = makeVarList(frv.first, frv.second, lm->freeVars);
        }
    }
    Object m = makeLambdaObject(lm);
    leave();
    return m;
}


Object execCreateUnNamedList(astnode* node) {
    enter("unnamed list");
    ListObj* list = makeListObj();
    for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
        Object m = execExpression(it);
        appendToList(list, m);
    }
    leave();
    return makeListObject(list);
}

void resolveListForExpression(astnode* node, string& id, Object& m) {
    if (node->child[0]->attributes.symbol ==  TK_ID) {
        id = node->child[0]->attributes.strval;
        m = getObjectByID(id);
    } else if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        m = execCreateUnNamedList(node->child[0]);
    }  else if (node->child[0]->type == EXPR_NODE) {
        m = execExpression(node->child[0]);
    } else {
        m = makeNilObject();
    }
}

Object execAppend(astnode* node) {
    enter("append to list");
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    Object t = execExpression(node->child[1]);
    appendToList(getList(m), t);
    addToContext(id, m);
    leave();
    return t;
}

Object execPush(astnode* node) {
    enter("push to list");
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    Object t = execExpression(node->child[1]);
    pushToList(getList(m), t);
    addToContext(id, m);
    leave();
    return t;
}

Object execPop(astnode* node) {
    enter("pop list");
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    popList(getList(m));
    addToContext(id, m);
    leave();
    return m;
}

Object execLength(astnode* node) {
    enter("list length");
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    Object t = makeIntObject(listLength(getList(m)));
    leave();
    return t;
}

Object execEmpty(astnode* node) {
    enter("test list for empty");
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    Object t = makeBoolObject(listLength(getList(m)) == 0);
    leave();
    return t;
}

Object execSubscriptExpression(astnode* node) {
    string id;
    Object m;
    resolveListForExpression(node, id, m);
    ListObj* list = getList(m);
    Object subm = execExpression(node->child[1]);
    int subscript = atoi(toString(subm).c_str());
    if (subscript > list->length || list->length == 0) {
        cout<<"Error: Subscript out of range."<<endl;
        return makeNilObject();
    }
    ListNode* it = list->head;
    for (int i = 0; i < subscript; i++) {
        it = it->next;
    }
    return it->info;
}

Object execSort(astnode* node) {
    enter("sort list");
    Object m;
    string id;
    resolveListForExpression(node, id, m);
    ListObj* list = getList(m);
    ListNode* head = mergesort(list->head);
    ListNode* tail = head;
    while (tail->next != nullptr)
        tail = tail->next;
    list->head = head;
    list->tail = tail;
    m.objval->listObj = list;
    if (!id.empty())
        addToContext(id, m);
    leave();
    return m;
}

Object execFirst(astnode* node) {
    enter("car");
    Object m;
    string id;
    resolveListForExpression(node, id, m);
    if (typeOf(m) != AS_LIST) {
        cout<<"Error: 'first' can only be performed on list types."<<endl;
        return makeNilObject();
    }
    Object t = getList(m)->head->info;
    leave();
    return t;
}

Object execRest(astnode* node) {
    enter("cdr");
    Object m;
    string id;
    resolveListForExpression(node, id, m);
    if (typeOf(m) != AS_LIST) {
        cout<<"Error: 'rest' can only be performed on list types."<<endl;
        return makeNilObject();
    }
    ListObj* list = makeListObj();
    for (ListNode* it = getList(m)->head->next; it != nullptr; it = it->next)
        appendToList(list, it->info);
    leave();
    return makeListObject(list);
}

Symbol getSymbol(Object m) {
    switch (m.type) {
        case AS_INT: return TK_NUM;
        case AS_REAL: return TK_REALNUM;
        case AS_BOOL: return m.boolval ? TK_TRUE:TK_FALSE;
        case AS_STR: return TK_ID;
        case AS_LIST: return TK_LSQUARE;
        case AS_LAMBDA: return TK_LAMBDA;
        default:
            break;
    }
    return TK_NONE;
}

Object execMap(astnode* node) {
    enter("Map list");
    Object m;
    string id;
    resolveListForExpression(node, id, m);
    Object lm = execExpression(node->child[1]);
    LambdaObj* lmbd = getLambda(lm);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(m)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        Object r = executeLambda(lmbd, t);
        appendToList(result, r);
    }
    leave();
    return makeListObject(result);
}

Object execListExpression(astnode* node) {
    enter("List expression");
    Object m;
    if (node->exprType == SUBSCRIPT_EXPR) {
        m = execSubscriptExpression(node);
    } else {
        switch (node->attributes.symbol) {
            case TK_LSQUARE: 
                m = execCreateUnNamedList(node);
                break;
            case TK_APPEND:
                m = execAppend(node);
                break;
            case TK_PUSH:
                m = execPush(node);
                break;
            case TK_POP:
                m = execPop(node);
                break;
            case TK_LENGTH:
                m = execLength(node);
                break;
            case TK_SORT:
                m = execSort(node);
                break;
            case TK_FIRST:
                m = execFirst(node);
                break;
            case TK_REST:
                m = execRest(node);
                break;
            case TK_MAP:
                m = execMap(node);
                break;
            case TK_EMPTY:
                m = execEmpty(node);
                break;
            default:
                break;
        }
    }
    leave();
    return m;
}

Object performWhileStatement(astnode* node) {
    enter("while statement");
    Object m;
    while (execExpression(node->child[0]).boolval) {
        execute(node->child[1]);
    }
    leave();
    return m;
}

Object performForStatement(astnode* node) {
    enter("for statement");
    Object m;
    astnode* precon = node->child[0];
    astnode* testcon = node->child[0]->next;
    astnode* postExpr = node->child[0]->next->next;
    astnode* loopBody = node->child[1];
    m = execExpression(precon);
    while (execExpression(testcon).boolval) {
        execute(loopBody);
        m = execExpression(postExpr);
    }
    leave();
    return m;
}

Object performDefStatement(astnode* node) {
    enter("def statement");
    string id = node->attributes.strval;
    LambdaObj* lm = makeLambdaObj(node->child[1], node->child[0]);
    Object m = makeLambdaObject(lm);
    if (!cxt.scoped.empty()) {
        cxt.scoped.top()[id] = m;
    } else {
        cxt.globals[id] = m;
    }
    leave();
    return m;
}

Object performIfStatement(astnode* node) {
    enter("If statement");
    Object m;
    if (execExpression(node->child[0]).boolval) {
        m = execute(node->child[1]);
    } else {
        m = execute(node->child[2]);
    }
    leave();
    return m;
}

Object performPrintStatement(astnode* node) {
    Object m = execute(node->child[0]);
    cout<<toString(m);
    if (node->attributes.strval == "println") cout<<endl;
    return m;
}

Object execStatement(astnode* node) {
    enter("[statement]");
    Object m;
    switch (node->stmtType) {
        case PRINT_STMT:
            m = performPrintStatement(node);
            break;
        case WHILE_STMT:
            m = performWhileStatement(node);
            break;
        case IF_STMT:
            m = performIfStatement(node);
            break;
        case DEF_STMT:
            m = performDefStatement(node);
            break;
        case FOR_STMT:
            m = performForStatement(node);
            break;
        case EXPR_STMT:
            m = execExpression(node->child[0]);
            break;
        case LET_STMT:
            m = execExpression(node->child[0]);
            break;
        case RETURN_STMT: 
            m = execExpression(node->child[0]);
            break;
        default:
            break;
    }
    leave();
    return m;
}

Object execExpression(astnode* node) {
    if (node == nullptr) 
        return makeIntObject(0);
    enter("[expression]");
    Object m;
    //printNode(node);
    switch (node->exprType) {
        case CONST_EXPR:
            m = getConstValue(node);  
            break;
        case ID_EXPR:
            m = getObjectByID(node->attributes.strval);
            break;
        case BINARYOP_EXPR:
            m = eval(node);
            break;
        case UNARYOP_EXPR:
            m = evalUnaryOp(node);
            break;
        case RELOP_EXPR:
            m = eval(node);
            break;
        case ASSIGN_EXPR:
            m = execAssignment(node);
            break;
        case FUNC_EXPR:
            m = execFunction(node);
            break;
        case LAMBDA_EXPR:
            m = execLambda(node);
            break;
        case LIST_EXPR:
        case SUBSCRIPT_EXPR:
            m = execListExpression(node);
        default:
            break;
    }
    leave();
    return m;
}

Object execute(astnode* node) {
    Object m;
    if (node != nullptr) {
        switch (node->type) {
            case STMT_NODE: 
                m = execStatement(node);
                break;
            case EXPR_NODE:
                m = execExpression(node);
                break;
            default:
                break;
        }
        if (node->next != nullptr)
            m = execute(node->next);
    }
    return m;
}