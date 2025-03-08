#include "ast_interpreter.hpp"

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
            return env[id];
        }
    }
    if (!resolved_in_scoped) {
        if (cxt.globals.find(id) != cxt.globals.end()) {
            say("[Resolved " + id + " from global scope.]");
            m = cxt.globals[id];
            return cxt.globals[id];
        } else {
            cout<<"Unknown Identifier: "<<id<<endl;
            m = makeNilObject();
        }
    }
    leave();
    return m;
}

Object ASTInterpreter::declareInContext(astnode* t, Environment& env, string id) {
    Object m;
    if (env.find(id) != env.end()) {
        cout<<"Error: '"<<id<<"' has already been declared in this scope."<<endl;
        return makeNilObject();
    } else {
        env[id] = makeNilObject();
        m = exec(t);
        env[id] = m;
    }
    return m;
}

void ASTInterpreter::updateContext(string id, Object m, int scope) {
    if (!cxt.scoped.empty() && scope > -1) {
        cxt.putAt(id, m, scope);
    } else {
        cxt.globals[id] = m;
    }    
}

NameAndScope ASTInterpreter::getNameAndScopeFromNode(astnode* node) {
    return NameAndScope(node->attributes.strval, node->attributes.depth);
}

void ASTInterpreter::resolveObjForExpression(astnode* node, string& id, Object& m) {
    if (getAttributes(node->child[0]).symbol == TK_ID) {
        NameAndScope resolved = getNameAndScopeFromNode(node->child[0]);
        int scope = resolved.scope;
        id = resolved.name;
        m = getObjectByID(id, scope);
    } else if (node->child[0]->attributes.symbol == TK_LSQUARE) {
        m = performCreateUnNamedList(node->child[0]);
    }  else if (isExprNode(node->child[0])) {
        m = evalExpression(node->child[0]);
    } else {
        m = makeNilObject();
    }
}

Object ASTInterpreter::evalAssignmentExpression(astnode* node) {
    Object m;
    string id;
    int scope;
    NameAndScope resolved;
    enter("[assignment]");
    if (isExprType(node->child[0], SUBSCRIPT_EXPR) || isExprType(node->child[0], OBJECT_DOT_EXPR)) {
        resolved = getNameAndScopeFromNode(node->child[0]->child[0]);
        id = resolved.name;
        scope = resolved.scope;
        m = performSubscriptAssignment(node->child[0], node->child[1], id, scope);
    } else {
        resolved = getNameAndScopeFromNode(node->child[0]);
        id = resolved.name;
        scope = resolved.scope;
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
    }
    updateContext(id,  m, scope);
    leave();
    return m;
}

Object ASTInterpreter::performSubscriptAssignment(astnode* node, astnode* expr, string& id, int& scope) {
    Object m;
    bool unomas = false;
    if (isExprType(node->child[0], SUBSCRIPT_EXPR) || isExprType(node->child[0], OBJECT_DOT_EXPR)) {
        m = evalSubscriptExpression(node->child[0]);
    } else {
        id = getNameAndScopeFromNode(node->child[0]).name;
        scope = getNameAndScopeFromNode(node->child[0]).scope;
        m = getObjectByID(id, scope);
    }
    if (typeOf(m) == AS_LIST || typeOf(m) == AS_FILE) {
        m = performListAssignment(node, expr, m);
    } else if (typeOf(m) == AS_STRUCT || typeOf(m) == AS_KVPAIR) {
        m = performStructFieldAssignment(node, expr, m);
    } else if (typeOf(m) == AS_STRING) {
        m = performSubscriptStringAssignment(node, expr, m);
    } else {
        cout<<"Object "<<id<<" type does not support subscript assignment: "<<toString(m)<<endl;
        return makeNilObject();
    }
    return m;
}

Object ASTInterpreter::performListAccess(astnode* node, Object m) {
    //cout<<"subscript list access"<<endl;
    Object subm = evalExpression(node->child[1]);
    ListObj* list = getList(m);
    int subscript = stoi(toString(subm));
    return getListItem(list, subscript);
}

Object ASTInterpreter::performListAssignment(astnode* node, astnode* expr, Object& m) {
    ListObj* list = getList(m);
    Object subm = evalExpression(node->child[1]);
    int sub = stoi(toString(subm));
    list = updateListItem(list, sub, evalExpression(expr));
    if (typeOf(m) == AS_LIST) {
        m.objval->listObj = list;
    } else {
        m.objval->fileObj->lines = list;
        saveFile(m);
    }
    return m;
}

Object ASTInterpreter::performStructFieldAccess(astnode* node, string id, Object m) {
    string vname = getAttributes(node->child[1]).strval;
    Object t = makeNilObject();
    if (typeOf(m) == AS_STRUCT) {
        StructObj* sobj = getStruct(m);
        if (!sobj->blessed) {
            cout<<"Structs must be instantiated before use."<<endl;
            return t;
        }
        if (sobj->bindings.find(vname) == sobj->bindings.end()) {
            cout<<"Struct '"<<id<<"' has no such property '"<<vname<<"'."<<endl;
            return t;
        }
        t = sobj->bindings[vname];
    } else {
        KVPair* kvp = getKVPair(m);
        if (vname == toString(kvp->key))
            t = kvp->value;
        else cout<<"No Object found with Key '"<<vname<<"'"<<endl;
    }
    return t;
}

Object ASTInterpreter::performStructFieldAssignment(astnode* node, astnode* expr, Object& m) {
    string vname = getAttributes(node->child[1]).strval;
    Object t = evalExpression(expr);
    if (typeOf(m) == AS_STRUCT) {
        StructObj* st = getStruct(m);
        if (st->blessed) {
            st->bindings[vname] = t;
        } else {
            cout<<"Structs must be instantiated before use."<<endl;
            return makeNilObject();
        }
    } else {
        KVPair* kvp = getKVPair(m);
        if (vname == toString(kvp->key))
            kvp->value = t;
        else cout<<"No Object found with Key '"<<toString(m)<<"'"<<endl;
    }
    return m;
}

Object ASTInterpreter::performSubscriptStringAccess(astnode* node, Object m) {
    Object subm = evalExpression(node->child[1]);
    int subscript = stoi(toString(subm));
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

Object ASTInterpreter::performSubscriptStringAssignment(astnode* node, astnode* expr, Object& m) {
    Object subm = evalExpression(node->child[1]);
    int subscript = stoi(toString(subm));
    StringObj* strobj = getString(m);
    if (subscript >= 0 && subscript < strobj->length) {
        string s;
        int i;
        for (i = 0; i < subscript; i++)
            s.push_back(strobj->str[i]);
        s += toString(evalExpression(expr));
        for (int i = s.length()-1; i < strobj->length; i++)
            s.push_back(strobj->str[i]);
        m = makeStringObject(s); 
    } else {
        cout<<"index "<<subscript<<" is out of range for string of length "<<strobj->length<<endl;
    }
    return m;
}
