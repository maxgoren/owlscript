#include "interpreter.hpp"

bool Interpreter::isRelOp(TOKENS token) {
    switch (token) {     
        case LTE:      
        case GTE:  
        case LESS:
        case EQUAL:    
        case GREATER:  
        case NOTEQUAL: return true;
        default: break; 
    }
    return false;
}

Object* Interpreter::evalRelOp(TOKENS op, double left, double right) {
    switch (op) {
        case LESS:     return makeBoolObject(left < right);
        case GREATER:  return makeBoolObject(left > right);
        case LTE:      return makeBoolObject(left <= right);
        case GTE:      return makeBoolObject(left >= right);
        case EQUAL:    return makeBoolObject(left == right);
        case NOTEQUAL: return makeBoolObject(left != right);
    }
}

Object* Interpreter::eval(ASTNode* node) {
    enter("eval" + node->data.stringVal);
    Object* lhs = expression(node->left);
    Object* rhs = expression(node->right);
    if (dontEval.find(lhs->type) == dontEval.end() && dontEval.find(rhs->type) == dontEval.end()) {
        double left, right;
        if (lhs->type == AS_CLOSURE) {
            right = stof(toString(runClosure(node, lhs)).c_str());
        } else {
            left = stof(toString(lhs).c_str());
        }
        if (rhs->type == AS_CLOSURE) {
            right = stof(toString(runClosure(node, rhs)).c_str());
        } else {
            right = stof(toString(rhs).c_str());
        }
        if (isRelOp(node->data.tokenVal)) {
            return evalRelOp(node->data.tokenVal, left, right);
        }
        switch (node->data.tokenVal) {
            case SQRT:     return makeRealObject(sqrt(left));
            case POW:      return makeRealObject(pow(left, right));
            case PLUS:     return makeRealObject(left+right);
            case MINUS:    return makeRealObject(left-right);
            case MULTIPLY: return makeRealObject(left*right);
            case DIVIDE:
                if (right == 0) {
                    cout<<"Error: attempted divide by zero"<<endl;
                    return makeIntObject(0);
                }
                return makeRealObject(left/right);
            case MOD:
                if (right == 0) {
                    cout<<"Error: attempted divide by zero"<<endl;
                    return makeIntObject(0);
                }
                return makeRealObject((int)left%(int)right);
            default:
                cout<<"Unknown Operator: "<<node->data.stringVal<<endl;
        }
    } else if ((lhs->type == AS_STRING || rhs->type == AS_STRING) && node->data.tokenVal == PLUS) {
        return makeStringObject(new string(toString(lhs) + toString(rhs)));
    } else if ((lhs->type == AS_LIST) && (rhs->type == AS_LIST) && node->data.tokenVal == PLUS) {
        ListHeader* newList = makeListHeader();
        for (auto list = lhs->list->head; list != nullptr; list = list->next)
            push_back_list(newList, list->data);
        for (auto list = rhs->list->head; list != nullptr; list = list->next)
            push_back_list(newList, list->data);
        return makeListObject(newList);
    } else {
        cout<<"Error: Unsupported operation for those types: "<<(lhs->type)<<", "<<(rhs->type)<<endl;
    }
    leave();
    return makeIntObject(0);
}

Object* Interpreter::runClosure(ASTNode* node, Object* obj) {
    auto clos = obj->closure;
    ActivationRecord ar;
    ar.function = new Procedure;
    ar.function->functionBody = clos->functionBody;
    ar.env = clos->env;
    ASTNode* t = node->left;
    for (auto it = clos->paramList; it != nullptr; it = it->left) {
        ar.env.insert(make_pair(it->data.stringVal, memStore.storeAtNextFree(t == nullptr ? makeIntObject(0):expression(t))));
        say(it->data.stringVal + " added to AR.");
        if (t != nullptr && t->left != nullptr)
            t = t->left;
    }
    ar.returnValue = makeIntObject(0);
    ar.staticLink = &callStack.top();
    callStack.push(ar);
    auto body = callStack.top().function->functionBody;
    stopProcedure = false;
    say("Executing lambda");
    run(body);
    Object* retVal = callStack.top().returnValue;

    //arguments get freed as normal whether a plain lambda or closure
    for (auto it = clos->paramList; it != nullptr; it = it->left) {
        string name = it->data.stringVal;
        int freeAddr = clos->env[name];
        memStore.free(freeAddr);
        clos->env.erase(name);
    }
    //Things are a bit trickier for free variables.
    if (clos->isClosure) {
        for (auto m : callStack.top().env) {
            int oldAddr = clos->env[m.first];
            int replaceAddr = callStack.top().env[m.first];
            clos->env[m.first] = replaceAddr;
            memStore.free(oldAddr);
        }
    }
    callStack.pop();
    return retVal;
}

Object* Interpreter::runProcedure(ASTNode* node) {
    callStack.push(prepareActivationRecord(node));
    ASTNode* body = callStack.top().function->functionBody;
    say("Calling: " + node->data.stringVal);
    stopProcedure = false;
    run(body);
    Object* retVal = callStack.top().returnValue;
    say("Returned: " + toString(retVal) + " from " + node->data.stringVal);
    for (auto toFree : callStack.top().env) {
        memStore.free(toFree.second);
    }
    callStack.pop();
    return retVal;
}

ActivationRecord Interpreter::prepareActivationRecord(ASTNode* node) {
    ActivationRecord ar;
    Procedure* func;
    string procedureName = node->data.stringVal;
    if (!callStack.empty() && callStack.top().nestedProc.find(procedureName) !=  callStack.top().nestedProc.end())
        func =  callStack.top().nestedProc[procedureName];
    else func = procedures[procedureName];
    ar.function = func;
    ASTNode* t = node->left;
    for (auto it = func->paramList; it != nullptr; it = it->left) {
        ar.env.insert(make_pair(it->data.stringVal, memStore.storeAtNextFree(expression(t))));
        say(it->data.stringVal + " added to AR.");
        if (t->left != nullptr)
            t = t->left;
    }
    ar.returnValue = makeRealObject(0.0);
    ar.staticLink = &callStack.top();
    return ar;
}

Object* Interpreter::procedureCall(ASTNode* node) {
    enter("[procedureCall] " + node->data.stringVal);
    if (!callStack.empty() && callStack.top().nestedProc.find(node->data.stringVal) !=  callStack.top().nestedProc.end()) {
        return runProcedure(node);
    }
    if (procedures.find(node->data.stringVal) != procedures.end()) {
        return runProcedure(node);
    }
    int addr = getAddress(node->data.stringVal);
    Object* obj = memStore.get(addr);
    if (addr > 0 && obj->type == AS_CLOSURE) {
        return runClosure(node, obj);
    } else {
        say("No such function: " + node->data.stringVal);
    }
    leave();
    return makeNilObject();
}

Object* Interpreter::lambdaExpr(ASTNode* node) {
    enter("[lambda_expr]"); leave();
    bool isClosure = (!callStack.empty());
    return makeClosureObject(makeLambda(node->right, node->left, isClosure ? callStack.top().env:Environment(), isClosure));
}

Object* Interpreter::listExpr(ASTNode* node) {
    enter("[list_expr]");
    ListHeader* list = makeListHeader();
    ASTNode* t = node->left;
    while (t != nullptr) {
        Object* obj = expression(t);
        say("push: " + toString(obj));
        push_back_list(list, obj);
        t = t->left;
    }
    leave();
    return makeListObject(list);
}