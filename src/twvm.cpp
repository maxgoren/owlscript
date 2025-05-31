#include "twvm.hpp"

TWVM::TWVM(bool debug) {
    loud = debug;
    bailout = false;
}

void TWVM::push(Object info) {
    cxt.getOperandStack().push(info);
}

Object TWVM::pop() {
    if (cxt.getOperandStack().empty()) {
        cout<<"Error: Stack Underflow."<<endl;
        return cxt.nil();
    }
    return cxt.getOperandStack().pop();
}

Object& TWVM::peek(int spaces) {
    return cxt.getOperandStack().get(cxt.getOperandStack().size()-1-spaces);
}

void TWVM::exec(astnode* node) {
    if (node != nullptr) {
        if (node->nk == STMT_NODE) {
            evalStmt(node);
        } else {
            evalExpr(node);
        }
        if (!bailout)
            exec(node->next);
    }
}
Context& TWVM::context() {
    return cxt;
}

void TWVM::listExpression(astnode* node) {
    switch (node->token.symbol) {
        case TK_LB:     makeAnonymousList(node); break;
        case TK_SIZE:   getListSize(node); break;
        case TK_EMPTY:  getListEmpty(node); break;
        case TK_APPEND: doAppendList(node); break;
        case TK_PUSH:   doPushList(node); break;
        case TK_FIRST:  getFirstListElement(node); break;
        case TK_REST:   getRestOfList(node); break;
        case TK_MAP:    doMap(node);break;
        case TK_FILTER: doFilter(node); break;
        case TK_REDUCE: doReduce(node); break;
        case TK_SORT:   doSort(node); break;
        default:
            break;
    }
}
void TWVM::evalExpr(astnode* node) {
    if (node != nullptr) {
        switch (node->type.expr) {
            case UNOP_EXPR:     unaryOperation(node); break;
            case RELOP_EXPR:    binaryOperation(node); break;
            case BINOP_EXPR:    binaryOperation(node); break;
            case TERNARY_EXPR:  ternaryConditional(node); break;
            case LOGIC_EXPR:    booleanOperation(node); break;
            case BITWISE_EXPR:  binaryOperation(node); break;
            case ASSIGN_EXPR:   assignExpr(node); break;
            case CONST_EXPR:    constExpr(node); break;
            case ID_EXPR:       idExpr(node); break;
            case FUNC_EXPR:     functionCall(node); break;
            case LIST_EXPR:     listExpression(node); break;
            case SUBSCRIPT_EXPR: subscriptExpression(node); break;
            case REG_EXPR:      regularExpression(node); break;
            case REF_EXPR:      referenceExpression(node); break;
            case LAMBDA_EXPR:   lambdaExpression(node); break;
            case RANGE_EXPR:    rangeExpression(node); break;
            case ZF_EXPR:       listComprehension(node); break;
            case BLESS_EXPR:    blessExpression(node); break;
            default:
                break;
        }
    }
}
void TWVM::evalStmt(astnode* node) {
    switch (node->type.stmt) {
        case LET_STMT:      letStatement(node); break;
        case IF_STMT:       ifStatement(node); break;
        case WHILE_STMT:    whileStatement(node); break;
        case FOREACH_STMT:  foreachStatement(node); break;
        case PRINT_STMT:    printStatement(node); break;
        case EXPR_STMT:     expressionStatement(node); break;
        case FUNC_DEF_STMT: defineFunction(node); break;
        case RETURN_STMT:   returnStatement(node); break;
        case BLOCK_STMT:    blockStatement(node); break;
        case STRUCT_DEF_STMT: defineStruct(node); break;
        case BREAK_STMT:    breakStatement(node); break;
        default:
            break;
    }
}

Object TWVM::resolveFunction(astnode* node) {
    Object m;
    if (isExprType(node->child[0], LAMBDA_EXPR)) {
        evalExpr(node->child[0]);
        m = pop();
    } else if (node->child[0]->token.strval == "_rc") {
        if (!cxt.getCallStack()->accessLink) {
            m = cxt.getCallStack()->bindings["_rc"];
        } else {
            cout<<"Current scope is in the wrong context to re-call."<<endl;
        }
    } else { 
        m = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
    }
    return m;
}

