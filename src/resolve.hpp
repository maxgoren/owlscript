#ifndef resolve_hpp
#define resolve_hpp
#include <iostream>
#include <unordered_map>
#include "ast.hpp"
#include "object.hpp"
#include "context.hpp"
using namespace std;

/*
enum ExprType {
    CONST_EXPR, ID_EXPR, BINARYOP_EXPR, UNARYOP_EXPR, RELOP_EXPR, ASSIGN_EXPR, 
    FUNC_EXPR, LAMBDA_EXPR, LIST_EXPR, SUBSCRIPT_EXPR, BLESS_EXPR
};

enum StmtType {
    WHILE_STMT, FOR_STMT, PRINT_STMT, LET_STMT, IF_STMT, 
    EXPR_STMT, DEF_STMT, RETURN_STMT, STRUCT_STMT, BLOCK_STMT
};

*/
class Resolve {
    private:
        int d = 0;
        typedef unordered_map<string, bool> ScopeMap;
        InspectableStack<ScopeMap> scopes;
        void declare(string id) {
            if (scopes.empty())
                return;
            if (scopes.top().find(id) != scopes.top().end()) {
                cout<<"That name already exists in this scope."<<endl;
                return;
            }
            cout<<"Declare: "<<id<<endl;
            scopes.top()[id] = false;
        }
        void define(string id) {
            if (scopes.empty())
                return;
            cout<<"Define: "<<id<<endl;
            scopes.top()[id] = true;
        }
        void beginScope() {
            scopes.push(ScopeMap());
        }
        void endScope() {
            scopes.pop();
        }
        void resolveLocal(astnode* node, string id) {
            for (int i = scopes.size() - 1; i >= 0; i--) {
                ScopeMap scope = scopes.get(i);
                if (scope.find(id) != scope.end()) {
                    node->attributes.nestLevel = scopes.size() - 1 - i;
                    cout<<id<<" is a depth "<<node->attributes.nestLevel<<endl;
                    return;
                }
            }
        }
        void visitExpression(astnode* node) {
            switch (node->exprType) {
                case ID_EXPR:
                    resolveLocal(node, node->attributes.strval);
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
                        define(node->child[0]->child[0]->attributes.strval);
                        if (node->child[0]->child[1]->attributes.symbol == TK_ID) {
                            define(node->child[0]->child[1]->attributes.strval);
                        }
                    }
                    define(node->child[0]->attributes.strval);
                    resolve(node->child[0]);
                    node->attributes.nestLevel = node->child[0]->attributes.nestLevel;
                    resolve(node->child[1]);
                    break;
                case LAMBDA_EXPR: {
                        resolve(node->child[1]);
                        resolve(node->child[0]);
                    }
                    break;
                case FUNC_EXPR: {
                        define(node->attributes.strval);
                        resolve(node->child[1]);
                        resolve(node->child[0]);
                        node->attributes.nestLevel = node->child[0]->attributes.nestLevel;
                    }
                    break;
                case LIST_EXPR:
                case SUBSCRIPT_EXPR:
                    define(node->child[0]->attributes.strval);
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

        void visitStatement(astnode* node) {
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
                        beginScope();
                        resolve(node->child[0]);
                        endScope();
                    }
                    break;
                case DEF_STMT: {
                        declare(node->attributes.strval);
                        define(node->attributes.strval);
                        beginScope();
                        for (auto it = node->child[1]; it != nullptr; it = it->next) {
                            declare(it->attributes.strval);
                            define(it->attributes.strval);
                        }
                        resolve(node->child[0]);
                        endScope();
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
                    //if (node->child[0]->child[0]->attributes.strval != "[")
                        declare(node->child[0]->child[0]->attributes.strval);
                    resolve(node->child[0]);
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
        void visit(astnode* node) {
            switch (node->type) {
                case EXPR_NODE:
                    visitExpression(node);
                    break;
                case STMT_NODE:
                    visitStatement(node);
                    break;
            }
        }
        void resolve(astnode* node) {
            d++;
            if (node != nullptr) {
                //for (int i = 0; i < d; i++) cout<<"  ";
                visit(node);
                resolve(node->next);
            }
            d--;
        }
        Context cxt;
        public:
            Resolve() {
                d = 0;
            }
        
            void doResolve(Context& context, astnode* node) {
                d = 0;
                scopes.clear();
                cxt = context;
                resolve(node);
                context = cxt;
            }
            Context& getResolvedContext() {
                return cxt;
            }
};

#endif