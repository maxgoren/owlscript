#include "twvm.hpp"

Object TWVM::resolveFunction(astnode* node) {
    Object m;
    if (isExprType(node->child[0], LAMBDA_EXPR)) {
        evalExpr(node->child[0]);
        m = pop();
    } else if (node->child[0]->token.strval == "_rc") {
        m = cxt.getCallStack()->bindings["_rc"];
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

bool TWVM::checkFunctionArity(astnode* args, astnode* params) {
    astnode* x = args, *y = params;
    while (x != nullptr && y != nullptr) {
        x = x->next;
        y = y->next;
    }
    return (x == nullptr && y == nullptr);
}

bool TWVM::evalFunctionArguments(astnode* args, astnode* params, ActivationRecord*& env) {
    if (!checkFunctionArity(args, params)) {
        cout<<"Error: Mismatched arguments"<<endl;
        return false;
    }
    while (params != nullptr && args != nullptr) {
        if (isExprType(params, REF_EXPR)) {
            cout<<"Bound arg '"<<params->child[0]->token.strval<<"' as Reference to "<<args->token.strval<<" at scope depth "<<args->token.depth<<endl;
            env->bindings.insert(make_pair(params->child[0]->token.strval, makeReference(args->token.strval, args->token.depth)));
        } else {
            evalExpr(args);
            env->bindings.insert(make_pair(params->token.strval, pop())); 
        }
        params = params->next;
        args = args->next;
    }
    return true;
}

void TWVM::funcExpression(Function* func, astnode* params) {
    ActivationRecord* env = new ActivationRecord(func->closure, cxt.getCallStack());
    if (evalFunctionArguments(params, func->params, env)) {
        cxt.openScope(env);
        cxt.insert("_rc", cxt.getAlloc().makeFunction(func));
        exec(func->body);
        bailout = false;
        cxt.closeScope();
    } else {
        delete env;
    }
}

void TWVM::regularExpression(astnode* node) {
    evalExpr(node->child[0]);
    string text = *getString(pop());
    evalExpr(node->child[1]);
    string pattern = *getString(pop());
    //StringBuffer sb; sb.init(text);
    //push(makeBool(MatchRE()(sb, pattern, loud)));
    push(makeBool(matchDFA(const_cast<char*>(pattern.data()), const_cast<char*>(text.data()))));
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
    int i = 0;
    if (node->child[1] != nullptr) {
        for (auto x = node->child[1]; x != nullptr; x = x->next) {
            evalExpr(x);
            nextInstance->fields[st->constructorOrder[i++]] = pop();
        }
    }
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


void TWVM::idExpr(astnode* node) {
    Object m = cxt.get(node->token.strval, node->token.depth);
    if (typeOf(m) == AS_REF) {
        cout<<"Oh snap, a reference! "<<node->token.strval<<" is pointing to something else: "<<m.data.reference->identifier<<" at "<<m.data.reference->scopelevel<<endl;
        Object t = cxt.get(m.data.reference->identifier, m.data.reference->scopelevel);
        cout<<"Found: "<<toString(t)<<endl;
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
    astnode* x = node;
    while (!isExprType(x, ID_EXPR)) x = x->child[0];
    if (!cxt.exists(x->token.strval, x->token.depth)) {
        cout<<"Unknown Identifier: "<<x->token.strval<<endl;
        return;
    }
    if (isExprType(node->child[0], ID_EXPR)) {
        evalExpr(node->child[1]);
        switch (node->token.symbol) {
            case TK_ASSIGN: {
                Object t = cxt.get(node->child[0]->token.strval, node->child[0]->token.depth);
                if (t.type == AS_REF) {
                    cout<<"Oh snap, implicit deref to "<<t.data.reference->identifier<<" at "<<t.data.reference->scopelevel<<endl;
                    cxt.get(t.data.reference->identifier, t.data.reference->scopelevel+1);
                    Object r = pop();
                    cout<<"Found: "<<toString(t)<<", assigning "<<toString(r)<<endl;
                    cxt.put(t.data.reference->identifier, t.data.reference->scopelevel, r);
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