void TWVM::functionCall(astnode* node) {
    Object m;
    if (isExprType(node->child[0], FUNC_EXPR)) {
        evalExpr(node->child[0]);
        m = pop();
    } else {
        m = resolveFunction(node);
    }
    if (m.type != AS_FUNC) {
        cout<<"Couldn't find function named: "<<node->child[0]->token.strval<<endl;
        return;
    }
    Function* func = m.data.gcobj->funcval;
    funcExpression(func, node->child[1]);
}

void TWVM::lambdaExpression(astnode* node) {
    Function* func = new Function(copyTree(node->child[0]), copyTree(node->child[1]));
    func->name = "(lambda)";
    func->closure = cxt.getCallStack();
    push(cxt.getAlloc().makeFunction(func));
}

void TWVM::evalFunctionArguments(astnode* args, astnode* params, ActivationRecord*& env) {
    while (params != nullptr && args != nullptr) {
        if (isExprType(params, REF_EXPR)) {
            cout<<"Bound arg '"<<args->token.strval<<"' as Reference "<<endl;
            env->bindings.insert(make_pair(params->child[0]->token.strval, makeReference(args->token.strval, args->token.depth)));
        } else {
            evalExpr(args);
            env->bindings.insert(make_pair(params->token.strval, pop())); 
        }
        params = params->next;
        args = args->next;
    }
}

void TWVM::funcExpression(Function* func, astnode* params) {
    ActivationRecord* env = new ActivationRecord(func->closure, cxt.getCallStack());
    evalFunctionArguments(params, func->params, env);
    cxt.openScope(env);
    cxt.insert("_rc", cxt.getAlloc().makeFunction(func));
    exec(func->body);
    bailout = false;
    cxt.closeScope();
}

void TWVM::regularExpression(astnode* node) {
    evalExpr(node->child[0]);
    string text = *getString(pop());
    evalExpr(node->child[1]);
    string pattern = *getString(pop());
    push(makeBool(matchre(text, pattern)));
}

void TWVM::blessExpression(astnode* node) {
    string name = node->child[0]->token.strval;
    Struct* st = cxt.getInstanceType(name);
    if (st == nullptr) {
        cout<<"No such type '"<<name<<"'"<<endl;
        return;
    }
    Struct* nextInstance = new Struct(st->typeName);
    for (auto m : st->fields) {
        nextInstance->fields[m.first] = m.second;
    }
    nextInstance->blessed = true;
    push(cxt.getAlloc().makeStruct(nextInstance));
}

void TWVM::booleanOperation(astnode* node) {
    if (node->token.symbol == TK_AND) {
        evalExpr(node->child[0]);
        if (peek(0).data.boolval) {
            pop();
            evalExpr(node->child[1]);
        }
    } else if (node->token.symbol == TK_OR) {
        evalExpr(node->child[0]);
        if (!peek(0).data.boolval) {
            pop();
            evalExpr(node->child[1]);
        }
    }
}

void TWVM::ternaryConditional(astnode* node) {
    evalExpr(node->child[0]);
    if   (getBoolean(pop())) evalExpr(node->child[1]);
    else evalExpr(node->child[2]);
}

void TWVM::referenceExpression(astnode* node) {
    evalExpr(node->child[0]);
    Object pointedAt = pop();
    if (typeOf(pointedAt) == AS_REF) {
        Object deref = cxt.get(pointedAt.data.reference->identifier, pointedAt.data.reference->scopelevel);
        cout<<"De referenced "<<toString(deref)<<" from "<<toString(pointedAt)<<endl;
        push(deref);
    }
}

