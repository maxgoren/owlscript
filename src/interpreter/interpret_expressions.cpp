#include "ast_interpreter.hpp"
#include "regex/re_compiler.hpp"
#include "regex/nfa.hpp"
#include "regex/patternmatcher.hpp"
#include "../ast_builder.hpp"
#include <cmath>

Object ASTInterpreter::eval(astnode* node) {
    if (node == nullptr)
        return makeNilObject();
    Object lhn = evalExpression(node->child[0]);
    Object rhn = evalExpression(node->child[1]);
    switch (node->exprType) {
        case RELOP_EXPR:  return evalRelop(node, lhn, rhn);
        case BINARYOP_EXPR: return evalBinOp(node, lhn, rhn);
        default:
            break;
    }
    return makeIntObject(0);
}

Object ASTInterpreter::evalRelop(astnode* node, Object& lhn, Object& rhn) {
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
            case TK_LOGIC_AND: return makeBoolObject(lhs && rhs);
            case TK_LOGIC_OR:  return makeBoolObject(lhs || rhs);
            default:
                break;
        }
    }
    string lst = toString(lhn);
    string rst = toString(rhn);
    switch (node->attributes.symbol) {
        case TK_LT: return makeBoolObject(lst < rst);
        case TK_GT: return makeBoolObject(lst > rst);
        case TK_GTE: return makeBoolObject(lst >= rst);
        case TK_LTE: return makeBoolObject(lst <= rst);
        case TK_EQU: return makeBoolObject(lst == rst);
        case TK_NOTEQU: return makeBoolObject(lst != rst);
        default:
            break;
    }
    return makeBoolObject(false);
}

Object ASTInterpreter::evalBinOp(astnode* node, Object& lhn, Object& rhn) {
    enter("eval binary op: " + node->attributes.strval);
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
            case TK_MOD: {
                if (rhs == 0) {
                    cout<<"Error: Divide by zero."<<endl;
                    return makeNilObject();
                }
                return makeIntObject((int)lhs % (int)rhs);
            }
            case TK_POW: return makeRealObject(pow(lhs, rhs));
            default: 
                break;
        }
    } else if (typeOf(lhn) == AS_STRING || typeOf(rhn) == AS_STRING) {
        return evalStringOp(node->attributes.symbol, lhn, rhn);
    } else if (typeOf(lhn) == AS_LIST && typeOf(rhn) == AS_LIST) {
        ListObj* nl = makeListObj();
        for (ListNode* it = getList(lhn)->head; it != nullptr; it = it->next) {
            nl = appendToList(nl, it->info);
        }
        for (ListNode* it = getList(rhn)->head; it != nullptr; it = it->next) {
            nl = appendToList(nl, it->info);
        }
        return makeListObject(nl);
    }
    return makeIntObject(0);
}


/// @brief string concat
/// @param op 
/// @param lhn 
/// @param rhn 
/// @return 
Object ASTInterpreter::evalStringOp(Symbol op, Object& lhn, Object& rhn) {
    if (op != TK_ADD) {
        cout<<"Error: unsupported operation on type 'string'"<<endl;
        return makeNilObject();
    }
    string concat = toString(lhn) + toString(rhn);
    return makeStringObject(concat);
}


/// @brief unary minus and logical not
/// @param node 
/// @return 
Object ASTInterpreter::evalUnaryOp(astnode* node) {
    enter("eval unary op");
    Object m = evalExpression(node->child[0]);
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
        case TK_LOGIC_NOT: {
                say("unary not");
                if (m.type == AS_REAL)  m.realval = !(m.realval);
                else if (m.type == AS_INT)  m.intval = !(m.intval);
                else if (m.type == AS_BOOL) m.boolval = !(m.boolval);
            }
            leave();
            return m;
        case TK_POST_INC: {
            if (m.type == AS_REAL) { 
                m.realval += 1; 
            } else if (m.type == AS_INT) {
                m.intval += 1; 
            }
            string id = getNameAndScopeFromNode(node->child[0]).name;
            int scope = getNameAndScopeFromNode(node->child[0]).scope;
            updateContext(id, m, scope);
        } return m;
        case TK_POST_DEC: {
            if (m.type == AS_REAL) { 
                m.realval -= 1; 
            } else if (m.type == AS_INT) {
                m.intval -= 1; 
            }
            string id = getNameAndScopeFromNode(node->child[0]).name;
            int scope = getNameAndScopeFromNode(node->child[0]).scope;
            updateContext(id, m, scope);
        } return m;
        case TK_SQRT: {
                if (m.type == AS_REAL) m.realval = sqrt(m.realval);
                else if (m.type == AS_INT) m = makeRealObject(sqrt(m.intval));
            }
            leave();
            return m;
        default:
            break;
    }
    leave();
    return m;
}


/// @brief match regular expression
/// @param node 
/// @return 
Object ASTInterpreter::evalRegularExpr(astnode* node) {
    enter("Regular Expression");
    Object m;
    Object toCheck = evalExpression(node->child[0]);
    Object regExpr = evalExpression(node->child[1]);
    NFACompiler compiler(traceEval);
    NFA nfa = compiler.compile(toString(regExpr));
    gc.add(toCheck.objval);
    gc.add(regExpr.objval);
    RegExPatternMatcher pm(nfa, traceEval);
    m = makeBoolObject(pm.match(toString(toCheck)));
    leave();
    return m;
}


