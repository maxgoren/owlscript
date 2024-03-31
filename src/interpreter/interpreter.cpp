#include "interpreter.hpp"

ActivationRecord::ActivationRecord() {

}

Object* Interpreter::eval(ASTNode* node) {
    enter("eval");
    Object* lhs = expression(node->left);
    Object* rhs = expression(node->right);
    if (dontEval.find(lhs->type) == dontEval.end() && dontEval.find(rhs->type) == dontEval.end()) {
        double left = stof(toString(lhs));
        double right = stof(toString(rhs));
        switch (node->data.tokenVal) {
            case PLUS:     return makeRealObject(left+right);
            case MINUS:    return makeRealObject(left-right);
            case DIVIDE:
                if (right == 0) {
                    cout<<"Error: attempted divide by zero"<<endl;
                    return makeRealObject(0.0f);
                }
                return makeRealObject(left/right);
            case MULTIPLY: return makeRealObject(left*right);
            case EQUAL:    return makeBoolObject(left == right);
            case LESS:     return makeBoolObject(left < right);
            case GREATER:  return makeBoolObject(left > right);
            case NOTEQUAL: return makeBoolObject(left != right);
            default:
                cout<<"Unknown Operator: "<<node->data.stringVal<<endl;
        }
    } else if ((lhs->type == AS_STRING || rhs->type == AS_STRING) && node->data.tokenVal == PLUS) {
        return makeStringObject(new string(toString(lhs) + toString(rhs)));
    } else {
        cout<<"Error: Unsupported operation for those types."<<endl;
    }
    leave();
    return makeRealObject(-1.0f);
}

ActivationRecord* Interpreter::prepareActivationRecord(ASTNode* node) {
    ActivationRecord* ar = new ActivationRecord();
    Procedure* func = procedures[node->data.stringVal];
    ar->function = func;
    ASTNode* t = node->left;
    for (auto it = func->paramList; it != nullptr; it = it->left) {
        ar->env.insert(make_pair(it->data.stringVal, memStore.storeAtNextFree(expression(t))));
        say(it->data.stringVal + " added to AR.");
        if (t->left != nullptr)
            t = t->left;
    }
    ar->returnValue = makeRealObject(0.0);
    ar->staticLink = callStack.top();
    return ar;
}

Object* Interpreter::runClosure(ASTNode* node, Object* obj) {
    auto clos = obj->closure;
    ActivationRecord* ar = new ActivationRecord;
    ar->function = new Procedure;
    ar->function->functionBody = clos->functionBody;
    ar->env = clos->env;
    ASTNode* t = node->left;
    for (auto it = clos->paramList; it != nullptr; it = it->left) {
        ar->env.insert(make_pair(it->data.stringVal, memStore.storeAtNextFree(expression(t))));
        say(it->data.stringVal + " added to AR.");
        if (t->left != nullptr)
            t = t->left;
    }
    ar->returnValue = makeRealObject(0.0);
    ar->staticLink = callStack.top();
    callStack.push(ar);
    auto body = callStack.top()->function->functionBody;
    stopProcedure = false;
    say("Executing lambda");
    run(body);
    Object* retVal = callStack.top()->returnValue;

    //arguments get freed as normal whether a plain lambda or closure
    for (auto it = clos->paramList; it != nullptr; it = it->left) {
        string name = it->data.stringVal;
        int freeAddr = clos->env[name];
        memStore.free(freeAddr);
        clos->env.erase(name);
    }
    //Things are a bit trickier for free variables.
    if (clos->isClosure) {
        for (auto m : callStack.top()->env) {
            int oldAddr = clos->env[m.first];
            int replaceAddr = callStack.top()->env[m.first];
            clos->env[m.first] = replaceAddr;
            memStore.free(oldAddr);
        }
    }
    callStack.pop();
    return retVal;
}

Object* Interpreter::procedureCall(ASTNode* node) {
    Object* retVal;
    enter("[procedureCall]");
    if (procedures.find(node->data.stringVal) != procedures.end()) {
        callStack.push(prepareActivationRecord(node));
        say("Calling: " + node->data.stringVal);
        auto body = callStack.top()->function->functionBody;
        stopProcedure = false;
        run(body);
        retVal = callStack.top()->returnValue;
        say("Returned: " + to_string(retVal->realVal) + " from " + node->data.stringVal);
        for (auto toFree : callStack.top()->env) {
            memStore.free(toFree.second);
        }
        callStack.pop();
        return retVal;
    }
    int addr = getAddress(node->data.stringVal);
    Object* obj = memStore.get(addr);
    if (addr > 0 && obj->type == AS_CLOSURE) {
        return runClosure(node, obj);
    } else {
        say("No such function: " + node->data.stringVal);
    }
    leave();
    return retVal;
}

