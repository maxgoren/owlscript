#include "ast_interpreter.hpp"

Object ASTInterpreter::execAppendList(astnode* node) {
    enter("append to list");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    Object t = evalExpression(node->child[1]);
    appendToList(getList(m), t);
    updateContext(id, m, getAttributes(node->child[0]).depth);
    if (typeOf(m) == AS_FILE) {
        saveFile(m);
    }
    leave();
    return t;
}

Object ASTInterpreter::execPushList(astnode* node) {
    enter("push to list");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    Object t = evalExpression(node->child[1]);
    pushToList(getList(m), t);
    updateContext(id, m, getAttributes(node->child[0]).depth);
    if (typeOf(m) == AS_FILE) {
        saveFile(m);
    }
    leave();
    return t;
}

Object ASTInterpreter::execPopList(astnode* node) {
    enter("pop list");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    popList(getList(m));
    updateContext(id, m,getAttributes(node->child[0]).depth);
    if (typeOf(m) == AS_FILE) {
        saveFile(m);
    }
    leave();
    return m;
}

Object ASTInterpreter::execPopBackList(astnode* node) {
    enter("unshift list");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    popBackList(getList(m));
    updateContext(id, m, getAttributes(node->child[0]).depth);
    leave();
    return m;
}

Object ASTInterpreter::execLength(astnode* node) {
    enter("list length");
    string id;
    Object m, t;
    resolveObjForExpression(node, id, m);
    switch (typeOf(m)) {
        case AS_FILE:
        case AS_LIST:
            t = makeIntObject(listLength(getList(m)));
            break;
        case AS_STRING:
            t = makeIntObject(getString(m)->length);
            break;
        default:
            break;
    }
    leave();
    return t;
}

Object ASTInterpreter::execIsEmptyList(astnode* node) {
    enter("test list for empty");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    Object t = makeBoolObject(listLength(getList(m)) == 0);
    leave();
    return t;
}

ListNode* ASTInterpreter::merge(ListNode* a, ListNode* b, LambdaObj* compFunc) {
    ListNode d; ListNode* c = &d;
    while (a && b) {
        bool cmpResult = false;
        if (compFunc == nullptr) {
            cmpResult = compareUnknownTypes(a->info, b->info);
        } else {
            astnode* args = makeExprNode(CONST_EXPR, Token(getSymbol(a->info), toString(a->info)));
            args->next = makeExprNode(CONST_EXPR, Token(getSymbol(b->info), toString(b->info)));
            cmpResult = evalFunctionExpr(compFunc, args).boolval;
        }
        if (cmpResult) {
            c->next = a; a = a->next; c = c->next;
        } else {
            c->next = b; b = b->next; c = c->next;
        }
    }
    c->next = (a == nullptr) ? b:a;
    return d.next;
}

ListNode* ASTInterpreter::mergesort(ListNode* list, LambdaObj* compFunc) {
    if (list == nullptr || list->next == nullptr)
        return list;
    ListNode* slow = list, *fast = list->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* front = list, *back = slow->next;
    slow->next = nullptr;
    return merge(mergesort(front, compFunc), mergesort(back, compFunc), compFunc);
}

Object ASTInterpreter::execSortList(astnode* node) {
    enter("sort list");
    Object m;
    string id;
    resolveObjForExpression(node, id, m);
    ListObj* list = getList(m);
    Object lm = evalExpression(node->child[1]);
    LambdaObj* lmbd = nullptr;
    if (node->child[1] != nullptr) {
        Object lm = evalExpression(node->child[1]);
        lmbd = getLambda(lm);
    }
    ListNode* head = mergesort(list->head, lmbd);
    ListNode* tail = head;
    while (tail->next != nullptr)
        tail = tail->next;
    list->head = head;
    list->tail = tail;
    m.objval->listObj = list;
    if (!id.empty())
        updateContext(id, m, getAttributes(node->child[0]).depth);
    
    leave();
    return m;
}

Object ASTInterpreter::execFirst(astnode* node) {
    enter("car");
    Object m;
    string id;
    resolveObjForExpression(node, id, m);
    if (typeOf(m) != AS_LIST) {
        cout<<"Error: 'first' can only be performed on list types."<<endl;
        return makeNilObject();
    }
    Object t = getList(m)->head->info;
    leave();
    return t;
}

Object ASTInterpreter::execRest(astnode* node) {
    enter("cdr");
    Object m;
    string id;
    resolveObjForExpression(node, id, m);
    if (typeOf(m) != AS_LIST) {
        cout<<"Error: 'rest' can only be performed on list types."<<endl;
        return makeNilObject();
    }
    ListObj* list = makeListObj();
    for (ListNode* it = getList(m)->head->next; it != nullptr; it = it->next)
        appendToList(list, it->info);
    leave();
    Object mr = makeListObject(list);
    gc.add(mr.objval);
    return mr;
}