void TWVM::subscriptAssignment(astnode* node) {
    astnode* tnode = node->child[0];
    evalExpr(tnode->child[0]);
    if (peek(0).type == AS_LIST) {
        List* list = getList(pop());
        evalExpr(tnode->child[1]);
        int pos = 0;
        int indx = pop().data.intval;
        ListNode* itr = list->head;
        while (itr != nullptr && pos < indx) {
            pos++;
            itr = itr->next;
        }
        evalExpr(node->child[1]);
        if (itr != nullptr) itr->info = pop();
    } else if (peek(0).type == AS_STRUCT) {
        Struct* st = getStruct(pop());
        string name = tnode->child[1]->token.strval;
        if (st->fields.find(name) == st->fields.end()) {
            cout<<"Object doesnt have field '"<<name<<"'"<<endl;
            return;
        }
        evalExpr(node->child[1]);
        st->fields[name] = pop();
    } else if (peek(0).type == AS_STRING) {
        Object strObj = pop();
        string* str = getString(strObj);
        evalExpr(tnode->child[1]);
        int indx = getInteger(pop());
        if (indx < 0 || indx > str->length()) {
            cout<<"Index out of range: "<<indx<<endl;
            return;
        }
        string back = str->substr(indx+1);
        string front = str->substr(0, indx);
        evalExpr(node->child[1]);
        string toins = *getString(pop());
        str = new string(front+toins+back);
        strObj.data.gcobj->strval = str;
        push(strObj);
    }
}

void TWVM::subscriptExpression(astnode* node) {
    evalExpr(node->child[0]);
    if (peek(0).type == AS_LIST) {
        List* list = getList(pop());
        evalExpr(node->child[1]);
        int i = 0;
        int indx = pop().data.intval;
        ListNode* itr = list->head;
        while (itr != nullptr && i < indx) {
            i++;
            itr = itr->next;
        }
        if (itr != nullptr) push(itr->info);
    } else if (peek(0).type == AS_STRUCT) {
        Struct* st = getStruct(pop());
        string name = node->child[1]->token.strval;
        if (st->fields.find(name) == st->fields.end()) {
            cout<<"Object doesnt have field '"<<name<<"'"<<endl;
            return;
        }
        push(st->fields[name]);
    } else if (peek(0).type == AS_STRING) {
        string* str = getString(pop());
        evalExpr(node->child[1]);
        int indx = getInteger(pop());
        if (indx < 0 || indx > str->length()) {
            cout<<"Index out of range: "<<indx<<endl;
            return;
        }
        char c = str->at(indx);
        string tmp;
        tmp.push_back(c);
        push(cxt.getAlloc().makeString(tmp));
    }
}

ListNode* TWVM::mergesort(ListNode* head, Function* cmp) {
    if (head == nullptr || head->next == nullptr)
        return head;
    ListNode* fast = head->next;
    ListNode* slow = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* back = slow->next;
    slow->next = nullptr;
    return merge(mergesort(head, cmp), mergesort(back, cmp), cmp);
}

void TWVM::doCompare(ListNode* front, ListNode* back, Function* cmp) {
    if (cmp != nullptr) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(front->info), toString(front->info)));
        t->next = makeExprNode(CONST_EXPR, Token(getSymbol(back->info), toString(back->info)));
        funcExpression(cmp, t);
    } else {
        push(gt(back->info, front->info));
    }
}

ListNode* TWVM::merge(ListNode* front, ListNode* back, Function* cmp) {  
    ListNode d; ListNode* c = &d;
    while (front != nullptr && back != nullptr) {
        doCompare(front, back, cmp);
        Object result = pop();
        if (result.data.boolval) {
            c->next = front; front = front->next; c = c->next;
        } else {
            c->next = back; back = back->next; c = c->next;
        }
    }
    c->next = (front == nullptr) ? back:front;
    return d.next;
}

void TWVM::doSort(astnode* node) {
    evalExpr(node->child[0]);
    Object listObj = pop();
    if (listObj.type != AS_LIST) {
        cout<<"Error: sort expects a list"<<endl;
        return;
    }
    Function* cmp = nullptr;
    if (node->child[1] != nullptr) {
        evalExpr(node->child[1]);
        cmp = getFunction(pop());
    }
    List* list = listObj.data.gcobj->listval;
    if (!listEmpty(list)) {
        list->head = mergesort(list->head, cmp);
        ListNode* x = list->head;
        while (x->next != nullptr) x = x->next;
        list->tail = x;
    }
    push(listObj);
}