/// @brief user supplied eval()
/// @param node 
/// @return 
Object ASTInterpreter::evalMetaExpression(astnode* node) {
    Object m = evalExpression(node->child[0]);
    ASTBuilder astbuilder;
    astnode* meta = astbuilder.build(toString(m));
    Object r = exec(meta);
    return r;
}


Object ASTInterpreter::getConstValue(astnode* node) {
    Object m;
    enter("[get const value: " + node->attributes.strval +"]");
    switch (node->attributes.symbol) {
        case TK_NUM:
            m = makeIntObject(atoi(node->attributes.strval.c_str()));
            break;
        case TK_REALNUM:
            m = makeRealObject(stod(node->attributes.strval));
            break;
        case TK_TRUE:
        case TK_FALSE:
            m = makeBoolObject(node->attributes.strval == "true" ? true:false);
            break;
        case TK_ID:
        case TK_STRING:
            m = makeStringObject(node->attributes.strval);
            gc.add(m.objval);
            break;
        case TK_NIL:
        default:
            m = makeNilObject();
            break;
    }
    leave();
    return m;
}

/// @brief set up environment for function call
/// @param lambdaObj 
/// @param args 
/// @param env 
void ASTInterpreter::prepareEnvForFunctionCall(LambdaObj* lambdaObj, astnode* args, Environment& env) {
    astnode* params = lambdaObj->params;
    VarList* freeVars = lambdaObj->freeVars;
    for (VarList* it = freeVars; it != nullptr; it = it->next) {
        env[it->key] = it->value;
    }
    astnode* itr = args;
    while (params != nullptr && itr != nullptr) {
        string vname = isStmtType(params,REF_STMT) ? getAttributes(params->child[0]).strval:getAttributes(params).strval;
        env[vname] = evalExpression(itr);
        //cout<<"Assigning: "<<vname<<" value "<<env[vname]<<endl;
        params = params->next;
        itr = itr->next;
    }
}

/// @brief restore environment after function call
/// @param lobj 
/// @param args 
void ASTInterpreter::cleanUpAfterFunctionCall(LambdaObj* lobj, astnode* args) {
    astnode* params = lobj->params;
    VarList* freeVars = lobj->freeVars;
    bailout = false;
    //update any closed-over variables before exiting.
    for (VarList* it = freeVars; it != nullptr; it = it->next) {
        it->value = cxt.scoped.top()[it->key];
    }
    lobj->freeVars = freeVars;
    astnode* itr = args;
    //listen. I _know_ this is technically pass by value-result
    //pass by reference is unexpectedly hard to implement with 
    //the current environment schemea
    while (params != nullptr && itr != nullptr) {
        if (isStmtType(params, REF_STMT)) {
            Object m = cxt.scoped.top()[params->child[0]->attributes.strval];
            auto [id, scope]  = getNameAndScopeFromNode(itr);
            updateContext(id, m, scope > -1 ? scope+1:scope);
        }
        params = params->next;
        itr = itr->next;
    }
}


/// @brief Evaluate user defined function
/// @param lambdaObj 
/// @param args 
/// @return 
Object ASTInterpreter::evalFunctionExpr(LambdaObj* lambdaObj, astnode* args) {
    enter("[execute lambda]");
    Environment env;
    prepareEnvForFunctionCall(lambdaObj, args, env);
    cxt.openScope(env);
    Object m = exec(lambdaObj->body);
    cleanUpAfterFunctionCall(lambdaObj, args);
    cxt.closeScope();
    leave();
    return m;
}

/// @brief resolve a subscript
/// @param node 
/// @return Object
Object ASTInterpreter::evalSubscriptExpression(astnode* node) {
    string id;
    Object m;
    //cout<<"Subscript expression"<<endl;
    if (isExprType(node->child[0], SUBSCRIPT_EXPR) || isExprType(node->child[0], OBJECT_DOT_EXPR)) {
        m = evalSubscriptExpression(node->child[0]);
    } else {
        resolveObjForExpression(node, id, m);
    }
    if (typeOf(m) == AS_LIST || typeOf(m) == AS_FILE) {
        m = performListAccess(node, m);
    } else if (typeOf(m) == AS_STRUCT) {
        m = performStructFieldAccess(node, id, m);
    } else if (typeOf(m) == AS_STRING) {
        m = performSubscriptStringAccess(node, m);
    }
    return m;
}

/// @brief instantiate an object
/// @param node 
/// @return Object 
Object ASTInterpreter::evalBlessExpression(astnode* node) {
    Object m;
    enter("[bless struct]");
    NameAndScope resolved = getNameAndScopeFromNode(node->child[0]);
    Object master = getObjectByID(resolved.name, resolved.scope);
    StructObj* og = getStruct(master);
    StructObj* ninst = makeStructObj();
    ninst->blessed = true;
    for (auto m : og->bindings) {
        ninst->bindings[m.first] = m.second;
    }
    m = makeStructObject(ninst);
    gc.add(m.objval);
    return m;
}