Symbol ASTInterpreter::getSymbol(Object m) {
    switch (m.type) {
        case AS_INT: return TK_NUM;
        case AS_REAL: return TK_REALNUM;
        case AS_BOOL: return m.boolval ? TK_TRUE:TK_FALSE;
        case AS_STRING: return TK_ID;
        case AS_LIST: return TK_LSQUARE;
        case AS_LAMBDA: return TK_LAMBDA;
        default:
            break;
    }
    return TK_NONE;
}

Object ASTInterpreter::execMap(astnode* node) {
    enter("Map list");
    Object m;
    string id;
    resolveObjForExpression(node, id, m);
    Object lm = evalExpression(node->child[1]);
    LambdaObj* lmbd = getLambda(lm);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(m)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        Object r = evalFunctionExpr(lmbd, t);
        appendToList(result, r);
    }
    m = makeListObject(result);
    gc.add(m.objval);
    if (typeOf(m) == AS_FILE) {
        saveFile(m);
    }
    leave();
    return m;
}

Object ASTInterpreter::execFilter(astnode* node) {
    enter("Map list");
    Object m;
    string id;
    resolveObjForExpression(node, id, m);
    Object lm = evalExpression(node->child[1]);
    LambdaObj* lmbd = getLambda(lm);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(m)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        Object r = evalFunctionExpr(lmbd, t);
        if (r.boolval) {
            appendToList(result, it->info);
        }
    }
    m = makeListObject(result);
    gc.add(m.objval);
    if (typeOf(m) == AS_FILE) {
        saveFile(m);
    }
    leave();
    return m;
}

Object ASTInterpreter::performListComprehension(astnode* node) {
    enter("[List Comprehension]");
    Object il = evalExpression(node->child[0]);
    if (typeOf(il) != AS_LIST) {
        leave();
        return makeNilObject();
    }
    Object lm = evalExpression(node->child[1]);
    Object prd = evalExpression(node->child[2]);
    LambdaObj* lmbd = getLambda(lm);
    LambdaObj* pred = getLambda(prd);
    ListObj* list = makeListObj();
    for (ListNode* it = getList(il)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        if (pred != nullptr) {
            if (evalFunctionExpr(pred, t).boolval)
                appendToList(list,  evalFunctionExpr(lmbd, t));
        } else {
            appendToList(list, evalFunctionExpr(lmbd, t));
        }
    }
    leave();
    return makeListObject(list);
}

Object ASTInterpreter::evalRangeExpression(astnode* node) {
    enter("[range expr]");
    Object from = evalExpression(node->child[0]);
    Object to = evalExpression(node->child[1]);
    ListObj* list = makeListObj();
    int a = getAsReal(from);
    int b = getAsReal(to);
    bool swapped = false;
    if (b < a) {
        int tmp = a;
        a = b;
        b = tmp;
        swapped = true;
    }
    for (int i = a; i <= b; i++) {
        if (swapped) {
            pushToList(list, makeIntObject(i));
        } else {
            appendToList(list, makeIntObject(i));
        }
    }
    leave();
    return makeListObject(list);
}

Object ASTInterpreter::performFileOpenExpression(astnode* node) {
    enter("[File Open Expr]");
    Object m;
    Object fn = evalExpression(node);
    FileObj* fobj = makeFileObj(fn.objval->stringObj);
    fobj->lines = makeListObj();
    ifstream ifile(toString(fn));
    leave();
    if (!ifile.is_open()) {
        ofstream ofile(toString(fn));
        if (ofile.is_open()) {
            ofile<<' ';
            ofile.close();
            m = makeFileObject(fobj);
            return m;
        }
        cout<<"Error: could not open file '"<<toString(fn)<<"'"<<endl;
        return makeNilObject();
    }
    string buff;
    ListObj* list = makeListObj();
    while (ifile.good()) {
        getline(ifile, buff);
        Object m = makeStringObject(buff);
        list = appendToList(list, m);
    }
    fobj->lines = list;
    m = makeFileObject(fobj);
    gc.add(m.objval);
    return m;
}

Object ASTInterpreter::performCreateUnNamedList(astnode* node) {
    Object m;
    enter("[unnamed list]");
    ListObj* list = makeListObj();
    if (isExprType(node->child[0],RANGE_EXPR) || isExprType(node->child[0], LISTCOMP_EXPR)) {
            m = evalExpression(node->child[0]);
    } else {
        for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
            Object m = evalExpression(it);
            appendToList(list, m);
        }
        m = makeListObject(list);
    }
    gc.add(m.objval);
    leave();
    return m;
}