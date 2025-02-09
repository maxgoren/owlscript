#include "resolve.hpp"

ResolveScope::ResolveScope(bool debug) {
    loud = debug;
}

void ResolveScope::resolveScope(astnode* node) {
    scopes.clear(); 
    resolve(node);
}

void ResolveScope::resolve(astnode* node) {
    if (node != nullptr) {
        switch (node->type) {
            case EXPR_NODE:
                resolveExpressionScope(node);
                break;
            case STMT_NODE:
                resolveStatementScope(node);
                break;
            default:
                break;
        }
        resolve(node->next);
    }
}

void ResolveScope::resolveBlockStatement(astnode* node) {
    openScope();
    resolve(node->child[0]);
    closeScope();
}

void ResolveScope::resolveLetStatement(astnode* node) {
    astnode* x = node->child[0];
    while (x != nullptr) {
        if (isExprType(x, ID_EXPR))
            break;
        x = x->child[0];
    }
    declareVarName(x->attributes.strval);
    resolve(node->child[0]);
    defineVarName(x->attributes.strval);
}

void ResolveScope::resolveDefStatement(astnode* node) {
    declareVarName(node->attributes.strval);
    defineVarName(node->attributes.strval);
    openScope();
    for (auto it = node->child[1]; it != nullptr; it = it->next) {
        declareVarName(it->attributes.strval);
        defineVarName(it->attributes.strval);
    }
    resolve(node->child[0]);
    closeScope();
}

void ResolveScope::resolveForEachStatement(astnode* node) {
    openScope();
        declareVarName(node->child[0]->attributes.strval);
        declareVarName(node->child[0]->next->attributes.strval);
        defineVarName(node->child[0]->attributes.strval);
        defineVarName(node->child[0]->next->attributes.strval);
        for (int i = 0; i < 3; i++)
            resolve(node->child[i]);
    closeScope();
}

void ResolveScope::resolveStatementScope(astnode* node) {
    switch (node->stmtType) {
        case BLOCK_STMT: resolveBlockStatement(node); break;
        case DEF_STMT:     resolveDefStatement(node); break;
        case FOREACH_STMT: resolveForEachStatement(node); break;
        case LET_STMT: 
            resolveLetStatement(node); break;
        case FOR_STMT:
            for (int i = 0; i < 3; i++)
                resolve(node->child[i]);
            break;
        case IF_STMT:
            for (int i = 0; i < 3; i++)
                resolve(node->child[i]);
            break;
        case WHILE_STMT:
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case EXPR_STMT:
            resolve(node->child[0]);
            break;
        case RETURN_STMT: 
            resolve(node->child[0]);
            break;
        case PRINT_STMT:
            resolve(node->child[0]);
            break;
        case STRUCT_STMT:
            resolve(node->child[0]); 
            break;
        default:
            break;
    }
}

void ResolveScope::resolveLambdaExpression(astnode* node) {
    openScope();
    for (auto it = node->child[1]; it != nullptr; it = it->next) {
        declareVarName(it->attributes.strval);
        defineVarName(it->attributes.strval);
    }
    resolve(node->child[0]);
    closeScope();
}

void ResolveScope::resolveExpressionScope(astnode* node) {                                                
    switch (node->exprType) {
        case LAMBDA_EXPR:
            resolveLambdaExpression(node);
            return;
        case ID_EXPR: 
            resolveVariableDepth(node, node->attributes.strval);
            break;
        case FUNC_EXPR:
            resolveVariableDepth(node, node->attributes.strval);
            break;
        case REF_EXPR:
            defineVarName(node->child[0]->attributes.strval);
            break;
        default:
            break;
    }
    for (int i = 0; i < 3; i++)
        if (node->child[i] != nullptr)
            resolve(node->child[i]);
}

void ResolveScope::declareVarName(string id) {
    if (scopes.empty())
        return;
    if (scopes.top().find(id) != scopes.top().end()) {
        cout<<"That name already exists in this scope."<<endl;
        return;
    }
    if (loud) {
        cout<<"Declare: "<<id<<" (scope: "<<scopes.size()<<")"<<endl;
    }
    scopes.top()[id] = false;
}

void ResolveScope::defineVarName(string id) {
    if (scopes.empty())
        return;
    if (loud) {
        cout<<"Define: "<<id<<" (scope: "<<scopes.size()<<")"<<endl;
    }
    scopes.top()[id] = true;
}

void ResolveScope::openScope() {
    scopes.push(ScopeMap());
}

void ResolveScope::closeScope() {
    scopes.pop();
}

void ResolveScope::resolveVariableDepth(astnode* node, string id) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes.get(i).find(id) != scopes.get(i).end()) {
            node->attributes.depth = scopes.size() - 1 - i;
            if (loud) {
                cout<<"Resolve: "<<id<<" at nest depth "<<node->attributes.depth<<endl;
            }
            return;
        }
    }
    node->attributes.depth = -1;
}