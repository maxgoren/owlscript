#ifndef st_resolver_hpp
#define st_resolver_hpp
#include <iostream>
#include "../parse/ast.hpp"
#include "scopingst.hpp"
#include <unordered_map>
using namespace std;

static const int GLOBAL_SCOPE = -1;
static const int LOCAL_SCOPE = 0;

class STBuilder {
    private:
        ScopingST* symTable;
        string nameBlock() {
            static int bnum = 0;
            return "Block" + to_string(bnum++);
        }
        string nameLambda() {
            static int n = 0;
            return "lambdafunc" + to_string(n++);
        }
        void buildStatementST(astnode* t) {
            if (t == nullptr)
                return;
            switch (t->stmt) {
                case DEF_CLASS_STMT: {
                    symTable->openObjectScope(t->left->token.getString());
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                    symTable->closeScope();
                } break;
                case BLOCK_STMT: {
                    t->token.setString(nameBlock());
                    symTable->openFunctionScope(t->token.getString(), -1);
                    buildSymbolTable(t->left);
                    symTable->closeScope();
                } break;
                case LET_STMT: {
                    switch (t->left->expr) {
                        case ID_EXPR: {
                            buildExpressionST(t->left, true); 
                        } break;
                        case BIN_EXPR: {
                            auto binexpr = t->left;
                            if (binexpr->right->expr == BLESS_EXPR) {
                                symTable->copyObjectScope(binexpr->left->token.getString(), binexpr->right->left->token.getString());
                            } else {
                                buildExpressionST(binexpr->left, true);
                                buildExpressionST(binexpr->right, false);
                            }
                        } break;          
                    }
                } break;
                case PRINT_STMT: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case IF_STMT: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case ELSE_STMT: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case WHILE_STMT: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case EXPR_STMT: {
                    buildSymbolTable(t->left);
                } break;
                case RETURN_STMT: {
                    buildSymbolTable(t->left);
                } break;
                case STMT_LIST: {
                    buildStatementST(t->left);
                } break;
                case FOREACH_STMT: {
                    t->token.setString(nameBlock());
                    symTable->openFunctionScope(t->token.getString(), -1);
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                    symTable->closeScope();
                } break;
                default: 
                break;
            }
            buildSymbolTable(t->next);
        } 
        void buildExpressionST(astnode* t, bool fromLet) {
            if (t == nullptr)
                return;
            switch (t->expr) {
                case ID_EXPR: {
                    if (fromLet) {
                        if (symTable->existsInScope(t->token.getString()) == false) {
                            symTable->insert(t->token.getString());
                        }
                    } else if (symTable->lookup(t->token.getString()).addr == -1) {    
                        cout<<"Error: Unknown variable name: "<<t->token.getString()<<endl;
                    }
                } break;
                case FUNC_EXPR: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case LAMBDA_EXPR: {
                    string name = nameLambda();
                    t->token.setString(name);
                    symTable->openFunctionScope(t->token.getString(), -1);
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                    symTable->closeScope();
                } break;
                case BLESS_EXPR: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case SUBSCRIPT_EXPR: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case FIELD_EXPR: {
                    //rright hand side resolved dynamically at run time.
                    buildSymbolTable(t->left);
                } break;
                case RANGE_EXPR: {
                    buildSymbolTable(t->left);
                    buildSymbolTable(t->right);
                } break;
                case SETCOMP_EXPR: {
                    symTable->insert("scitr");
                    symTable->insert("scclti");
                    symTable->insert("sctrl");
                    buildExpressionST(t->left, true);
                    buildExpressionST(t->right, false);
                } break;
                case ITERATOR_EXPR: {
                    symTable->insert("itr");
                    symTable->insert("clti");
                    buildExpressionST(t->left, true);
                    buildExpressionST(t->right, false);
                } break;
                default: 
                    buildExpressionST(t->left, fromLet);
                    buildExpressionST(t->right, fromLet);
                    break;
            }
            buildSymbolTable(t->next);
        }
        void buildSymbolTable(astnode* t) {
            if (t != nullptr) {
                switch (t->kind) {
                    case STMTNODE: {
                        buildStatementST(t);
                    } break;
                    case EXPRNODE: {
                        buildExpressionST(t, false);
                    } break;
                }
            }
        }
    public:
        STBuilder() { }
        void buildSymbolTable(astnode* ast, ScopingST* st) {
            symTable = st;
            buildSymbolTable(ast);
        }
};


#endif