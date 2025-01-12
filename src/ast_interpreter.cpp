#include <cmath>
#include "ast_interpreter.hpp"
#include "regex/compiler.hpp"
#include "regex/nfa.hpp"
#include "regex/patternmatcher.hpp"
using namespace std;

ASTInterpreter::ASTInterpreter(bool loud) {
    traceEval = loud;
    recDepth = 0;
    gc = GC(loud);
}

//This is the entry point for evaluating owlscript programs.
Object ASTInterpreter::execAST(astnode* node) {
    bailout = false;
    recDepth = 0;
    Object m = exec(node);
    gc.run(cxt);
    return m;
}

Context& ASTInterpreter::getContext() {
    return cxt;
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

Object ASTInterpreter::eval(astnode* node) {
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

Object ASTInterpreter::evalStringOp(Symbol op, Object& lhn, Object& rhn) {
    if (op != TK_ADD) {
        cout<<"Error: unsupported operation on type 'string'"<<endl;
        return makeNilObject();
    }
    string concat = toString(lhn) + toString(rhn);
    return makeStringObject(concat);
}

Object ASTInterpreter::evalUnaryOp(astnode* node) {
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
        case TK_LOGIC_NOT: {
                say("unary not");
                if (m.type == AS_REAL)  m.realval = !(m.realval);
                else if (m.type == AS_INT)  m.intval = !(m.intval);
                else if (m.type == AS_BOOL) m.boolval = !(m.boolval);
            }
            leave();
            return m;
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

Object ASTInterpreter::getObjectByID(string id, int scope) {
    Object m;
    enter("[Get object by ID: " + id + "]");
    bool resolved_in_scoped = false;
    if (!cxt.scoped.empty() && scope > -1) { 
        Environment env = cxt.getAt(scope);
        if (env.find(id) != env.end()) {
            m = env[id];
            resolved_in_scoped = true;
            say("[Resolved " + id + " as " + toString(m) + " from scope level: " + to_string(scope)  +"]");
        }
    }
    if (!resolved_in_scoped) {
        if (cxt.globals.find(id) != cxt.globals.end()) {
            say("[Resolved " + id + " from global scope.]");
            m = cxt.globals[id];
        } else {
            cout<<"Unknown Identifier: "<<id<<endl;
            m = makeNilObject();
        }
    }
    leave();
    return m;
}

Object ASTInterpreter::getObjectByReference(astnode* node) {
    enter("Getting Object by reference");
    string id = getNameAndScopeFromNode(node).first;
    int scope = getNameAndScopeFromNode(node).second;
    say("Got: " + id + " at scope level " + to_string(scope));
    Object m = getObjectByID(id, scope);
    Object refd = getObjectByID(m.refVal->objectId, m.refVal->objectScope);
    return refd;
}


void ASTInterpreter::addToContext(string id, Object m, int scope) {
    if (!cxt.scoped.empty() && scope > -1) {
        cxt.putAt(id, m, scope);
    } else {
        cxt.globals[id] = m;
    }    
}

void saveFile(Object& m) {
    ofstream ofile(m.objval->fileObj->fname->str);
    if (ofile.is_open()) {
        ofile<<toString(m)<<flush;
        ofile.close();
    }
}

Object ASTInterpreter::performListAssignment(astnode* node, Object& m) {
    ListObj* list = typeOf(m) == AS_LIST ? getList(m):m.objval->fileObj->lines;
    Object subm = execExpression(node->child[0]->child[1]);
    int sub = atoi(toString(subm).c_str());
    if (sub > list->length || sub < 0) {
        cout<<"Error: subscript out of range"<<endl;
        leave();
        return m;
    }
    ListNode* it = list->head;
    for (int i = 0; i < sub; i++) {
        it = it->next;
    }
    it->info = execExpression(node->child[1]);
    if (typeOf(m) == AS_LIST) {
        m.objval->listObj = list;
    } else {
        m.objval->fileObj->lines = list;
        saveFile(m);
    }
    return m;
}

Object ASTInterpreter::performStructFieldAssignment(astnode* node, Object& m) {
    string vname = node->child[0]->child[1]->attributes.strval;
    StructObj* st = getStruct(m);
    if (st->blessed) {
        st->bindings[vname] = execExpression(node->child[1]);
    } else {
        cout<<"Structs must be instantiated before use."<<endl;
        leave();
        return makeNilObject();
    }
    m.objval->structObj = st;
    return m;
}

pair<string,int> ASTInterpreter::getNameAndScopeFromNode(astnode* node) {
    return make_pair(node->attributes.strval, node->attributes.depth);
}

Object ASTInterpreter::performSubscriptAssignment(astnode* node, string id, int scope) {
    Object m = getObjectByID(id, scope);
    if (typeOf(m) == AS_LIST || typeOf(m) == AS_FILE) {
        m = performListAssignment(node, m);
    } else if (typeOf(m) == AS_STRUCT) {
        m = performStructFieldAssignment(node, m);
    } else {
        cout<<"Object "<<id<<" type does not support subscript access: "<<toString(m)<<endl;
        return makeNilObject();
    }
    return m;
}

Object ASTInterpreter::performAssignment(astnode* node) {
    Object m;
    string id;
    int scope;
    enter("[assignment]");
    if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        id = getNameAndScopeFromNode(node->child[0]->child[0]).first;
        scope = getNameAndScopeFromNode(node->child[0]->child[0]).second;
        //cout<<"Looking for  '"<<id<<"' in "<<scope<<endl;
        m = performSubscriptAssignment(node, id, scope);
    } else {
        id = getNameAndScopeFromNode(node->child[0]).first;
        scope = getNameAndScopeFromNode(node->child[0]).second;
        m = execExpression(node->child[1]);
    }
    if (scope > -1) {
        if (!cxt.scoped.empty() && cxt.getAt(scope).find(id) == cxt.getAt(scope).end()) {
            cout<<"Undeclared Identifier: "<<id<<endl;
            return makeNilObject();
        } 
    } else {
        if (cxt.globals.find(id) == cxt.globals.end()) {
            cout<<"Undeclared Identifier: "<<id<<endl;
            return makeNilObject();
        } 
    }
    addToContext(id,  m, scope);
    leave();
    return m;
}

Object ASTInterpreter::performFunctionCall(astnode* node) {
    enter("[function call]");
    string id = getNameAndScopeFromNode(node).first;
    int scope = getNameAndScopeFromNode(node).second;
    Object lmbd = getObjectByID(id, scope);
    if (lmbd.type != AS_LAMBDA) {
        leave();
        cout<<"Error: No function '"<<id<<"' coult be found."<<endl;
        return makeNilObject();
    }
    Object m = executeFunction(getLambda(lmbd), node->child[1]);
    addToContext(id, lmbd, scope);
    leave();
    return m;
}

Object ASTInterpreter::performBlessExpression(astnode* node) {
    Object m;
    enter("[bless struct]");
    string id = getNameAndScopeFromNode(node->child[0]).first;
    int scope = getNameAndScopeFromNode(node->child[0]).second;
    Object master = getObjectByID(id, scope);
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

Object ASTInterpreter::performMakeReference(astnode* node) {
    Object m;
    enter("[make reference]");
    string id = getNameAndScopeFromNode(node->child[0]).first;
    int scope = getNameAndScopeFromNode(node->child[0]).second;
    m = makeReferenceObject(id, scope);
    return m;
}   

Object ASTInterpreter::executeFunction(LambdaObj* lambdaObj, astnode* args) {
    enter("[execute lambda]");
    Environment env;
    astnode* params = lambdaObj->params;
    VarList* freeVars = lambdaObj->freeVars;
    if (freeVars != nullptr) {
        //Add captured variables to local context
        for (VarList* it = freeVars; it != nullptr; it = it->next) {
            env[it->key] = it->value;
        }
    }
    astnode* itr = args;
    while (params != nullptr && itr != nullptr) {
        string vname;
        if (params->type == EXPR_NODE && params->exprType == REF_EXPR) {
            vname = params->child[0]->attributes.strval;
        } else {
            vname = params->attributes.strval;
        }
        string val = itr->attributes.strval;
        env[vname] = execExpression(itr);
       // cout<<"Assigning: "<<vname<<" value "<<env[vname]<<endl;
        params = params->next;
        itr = itr->next;
    }
    cxt.scoped.push(env);
    Object m = exec(lambdaObj->body);
    bailout = false;
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

Object ASTInterpreter::performCreateLambda(astnode* node) {
    enter("[create lambda]");
    LambdaObj* lm = makeLambdaObj(node->child[0], node->child[1]);
    if (!cxt.scoped.empty()) {
        for (auto frv : cxt.scoped.top()) {
            lm->freeVars = makeVarList(frv.first, frv.second, lm->freeVars);
        }
    }
    Object m = makeLambdaObject(lm);
    gc.add(m.objval);
    leave();
    return m;
}


Object ASTInterpreter::execCreateUnNamedList(astnode* node) {
    Object m;
    enter("[unnamed list]");
    ListObj* list = makeListObj();
    if (node->child[0] != nullptr && node->child[0]->type == EXPR_NODE && 
        (node->child[0]->exprType == RANGE_EXPR || node->child[0]->exprType == LISTCOMP_EXPR)) {
        if (node->child[0]->exprType == RANGE_EXPR) {
            m = execExpression(node->child[0]);
        } else if (node->child[0]->exprType == LISTCOMP_EXPR) {
            m = execExpression(node->child[0]);
        }
    } else {
        for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
            Object m = execExpression(it);
            appendToList(list, m);
        }
        m = makeListObject(list);
    }
    gc.add(m.objval);
    leave();
    return m;
}

void ASTInterpreter::resolveObjForExpression(astnode* node, string& id, Object& m) {
    if (node->child[0]->attributes.symbol == TK_ID) {
        id = getNameAndScopeFromNode(node->child[0]).first;
        int scope = getNameAndScopeFromNode(node->child[0]).second;
        m = getObjectByID(id, scope);
    } else if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        m = execCreateUnNamedList(node->child[0]);
    }  else if (node->child[0]->type == EXPR_NODE) {
        m = execExpression(node->child[0]);
    } else {
        m = makeNilObject();
    }
}

Object ASTInterpreter::execAppendList(astnode* node) {
    enter("append to list");
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    Object t = execExpression(node->child[1]);
    appendToList(getList(m), t);
    addToContext(id, m, node->child[0]->attributes.depth);
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
    Object t = execExpression(node->child[1]);
    pushToList(getList(m), t);
    addToContext(id, m, node->child[0]->attributes.depth);
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
    addToContext(id, m, node->child[0]->attributes.depth);
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
    addToContext(id, m, node->child[0]->attributes.depth);
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

Object ASTInterpreter::execSubscriptExpression(astnode* node) {
    string id;
    Object m;
    resolveObjForExpression(node, id, m);
    if (typeOf(m) == AS_LIST || typeOf(m) == AS_FILE) {
        Object subm = execExpression(node->child[1]);
        ListObj* list = typeOf(m) == AS_LIST ? getList(m):m.objval->fileObj->lines;
        int subscript = atoi(toString(subm).c_str());
        if (subscript > list->length || list->length == 0) {
            cout<<"Error: Subscript out of range."<<endl;
            return makeNilObject();
        }
        ListNode* it = list->head;
        for (int i = 0; i < subscript; i++) {
            it = it->next;
        }
        m = it->info;
    } else if (typeOf(m) == AS_STRUCT) {
        StructObj* sobj = getStruct(m);
        string vname = node->child[1]->attributes.strval;
        if (sobj->bindings.find(vname) == sobj->bindings.end()) {
            cout<<"Struct '"<<id<<"' has no such property '"<<vname<<"'."<<endl;
            return makeNilObject();
        }
        m = sobj->bindings[vname];
    } else if (typeOf(m) == AS_STRING) {
        Object subm = execExpression(node->child[1]);
        int subscript = atoi(toString(subm).c_str());
        StringObj* strobj = getString(m);
        if (subscript >= 0 && subscript < strobj->length) {
            string s;
            s.push_back(strobj->str[subscript]);
            return makeStringObject(s); 
        } else {
            cout<<"index "<<subscript<<" is out of range for string of length "<<strobj->length<<endl;
            return makeNilObject();
        }
    }
    return m;
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
            cmpResult = executeFunction(compFunc, args).boolval;
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
    Object lm = execExpression(node->child[1]);
    LambdaObj* lmbd = nullptr;
    if (node->child[1] != nullptr) {
        Object lm = execExpression(node->child[1]);
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
        addToContext(id, m, node->child[0]->attributes.depth);
    
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
    Object lm = execExpression(node->child[1]);
    LambdaObj* lmbd = getLambda(lm);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(m)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        Object r = executeFunction(lmbd, t);
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
    Object lm = execExpression(node->child[1]);
    LambdaObj* lmbd = getLambda(lm);
    ListObj* result = makeListObj();
    for (ListNode* it = getList(m)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        Object r = executeFunction(lmbd, t);
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

Object ASTInterpreter::execRegularExpr(astnode* node) {
    enter("Regular Expression");
    Object m;
    Object toCheck = execExpression(node->child[0]);
    Object regExpr = execExpression(node->child[1]);
    NFACompiler compiler(traceEval);
    NFA nfa = compiler.compile(toString(regExpr));
    gc.add(toCheck.objval);
    gc.add(regExpr.objval);
    RegExPatternMatcher pm(nfa, traceEval);
    m = makeBoolObject(pm.match(toString(toCheck)));
    leave();
    return m;
}

Object ASTInterpreter::execListExpression(astnode* node) {
    enter("List expression");
    Object m;
    if (node->exprType == SUBSCRIPT_EXPR) {
        m = execSubscriptExpression(node);
    } else {
        switch (node->attributes.symbol) {
            case TK_LSQUARE: 
                m = execCreateUnNamedList(node);
                break;
            case TK_SHIFT:
            case TK_APPEND:
                m = execAppendList(node);
                break;
            case TK_PUSH:
                m = execPushList(node);
                break;
            case TK_POP:
                m = execPopList(node);
                break;
            case TK_UNSHIFT:
                m = execPopBackList(node);
                break;
            case TK_LENGTH:
                m = execLength(node);
                break;
            case TK_SORT:
                m = execSortList(node);
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
            case TK_FILTER:
                m = execFilter(node);
                break;
            case TK_EMPTY:
                m = execIsEmptyList(node);
                break;
            default:
                break;
        }
    }
    leave();
    return m;
}

Object ASTInterpreter::performWhileStatement(astnode* node) {
    enter("while statement");
    Object m;
    while (execExpression(node->child[0]).boolval) {
        exec(node->child[1]);
    }
    leave();
    return m;
}

Object ASTInterpreter::performForStatement(astnode* node) {
    enter("for statement");
    Object m;
    astnode* precon = node->child[0];
    astnode* testcon = node->child[0]->next;
    astnode* postExpr = node->child[0]->next->next;
    astnode* loopBody = node->child[1];
    m = execExpression(precon);
    while (execExpression(testcon).boolval) {
        exec(loopBody);
        m = execExpression(postExpr);
    }
    leave();
    return m;
}

Object ASTInterpreter::performDefStatement(astnode* node) {
    enter("def statement");
    string id = getNameAndScopeFromNode(node).first;
    int scope = getNameAndScopeFromNode(node).second;
    Object m = performCreateLambda(node);
    gc.add(m.objval);
    addToContext(id, m, scope);
    leave();
    return m;
}

Object ASTInterpreter::performIfStatement(astnode* node) {
    enter("If statement");
    Object m;
    bool test = execExpression(node->child[0]).boolval;
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
        string vname = it->child[0]->attributes.strval;
        sobj->bindings[vname] = makeNilObject();
    }
    sobj->blessed = false;
    cxt.globals[id] = makeStructObject(sobj);
    gc.add(cxt.globals[id].objval);
    return makeNilObject();
}
 
Object ASTInterpreter::performBlockStatement(astnode* node) {
    cxt.scoped.push(Environment());
    exec(node->child[0]);
    cxt.scoped.pop();
    if (cxt.scoped.size() <= 1)
        gc.run(cxt);
    return makeNilObject();
}

Object ASTInterpreter::performLetStatement(astnode* node) {
    Object m;
    string id = node->child[0]->child[0]->attributes.strval;
    if (!cxt.scoped.empty()) {
        Environment env = cxt.scoped.top();
        if (env.find(id) != env.end()) {
            cout<<"Error: '"<<id<<"' has already been declared in this scope."<<endl;
            return makeNilObject();
        } else {
             cxt.scoped.top()[id] = makeNilObject();
             m = exec(node->child[0]);
        }
    } else {
        if (cxt.globals.find(id) != cxt.globals.end()) {
            cout<<"Error: '"<<id<<"' has already been declared in this scope."<<endl;
            return makeNilObject();
        } else {
            cxt.globals[id] = makeNilObject();
            m = execExpression(node->child[0]);
        }
    }
    return m;
}

Object ASTInterpreter::execStatement(astnode* node) {
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
        case BLOCK_STMT:
            m = performBlockStatement(node);
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
            m = performLetStatement(node);
            break;
        case RETURN_STMT: 
            m = execExpression(node->child[0]);
            bailout = true;
            break;
        case STRUCT_STMT:
            m = performStructDefStatement(node);
            break;
        case REF_STMT:
            m = performMakeReference(node);
            break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::performListComprehension(astnode* node) {
    Object il = execExpression(node->child[0]);
    if (typeOf(il) != AS_LIST)
        return makeNilObject();
    Object lm = execExpression(node->child[1]);
    Object prd = execExpression(node->child[2]);
    LambdaObj* lmbd = lm.type == AS_LAMBDA ? getLambda(lm):nullptr;
    LambdaObj* pred = prd.type == AS_LAMBDA ? getLambda(prd):nullptr;
    ListObj* list = makeListObj();
    for (ListNode* it = getList(il)->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        if (pred != nullptr) {
            if (executeFunction(pred, t).boolval)
                appendToList(list,  executeFunction(lmbd, t));
        } else {
            appendToList(list, executeFunction(lmbd, t));
        }
    }
    return makeListObject(list);
}

Object ASTInterpreter::performRangeExpression(astnode* node) {
    Object from = execExpression(node->child[0]);
    Object to = execExpression(node->child[1]);
    ListObj* list = makeListObj();
    int a = getAsReal(from);
    int b = getAsReal(to);
    if ( a < b)  {
        for (int i = a; i <= b; i++) {
            appendToList(list, makeIntObject(i));
        }
    } else {
        for (int i = a; i >= b; i--) {
            appendToList(list, makeIntObject(i));
        }
    }
    return makeListObject(list);
}

Object ASTInterpreter::performFileOpenExpression(astnode* node) {
    Object m;
    Object fn = execExpression(node);
    FileObj* fobj = makeFileObj(fn.objval->stringObj);
    fobj->lines = makeListObj();
    ifstream ifile(toString(fn));
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
Object ASTInterpreter::performFileCloseExpression(astnode* node) {
    return makeNilObject();
}
Object ASTInterpreter::execExpression(astnode* node) {
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
            m = getObjectByID(node->attributes.strval, node->attributes.depth);
            break;
        case REG_EXPR:
            m = execRegularExpr(node);
            break;
        case REF_EXPR:
            m = getObjectByReference(node);
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
            m = performAssignment(node);
            break;
        case FUNC_EXPR:
            m = performFunctionCall(node);
            break;
        case LAMBDA_EXPR:
            m = performCreateLambda(node);
            break;
        case RANGE_EXPR:
            m = performRangeExpression(node);
            break;
        case LIST_EXPR:
        case SUBSCRIPT_EXPR:
            m = execListExpression(node);
            break;
        case BLESS_EXPR:
            m = performBlessExpression(node);
            break;
        case FILE_EXPR: {
            switch (node->attributes.symbol) {
                case TK_FOPEN:  m = performFileOpenExpression(node->child[0]); break;
                case TK_FCLOSE: m = performFileCloseExpression(node->child[0]); break;
                default:
                    break;
            };
        } break;
        case LISTCOMP_EXPR:
            m = performListComprehension(node);
            break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::exec(astnode* node) {
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
        if (node->next != nullptr && !bailout)
            m = exec(node->next);
    }
    return m;
}

void ASTInterpreter::say(string s) {
    if (!traceEval)
        return;
    for (int i = 0; i < recDepth; i++)
        cout<<" ";
    cout<<s<<endl;
}

void ASTInterpreter::enter(string s) {
    ++recDepth;
    if (traceEval)
        say(s);
}

void ASTInterpreter::leave() {
    --recDepth;
}