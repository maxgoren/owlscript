#include <cmath>
#include "ast_interpreter.hpp"
#include "../ast_builder.hpp"
using namespace std;

void saveFile(Object& m) {
    ofstream ofile(m.objval->fileObj->fname->str);
    if (ofile.is_open()) {
        ofile<<toString(m)<<flush;
        ofile.close();
    }
}

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

Object ASTInterpreter::exec(astnode* node) {
    Object m;
    if (node != nullptr) {
        switch (node->type) {
            case STMT_NODE: 
                m = execStatement(node);
                break;
            case EXPR_NODE:
                m = evalExpression(node);
                break;
            default:
                break;
        }
        if (node->next != nullptr && !bailout)
            m = exec(node->next);
    }
    return m;
}

Object ASTInterpreter::execStatement(astnode* node) {
    enter("[statement]");
    Object m;
    switch (node->stmtType) {
        case PRINT_STMT: m = performPrintStatement(node); break;
        case WHILE_STMT: m = performWhileStatement(node); break;
        case IF_STMT: m = performIfStatement(node); break;
        case BLOCK_STMT: m = performBlockStatement(node); break;
        case DEF_STMT: m = performDefStatement(node); break;
        case FOR_STMT: m = performForStatement(node); break;
        case EXPR_STMT: m = evalExpression(node->child[0]); break;
        case LET_STMT: m = performLetStatement(node); break;
        case RETURN_STMT: {
             m = evalExpression(node->child[0]);
            bailout = true;
        } break;
        case STRUCT_STMT: m = performStructDefStatement(node); break;
        case REF_STMT: m = performMakeReference(node); break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::evalExpression(astnode* node) {
    if (node == nullptr) 
        return makeIntObject(0);
    enter("[expression]");
    Object m;
    //printNode(node);
    switch (node->exprType) {
        case CONST_EXPR: m = getConstValue(node);   break;
        case ID_EXPR: m = getObjectByID(node->attributes.strval, node->attributes.depth); break;
        case REG_EXPR: m = evalRegularExpr(node); break;
        case REF_EXPR: m = getObjectByReference(node); break;
        case ASSIGN_EXPR: m = performAssignment(node); break;
        case FUNC_EXPR: m = performFunctionCall(node); break;
        case LAMBDA_EXPR: m = performCreateLambda(node); break;
        case RANGE_EXPR: m = evalRangeExpression(node); break;
        case UNARYOP_EXPR: m = evalUnaryOp(node); break;
        case BINARYOP_EXPR:
        case RELOP_EXPR: m = eval(node); break;
        case LIST_EXPR:
        case SUBSCRIPT_EXPR: m = evalListExpression(node); break;
        case LISTCOMP_EXPR: m = performListComprehension(node); break;
        case BLESS_EXPR: m = evalBlessExpression(node); break;
        case FILE_EXPR:  m = performFileOpenExpression(node->child[0]); break;
        case META_EXPR: m = performMetaExpression(node); break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::evalListExpression(astnode* node) {
    enter("List expression");
    Object m;
    if (node->exprType == SUBSCRIPT_EXPR) {
        m = evalSubscriptExpression(node);
    } else {
        switch (node->attributes.symbol) {
            case TK_LSQUARE:  m = performCreateUnNamedList(node); break;
            case TK_SHIFT:
            case TK_APPEND: m = execAppendList(node); break;
            case TK_PUSH: m = execPushList(node); break;
            case TK_POP: m = execPopList(node); break;
            case TK_UNSHIFT: m = execPopBackList(node); break;
            case TK_LENGTH: m = execLength(node); break;
            case TK_SORT: m = execSortList(node); break;
            case TK_FIRST: m = execFirst(node);  break;
            case TK_REST: m = execRest(node); break;
            case TK_MAP: m = execMap(node); break;
            case TK_FILTER: m = execFilter(node); break;
            case TK_EMPTY: m = execIsEmptyList(node); break;
            default:
                break;
        }
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

pair<string,int> ASTInterpreter::getNameAndScopeFromNode(astnode* node) {
    return make_pair(node->attributes.strval, node->attributes.depth);
}

Context& ASTInterpreter::getContext() {
    return cxt;
}

void ASTInterpreter::updateContext(string id, Object m, int scope) {
    if (!cxt.scoped.empty() && scope > -1) {
        cxt.putAt(id, m, scope);
    } else {
        cxt.globals[id] = m;
    }    
}

Object ASTInterpreter::declareInContext(astnode* t, Environment& env, string id) {
    Object m;
    if (env.find(id) != env.end()) {
        cout<<"Error: '"<<id<<"' has already been declared in this scope."<<endl;
        return makeNilObject();
    } else {
        env[id] = makeNilObject();
        m = exec(t);
    }
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
    Object m = evalFunctionExpr(getLambda(lmbd), node->child[1]);
    updateContext(id, lmbd, scope);
    leave();
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

void ASTInterpreter::resolveObjForExpression(astnode* node, string& id, Object& m) {
    if (getAttributes(node->child[0]).symbol == TK_ID) {
        id = getNameAndScopeFromNode(node->child[0]).first;
        int scope = getNameAndScopeFromNode(node->child[0]).second;
        m = getObjectByID(id, scope);
    } else if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        m = performCreateUnNamedList(node->child[0]);
    }  else if (isExprNode(node->child[0])) {
        m = evalExpression(node->child[0]);
    } else {
        m = makeNilObject();
    }
}

Object ASTInterpreter::performAssignment(astnode* node) {
    Object m;
    string id;
    int scope;
    enter("[assignment]");
    if (isExprType(node->child[0], SUBSCRIPT_EXPR)) {
        id = getNameAndScopeFromNode(node->child[0]->child[0]).first;
        scope = getNameAndScopeFromNode(node->child[0]->child[0]).second;
        m = performSubscriptAssignment(node->child[0], node->child[1], id, scope);
    } else {
        id = getNameAndScopeFromNode(node->child[0]).first;
        scope = getNameAndScopeFromNode(node->child[0]).second;
        m = evalExpression(node->child[1]);
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
        updateContext(id,  m, scope);
    }
    leave();
    return m;
}

Object ASTInterpreter::performSubscriptAssignment(astnode* node, astnode* expr, string& id, int& scope) {
    Object m;
    bool unomas = false;
    if (getAttributes(node->child[0]).symbol == TK_LSQUARE) {
        m = evalSubscriptExpression(node->child[0]);
    } else {
        id = getNameAndScopeFromNode(node->child[0]).first;
        scope = getNameAndScopeFromNode(node->child[0]).second;
        m = getObjectByID(id, scope);
    }
    if (typeOf(m) == AS_LIST || typeOf(m) == AS_FILE) {
        m = performListAssignment(node, expr, m);
    } else if (typeOf(m) == AS_STRUCT) {
        m = performStructFieldAssignment(node, expr, m);
    } else {
        cout<<"Object "<<id<<" type does not support subscript access: "<<toString(m)<<endl;
        return makeNilObject();
    }
    return m;
}

Object ASTInterpreter::performListAccess(astnode* node, Object m) {
    //cout<<"subscript list access"<<endl;
    Object subm = evalExpression(node->child[1]);
    ListObj* list = getList(m);
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

 Object ASTInterpreter::performListAssignment(astnode* node, astnode* expr, Object& m) {
    ListObj* list = getList(m);
    Object subm = evalExpression(node->child[1]);
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
    it->info = evalExpression(expr);
    if (typeOf(m) == AS_LIST) {
        m.objval->listObj = list;
    } else {
        m.objval->fileObj->lines = list;
        saveFile(m);
    }
    return m;
}

Object ASTInterpreter::performStructFieldAccess(astnode* node, string id, Object m) {
        StructObj* sobj = getStruct(m);
        string vname = getAttributes(node->child[1]).strval;
        if (sobj->bindings.find(vname) == sobj->bindings.end()) {
            cout<<"Struct '"<<id<<"' has no such property '"<<vname<<"'."<<endl;
            return makeNilObject();
        }
        return sobj->bindings[vname];
 }

 Object ASTInterpreter::performStructFieldAssignment(astnode* node, astnode* expr, Object& m) {
    string vname = getAttributes(node->child[1]).strval;
    StructObj* st = getStruct(m);
    if (st->blessed) {
        st->bindings[vname] = evalExpression(expr);
    } else {
        cout<<"Structs must be instantiated before use."<<endl;
        leave();
        return makeNilObject();
    }
    m.objval->structObj = st;
    return m;
}

 Object ASTInterpreter::performSubscriptStringAccess(astnode* node, Object m) {
    Object subm = evalExpression(node->child[1]);
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
    return m;
 }

Object ASTInterpreter::performMetaExpression(astnode* node) {
    Object m;
    switch (node->attributes.symbol) {
        case TK_EVAL: m = evalMetaExpression(node); break;
        case TK_TYPEOF: m = makeStringObject(getTypeName(exec(node->child[0]))); break;
        default: 
            break;
    };
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