void TWVM::makeAnonymousList(astnode* node) { 
    List* list = new List();
    for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
        evalExpr(it);
        list = appendList(list, pop());
    }
    push(cxt.getAlloc().makeList(list));
}

void TWVM::listComprehension(astnode* node) {
    evalExpr(node->child[0]);
    Object listobj = pop();
    if (listobj.type != AS_LIST) {
        cout<<"Error: list comprehensions only work on lists."<<endl;
        return;
    }
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    Function* pred = nullptr;
    if (node->child[2] != nullptr) {
        evalExpr(node->child[2]);
        pred = getFunction(pop());
    }
    List* list = getList(listobj);
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        if (pred != nullptr) {
            funcExpression(pred, t);
            if (pop().data.boolval) {
                funcExpression(func, t);
                result = appendList(result, pop());
            }
        } else {
            funcExpression(func, t);
            result = appendList(result, pop());
        }
    }
    push(cxt.getAlloc().makeList(result));
}

void TWVM::doAppendList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    evalExpr(node->child[1]);
    list = appendList(list, pop());
}
void TWVM::doPushList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    evalExpr(node->child[1]);
    list = pushList(list, pop());
}
void TWVM::getListSize(astnode* node) {
    evalExpr(node->child[0]);
    int size = 0;
    Object m = pop();
    if (m.type != AS_LIST && m.type != AS_STRING) {
        cout<<"Error: incorrect type supplied to size()."<<endl;
        push(makeNil());
        return;
    }
    switch (m.type) {
        case AS_LIST: size = m.data.gcobj->listval->count; break;
        case AS_STRING: size = m.data.gcobj->strval->size(); break;
        default: break;
    }
    push(makeInt(size));
}
void TWVM::getListEmpty(astnode* node) {
    evalExpr(node->child[0]);
    push(makeBool(getList(pop())->head == nullptr));
}
void TWVM::getFirstListElement(astnode* node) {
    evalExpr(node->child[0]);
    push(getList(pop())->head->info);
}
void TWVM::getRestOfList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    List* nl = new List();
    for (ListNode* it = list->head->next; it != nullptr; it = it->next)
        nl = appendList(nl, it->info);
    push(cxt.getAlloc().makeList(nl));
}
Symbol TWVM::getSymbol(Object m) {
    switch (m.type) {
        case AS_INT: return TK_NUM;
        case AS_REAL: return TK_NUM;
        case AS_BOOL: return m.data.boolval ? TK_TRUE:TK_FALSE;
        case AS_STRING: return TK_ID;
        case AS_LIST: return TK_LB;
        case AS_FUNC: return TK_LAMBDA;
        default:
            break;
    }
    return TK_NIL;
}
void TWVM::doMap(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        result = appendList(result, pop());
    }
    push(cxt.getAlloc().makeList(result));
}
void TWVM::doFilter(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        if (pop().data.boolval)
            result = appendList(result, it->info);
    }
    push(cxt.getAlloc().makeList(result));
}
void TWVM::doReduce(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(pop());
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    ListNode* it = list->head; 
    Object result = it->info;
    it = it->next;
    while (it != nullptr) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(result), toString(result)));
        t->next = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        result = pop();
        it = it->next;
    }
    push(result);
}

void TWVM::getType(astnode* node) {
    evalExpr(node);
    string typeName = "nil";
    switch (pop().type) {
        case AS_BOOL: typeName = "boolean"; break;
        case AS_INT: typeName = "integer"; break;
        case AS_REAL: typeName = "real"; break;
        case AS_LIST: typeName = "list"; break;
        case AS_STRING: typeName = "string"; break;
        case AS_STRUCT: typeName = "struct"; break;
        case AS_FUNC:   typeName = "function"; break;
        case AS_NULL:
        default:
            break;
    }
    push(cxt.getAlloc().makeString(typeName));
}

