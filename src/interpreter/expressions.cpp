#include "interpreter.hpp"

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
            case MOD:
                if (right == 0) {
                    cout<<"Error: attempted divide by zero"<<endl;
                    return makeRealObject(0.0f);
                }
                return makeRealObject((int)left % (int)right);
            case MULTIPLY: return makeRealObject(left*right);
            case LESS:     return makeBoolObject(left < right);
            case GREATER:  return makeBoolObject(left > right);
            case LTE:      return makeBoolObject(left <= right);
            case GTE:      return makeBoolObject(left >= right);
            case EQUAL:    return makeBoolObject(left == right);
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