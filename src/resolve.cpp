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

void ResolveScope::resolveStatementScope(astnode* node) {
    switch (node->stmtType) {
        case PRINT_STMT:
            resolve(node->child[0]);
            break;
        case WHILE_STMT:
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case IF_STMT:
            for (int i = 0; i < 3; i++)
                resolve(node->child[i]);
            break;
        case BLOCK_STMT: {
                openScope();
                resolve(node->child[0]);
                closeScope();
            }
            break;
        case DEF_STMT: {
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
            break;
        case FOR_STMT:
            for (int i = 0; i < 3; i++)
                resolve(node->child[i]);
            break;
        case EXPR_STMT:
            resolve(node->child[0]);
            break;
        case LET_STMT: {
                switch (node->child[0]->child[0]->exprType) {
                    case ID_EXPR:
                        declareVarName(node->child[0]->child[0]->attributes.strval);
                        break;
                    case SUBSCRIPT_EXPR:
                        declareVarName(node->child[0]->child[0]->child[0]->attributes.strval);
                        break;
                    default:
                        break;
                }
                resolve(node->child[0]);
                switch (node->child[0]->child[0]->exprType) {
                    case ID_EXPR:
                        defineVarName(node->child[0]->child[0]->attributes.strval);
                        break;
                    case SUBSCRIPT_EXPR:
                        defineVarName(node->child[0]->child[0]->child[0]->attributes.strval);
                        break;
                    default:
                        break;
                }
            }
            break;
        case RETURN_STMT: 
            resolve(node->child[0]);
            break;
        case STRUCT_STMT: 
            break;
        default:
            break;
    }
}

void ResolveScope::resolveExpressionScope(astnode* node) {                                                
    switch (node->exprType) {
        case ID_EXPR: 
            resolveVariableDepth(node, node->attributes.strval);
            break;
        case BINARYOP_EXPR:
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case UNARYOP_EXPR:
            resolve(node->child[0]);
            break;
        case RELOP_EXPR:
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case ASSIGN_EXPR:
            if (node->child[0]->exprType == SUBSCRIPT_EXPR) {
                defineVarName(node->child[0]->child[0]->attributes.strval);
                if (node->child[0]->child[1]->attributes.symbol == TK_ID) {
                    defineVarName(node->child[0]->child[1]->attributes.strval);
                }
            }
            defineVarName(node->child[0]->attributes.strval);
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case LAMBDA_EXPR: {
                resolve(node->child[1]);
                resolve(node->child[0]);
            }
            break;
        case FUNC_EXPR: {
                resolveVariableDepth(node, node->attributes.strval);
                resolve(node->child[1]);
            }
            break;
        case LIST_EXPR:
        case SUBSCRIPT_EXPR:
            if (node->child[0] != nullptr && !node->child[0]->attributes.strval.empty())
                defineVarName(node->child[0]->attributes.strval);
            resolve(node->child[0]);
            resolve(node->child[1]);
            break;
        case BLESS_EXPR:
            resolve(node->child[0]);
            break;
        default:
            break;
    }
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
        ScopeMap scope = scopes.get(i);
        if (scope.find(id) != scope.end()) {
            node->attributes.depth = scopes.size() - 1 - i;
            if (loud) {
                cout<<"Resolve: "<<id<<" at nest depth "<<node->attributes.depth<<endl;
            }
            return;
        }
    }
    node->attributes.depth = -1;
}