void TWVM::constExpr(astnode* node) {
    switch (node->token.symbol) {
        case TK_TRUE: push(makeBool(true)); break;
        case TK_FALSE: push(makeBool(false)); break;
        case TK_NUM: push(makeNumber(stoi(node->token.strval))); break;
        case TK_STR: push(cxt.getAlloc().makeString(node->token.strval)); break;
        case TK_NIL: push(cxt.nil()); break;
        case TK_TYPEOF: getType(node->child[0]); break;
        default: 
            break;
    }
}

void TWVM::rangeExpression(astnode* node) {
    evalExpr(node->child[0]);
    Object lhs = pop();
    int l = lhs.data.intval;
    evalExpr(node->child[1]);
    Object rhs = pop();
    int r = rhs.data.intval;
    List* nl = new List();
    if (l < r) {
        for (int i = l; i <= r; i++)
            nl = appendList(nl, makeInt(i));
    } else {
        for (int i = l; i >= r; i--)
            nl = appendList(nl, makeInt(i));
    }
    push(cxt.getAlloc().makeList(nl));
}

void TWVM::idExpr(astnode* node) {
    Object m = cxt.get(node->token.strval, node->token.depth);
    if (typeOf(m) == AS_REF) {
        //cout<<"Oh snap, a reference! "<<node->token.strval<<" is pointing to something else: "<<m.data.reference->identifier<<endl;
        Object t = cxt.get(m.data.reference->identifier, m.data.reference->scopelevel);
        //cout<<"Found: "<<toString(t)<<endl;
        push(t);
    } else push(m);
}

void TWVM::unaryOperation(astnode* node) {
    evalExpr(node->child[0]);
    switch (node->token.symbol) {
        case TK_NOT: push(makeBool(!pop().data.boolval)); break;
        case TK_SUB: push(neg(pop())); break;
        case TK_PRE_DEC: {
            Object m = pop();
            if (m.type == AS_INT) {
                m.data.intval -= 1;
            } else if (m.type == AS_REAL) {
                m.data.realval -= 1;
            }
            push(m);
            cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, m);
        } break;
        case TK_POST_DEC: {
            Object m = pop();
            push(m);
            if (m.type == AS_INT) {
                m.data.intval -= 1;
            } else if (m.type == AS_REAL) {
                m.data.realval -= 1;
            }
            cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, m);
        } break;
        case TK_PRE_INC: {
            Object m = pop();
            if (m.type == AS_INT) {
                m.data.intval += 1;
            } else if (m.type == AS_REAL) {
                m.data.realval += 1;
            }
            push(m);
            cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, m);
        } break;
        case TK_POST_INC: {
            Object m = pop();
            push(m);
            if (m.type == AS_INT) {
                m.data.intval += 1;
            } else if (m.type == AS_REAL) {
                m.data.realval += 1;
            }
            cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, m);
        } break;
        default: break;
    }
}

void TWVM::binaryOperation(astnode* node) {
    evalExpr(node->child[0]);
    evalExpr(node->child[1]);
    if (node->token.symbol == TK_ADD && (typeOf(peek(0)) == AS_STRING || typeOf(peek(1)) == AS_STRING)) {
        string newstr = toString(peek(1)) + toString(peek(0));
        Object result = cxt.getAlloc().makeString(newstr);
        pop(); pop();
        push(result);
        return;
    }
    Object rhs = pop();
    Object lhs = pop();
    switch (node->token.symbol) {
        case TK_ADD: push(add(lhs, rhs)); break;
        case TK_SUB: push(sub(lhs, rhs)); break;
        case TK_MUL: push(mul(lhs, rhs)); break;
        case TK_DIV: push(div(lhs, rhs)); break;
        case TK_MOD: push(mod(lhs, rhs)); break;
        case TK_EQU: push(equ(lhs, rhs)); break;
        case TK_NEQ: push(neq(lhs, rhs)); break;
        case TK_POW: push(pow(lhs, rhs)); break;
        case TK_LT:  push(lt(lhs, rhs)); break;
        case TK_LTE: push(lte(lhs, rhs)); break;
        case TK_GT:  push(gt(lhs, rhs)); break;
        case TK_GTE: push(gte(lhs, rhs)); break;
        case TK_BIT_AND: push(bwAnd(lhs, rhs)); break;
        case TK_BIT_XOR: push(bwXor(lhs, rhs)); break;
        case TK_BIT_OR:  push(bwOr(lhs, rhs)); break;
        default:
            break;
    }
}