Object* Interpreter::lambdaExpr(ASTNode* node) {
    bool isClosure = (!callStack.empty());
    return makeClosureObject(makeLambda(node->right, node->left, isClosure ? callStack.top()->env:Environment(), isClosure));
}

Object* Interpreter::listExpr(ASTNode* node) {
    enter("[list_expr]");
    ListHeader* list = new ListHeader;
    list->size = 0;
    ASTNode* t = node->left;
    ListNode d;
    ListNode* x = &d;
    if (t == nullptr) {
        x = new ListNode;
        list->size = 0;
        list->head = nullptr;
        leave();
        return makeListObject(list);
    }
    while (t != nullptr) {
        Object* obj = expression(t);
        say("push: " + toString(obj));
        x->next = new ListNode;
        x = x->next;
        x->data = obj;
        x->next = nullptr;
        t = t->left;
        list->size++;
    }
    list->head = d.next;
    leave();
    return makeListObject(list);
}

//resolve names to addresses using innermost nesting rule.
int Interpreter::getAddress(string name) {
    int addr = 0; //address zerp is never used, and is used as a control address.

    //if we are in a procedure call, check the procedures symbol table first.
    if (!callStack.empty() && callStack.top()->env.find(name) != callStack.top()->env.end())
        addr = callStack.top()->env[name];

    //If address is still zero, we havent found the variable yet.
    //Are we in a nested procedure? Check the outter procedures symbol table.
    if (addr == 0 && callStack.size() > 1 && callStack.top()->staticLink->env.find(name) != callStack.top()->staticLink->env.end())
        addr = callStack.top()->staticLink->env[name];

    //If the address is still zero, check the global symbol table
    if (addr == 0 && st.find(name) != st.end())
        addr = st[name];
   
    if (addr == 0)
        cout<<"Error: No var named "<<name<<" found."<<endl;
    
    return addr;
}

Object* Interpreter::listSize(ASTNode* node) {
    enter("[list size]");
    bool is_local = false;
    Object* obj;
    int addr, size;
    string name = node->left->data.stringVal;
    addr = getAddress(name);
    if (addr > 0) {
        obj = memStore.get(addr);
        size = obj->list->size;
    }
    leave();
    return makeIntObject(size);
}

Object* Interpreter::carExpr(ASTNode* node) {
    enter("[carExpr]");
    bool is_local = false;
    Object* obj;
    int addr, size;
    string name = node->left->data.stringVal;
    addr = getAddress(name);
    if (addr > 0) {
        obj = memStore.get(addr);
        return obj->list->head->data;
    }
    leave();
    return makeIntObject(size);
}

Object* Interpreter::cdrExpr(ASTNode* node) {
    enter("[carExpr]");
    bool is_local = false;
    Object* obj;
    int addr, size;
    string name = node->left->data.stringVal;
    addr = getAddress(name);
    if (addr > 0) {
        obj = memStore.get(addr);
        ListHeader* cdr = new ListHeader;
        cdr->size = obj->list->size - 1;
        cdr->head = obj->list->head->next;
        return makeListObject(cdr);
    }
    leave();
    return makeIntObject(size);
}


Object* Interpreter::sortList(ASTNode* node) {
    enter("[sort]");
    Object* obj;
    ListHeader* list = new ListHeader;
    list->head = nullptr;
    list->size = 0;
    int addr = 0;
    string name = node->left->data.stringVal;
    addr = getAddress(name);
    if (addr > 0) {
        obj = memStore.get(addr);
        list->head = copyList(obj->list->head);
        list->head = mergeSortList(list->head);
        list->size = obj->list->size;
    }
    leave();
    return makeListObject(list);
}

//I don't know how I feel about this.
//It has a blinking in and out of reality feel 
Object* Interpreter::mapExpr(ASTNode* node) {
    Object* lambdaObj = lambdaExpr(node->left);
    Object* listObj = nullptr;
    if (node->right->kind == EXPRNODE && node->right->type.expr == LIST_EXPR) {
        listObj = listExpr(node->right);
    } else {
        auto listAddr = getAddress(node->right->data.stringVal);
        listObj = memStore.get(listAddr);
    }
    ListHeader* resultList = new ListHeader;
    resultList->size = 0;
    resultList->head = nullptr;
    ListNode d;
    ListNode* tmpR = &d;
    for (auto it = listObj->list->head; it != nullptr; it = it->next) {
        ASTNode* t = new ASTNode;
        t->left = makeExprNode(CONST_EXPR, NUMBER, toString(it->data));
        t->right = nullptr; t->mid = nullptr; t->next = nullptr;
        auto result = runClosure(t, lambdaObj);
        tmpR->next = new ListNode;
        tmpR->next->data = result;
        tmpR = tmpR->next;
        tmpR->next = nullptr;
    }
    resultList->head = d.next;
    return makeListObject(resultList);
}