void TWVM::assignExpr(astnode* node) {
    if (isExprType(node->child[0], ID_EXPR)) {
        evalExpr(node->child[1]);
        switch (node->token.symbol) {
            case TK_ASSIGN: {
                Object t = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
                if (t.type == AS_REF) {
                    cout<<"Oh snap, implicit deref"<<endl;
                    cxt.put(t.data.reference->identifier, t.data.reference->scopelevel, pop());
                } else {
                    cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, pop());
                }
            } break;
            case TK_ASSIGN_SUM: {
                Object lhs = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
                Object rhs = pop();
                cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, add(lhs, rhs));
            } break;
            case TK_ASSIGN_DIFF: {
                Object lhs = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
                Object rhs = pop();
                cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, sub(lhs, rhs));
            } break;
            case TK_ASSIGN_PROD: {
                Object lhs = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
                Object rhs = pop();
                cxt.put(node->child[0]->token.strval, node->child[0]->token.depth, mul(lhs, rhs));
            } break;
            default:
                break;
        }
    } else if (isExprType(node->child[0], SUBSCRIPT_EXPR)) {
        subscriptAssignment(node);
    }
}

void TWVM::letStatement(astnode* node) {
    evalExpr(node->child[0]);
}
void TWVM::ifStatement(astnode* node) {
    evalExpr(node->child[0]);
    if (pop().data.boolval) {
        exec(node->child[1]);
    } else {
        exec(node->child[2]);
    }
}

void TWVM::breakStatement(astnode* node) {
    breakloop = true;
}

void TWVM::whileStatement(astnode* node) {
    evalExpr(node->child[0]);
    breakloop = false;
    while (pop().data.boolval) {
        exec(node->child[1]);
        if (breakloop) {
            break;
        }
        exec(node->child[0]);
    }
    breakloop = false;
}
void TWVM::foreachStatement(astnode* node) {
    evalExpr(node->child[1]);
    List* list = getList(pop());
    string itername = node->child[0]->token.strval;
    for (auto it = list->head; it != nullptr; it = it->next) {
        cxt.insert(itername, it->info);
        exec(node->child[2]);
    }
    cxt.remove(itername);
}
void TWVM::printStatement(astnode* node) {
    evalExpr(node->child[0]);
    cout<<toString(pop());
    if (node->token.symbol == TK_PRINTLN)
        cout<<endl;
}
void TWVM::defineFunction(astnode* node) {
    Function* func = new Function(copyTree(node->child[0]), copyTree(node->child[1]));
    func->name = node->token.strval;
    func->closure = cxt.getCallStack();
    Object m = cxt.getAlloc().makeFunction(func);
    cxt.insert(func->name, m);
}
void TWVM::defineStruct(astnode* node) {
    Struct* st = new Struct(node->child[0]->token.strval);
    for (astnode* it = node->child[1]; it != nullptr; it = it->next) {
        st->fields[it->child[0]->token.strval] = makeNil();
    }
    cxt.addStructType(st);
}
void TWVM::blockStatement(astnode* node) {
    cxt.openScope();
    exec(node->child[0]);
    cxt.closeScope();
}
void TWVM::expressionStatement(astnode* node) {
    evalExpr(node->child[0]);
}
void TWVM::returnStatement(astnode* node) {
    evalExpr(node->child[0]);
    bailout = true;
}