Object* Interpreter::getListItem(ASTNode* node, Object* listObj) {
    enter("listEntry");
    if (node->left == nullptr)
        return listObj;
    Object* tmp = expression(node->left);
    int arrIndex = tmp->realVal;
    say("Index: " + to_string(arrIndex));
    ListNode* x = listObj->list->head;
    if (arrIndex == 0)
        return x->data;
    int i = 0;
    while (i < arrIndex) {
        if (x->next != nullptr)
            x = x->next;
        else break;
        i++;
    }
    leave();
    return x->data;
}

void Interpreter::pushList(ASTNode* node) {
    enter("[push list]");
    Object* listObj;
    int addr;
    auto name = node->left->data.stringVal;
    say("list: " + name);
    Object* value = expression(node->right);
    say("push: " + toString(value));
    ListNode *toAdd = new ListNode;
    toAdd->data = value;
    toAdd->next = nullptr;
    addr = getAddress(name);
    if (addr > 0) {
        addr = st[name];
        listObj = memStore.get(addr);
        toAdd->next = listObj->list->head;
        listObj->list->head = toAdd;
        listObj->list->size += 1;
        memStore.store(addr, listObj);
    }
    leave();
}

void Interpreter::appendList(ASTNode* node) {
    enter("[append list]");
    Object* listObj;
    int addr;
    auto name = node->left->data.stringVal;
    say("list: " + name);
    Object* value = expression(node->right);
    say("push: " + toString(value));
    ListNode *toAdd = new ListNode;
    toAdd->data = value;
    toAdd->next = nullptr;
    addr = getAddress(name);
    if (addr > 0) {
        addr = st[name];
        listObj = memStore.get(addr);
        ListNode* itr = listObj->list->head;
        if (itr == nullptr) {
            listObj->list->head = toAdd;
        } else {
            while (itr->next != nullptr) itr = itr->next;
            itr->next = toAdd;
        }
        listObj->list->size += 1;
        memStore.store(addr, listObj);
    }
    leave();
}

void Interpreter::popList(ASTNode* node) {
    enter("[pop List]");
    bool is_local = false;
    Object* obj;
    int addr;
    auto name = node->left->data.stringVal;
    addr = getAddress(name);
    if (addr == 0) {
        cout<<"Error: No List named "<<name<<" found."<<endl;
        return;
    }
    obj = memStore.get(addr);
    ListNode* x = obj->list->head;
    Object* result;
    if (x != nullptr) {
        obj->list->head = obj->list->head->next;
        result = x->data;
        obj->list->size -= 1;
        memStore.store(addr, obj);
    }
    leave();
}

Object* Interpreter::getVariableValue(ASTNode* node) {
    int addr = 0;
    Object* result;
    string name = node->data.stringVal;
    addr = getAddress(name);
    result = memStore.get(addr);
    say(name + " resolves to address: " + to_string(addr) + ", value: " + toString(result));
    if (result->type == AS_LIST && node->left != nullptr) {
         result = getListItem(node, result);
    }
    return result;
}

Object* Interpreter::expression(ASTNode* node) {
    Object* result, *tmp;
    int addr, arrIndex = 0;
    switch (node->type.expr) {
        case OP_EXPR:
            enter("[op expression]" + node->data.stringVal); leave();
            return eval(node);
        case ID_EXPR:
            enter("[id expression]");
            result = getVariableValue(node);
            if (result != nullptr) {
                leave();
                return result;
            }
            say("no variable named: " + node->data.stringVal);
            leave();
            return makeStringObject(new string("(err)"));
        case CONST_EXPR:
            enter("[const expression] " + node->data.stringVal); leave();
            if (node->data.stringVal == "true") return makeBoolObject(true);
            if (node->data.stringVal == "false") return makeBoolObject(false);
            if (node->data.stringVal == "nil") return makeNilObject();
            return makeRealObject(stof(node->data.stringVal.c_str()));
        case FUNC_EXPR:
            enter("[func_expr] " + node->data.stringVal); leave();
            return procedureCall(node);
        case LAMBDA_EXPR:
            enter("[lambda_expr]"); leave();
            return lambdaExpr(node);
        case STRINGLIT_EXPR:
            enter("[string literal expression]"); leave();
            return makeStringObject(&(node->data.stringVal));
        case CAR_EXPR: 
            enter("[car expr]"); leave();
            return carExpr(node);
        case CDR_EXPR:
            enter("[cdr expr]"); leave();
            return cdrExpr(node);
        case LIST_EXPR:
            enter("[list expression]"); leave();
            return listExpr(node);
        case LISTLEN_EXPR:
            enter("[listlen expr]"); leave();
            return listSize(node);
        case SORT_EXPR:
            enter("[sortlist_expr]"); leave();
            return sortList(node);
        case MAP_EXPR:
            enter("map_expr"); leave();
            return mapExpr(node);
        default:
            break;
    }
    leave();
    return makeRealObject(0.0f);
}

void Interpreter::printStmt(ASTNode* node) {
    enter("[print]" + node->data.stringVal);
    Object* obj = expression(node->left);
    if (node->data.stringVal == "println") {
        cout<<toString(obj)<<endl;
    } else {
        cout<<toString(obj)<<flush;
    }
    leave();
}

void Interpreter::readStmt(ASTNode* node) {
    enter("[input]");
    string input;
    getline(cin, input);
    say("read in: " + input);
    int addr = getAddress(node->left->data.stringVal);
    if (addr == 0) {
        addr = memStore.storeAtNextFree(makeStringObject(new string(input)));
    } else {
        memStore.store(addr, makeStringObject(new string(input)));
    }
    leave();
}

void Interpreter::ifStmt(ASTNode* node) {
    enter("[if statement]");
    ASTNode* branch = nullptr;
    say("testing condition");
    auto result = expression(node->left)->boolVal;
    if (result) {
        say("executing matching result");
        branch = node->mid;
    } else {
        say("executing else clause");
        branch = node->right;
    }
    for (ASTNode* t = branch; t != nullptr; t = t->next ) {
        statement(t);
    }
    leave();
}

void Interpreter::loopStmt(ASTNode* node) {
    enter("[loop]");
    while (expression(node->left)->boolVal) {
        ASTNode* t = node->right;
        while (t != nullptr) {
            statement(t);
            t = t->next;
        }
    }
    leave();
}

void Interpreter::assignStmt(ASTNode* node) {
    enter("[assign]");
    string name;
    Object* value;
    if (node->left != nullptr) {
        name = node->left->data.stringVal;
    } else {
        cout<<"Error: missing name"<<endl;
        leave();
        return;
    }
    if (node->right != nullptr) {
        value = expression(node->right);
        int saveAddr = memStore.storeAtNextFree(value);
        if (!callStack.empty()) {
            callStack.top()->env[name] = saveAddr;
        } else {
            st[name] = saveAddr;
        }
    } else {
        cout<<"Error: missing assignment value"<<endl;
    }
    leave();
}

void Interpreter::defineFunction(ASTNode* node) {
    enter("define function");
    Procedure* np = new Procedure;
    np->name = node->data.stringVal;
    np->paramList = node->left;
    np->functionBody = node->right;
    procedures[np->name] = np;
    leave(np->name + " defined.");
}

void Interpreter::returnStmt(ASTNode* node) {
    enter("[return]");
    if (!callStack.empty()) {
        callStack.top()->returnValue = expression(node->left);
        say("Returning: " + toString(callStack.top()->returnValue));
        stopProcedure = true;
    }
    leave();
}

void Interpreter::statement(ASTNode* node) {
    enter("[statement]");
    switch (node->type.stmt) {
        case PRINT_STMT:
            printStmt(node);
            break;
        case READ_STMT:
            readStmt(node);
            break;
        case ASSIGN_STMT:
            assignStmt(node);
            break;
        case PUSH_STMT:
            pushList(node);
            break;
        case APPEND_STMT:
            appendList(node);
            break;
        case POP_STMT:
            enter("[pop list expr]"); leave();
            popList(node);
            break;
        case DEF_STMT:
            defineFunction(node);
            break;
        case IF_STMT:
            ifStmt(node);
            break;
        case LOOP_STMT:
            loopStmt(node);
            break;
        case RETURN_STMT:
            returnStmt(node);
            break;
        default: 
            cout<<"Invalid Statement: "<<node->data.stringVal<<endl;
        break;
    }
    leave();
}

Interpreter::Interpreter() {
    recDepth = 0;
    stopProcedure = false;
    dontEval.insert({AS_LIST, AS_CLOSURE, AS_STRING});
}

void Interpreter::run(ASTNode* node) {
    if (node == nullptr)
        return;
    switch(node->kind) {
        case STMTNODE:
            statement(node);
            break;
        case EXPRNODE:
            expression(node);
            break;
    }
    if (stopProcedure) {
        stopProcedure = false;
        return;
    }
    leave();
    run(node->next);
}

void Interpreter::enter(string s) {
    recDepth++;
    say(s);
}

void Interpreter::setLoud(bool l) {
    loud = l;
}

void Interpreter::say(string s) {
    if (loud) {
        for (int i = 0; i < recDepth; i++)
            cout<<"  ";
        cout<<"("<<recDepth<<") "<<s<<endl;
    }
}

void Interpreter::leave(string s) {
    say(s);
    recDepth--;
}

void Interpreter::leave() {
    --recDepth;
}