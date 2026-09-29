#ifndef actions_hpp
#define actions_hpp
#include <map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "ast.hpp"
using namespace std;

astnode* mkNum(vector<astnode*>& a) {
    a[0]->kind = EXPRNODE; 
    a[0]->expr = CONST_EXPR; 
    return a[0]; 
}

astnode* pass(vector<astnode*>& a) {
    delete a[0];
    if (a.size() > 2)
        delete a[2];
    return a[1];
}

astnode* mkId(vector<astnode*>& a) {
    a[0]->kind = EXPRNODE; 
    a[0]->expr = ID_EXPR; 
    return a[0];
}

astnode* mkString(vector<astnode*>& a) {
    a[0]->kind = EXPRNODE; 
    a[0]->expr = CONST_EXPR; 
    return a[0]; 
}

astnode* mkExprStmt(vector<astnode*>& reducing) {
    reducing[1]->kind = STMTNODE;
    reducing[1]->stmt = EXPR_STMT;
    reducing[1]->left = reducing[0];
    return reducing[1];
}

astnode* mkbinop(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE; nn->expr = BIN_EXPR;
    nn->left = reducing[0];
    nn->right = reducing[2];
    if (nn->token.getSymbol() == TK_RANGE) nn->expr = RANGE_EXPR;
    return nn; 
}
astnode* unary(vector<astnode*>& reducing) {
    astnode* nn = nullptr;
    if (reducing[0]->token.getSymbol() == TK_NOT || reducing[0]->token.getSymbol() == TK_SUB) {
        nn = reducing[0];
        nn->kind = EXPRNODE; nn->expr = UOP_EXPR;
        nn->left = reducing[1];
    } else if (reducing[1]->token.getSymbol() == TK_INCREMENT || reducing[1]->token.getSymbol() == TK_DECREMENT) {
        nn = reducing[1];
        nn->kind = EXPRNODE; nn->expr = UOP_EXPR;
        nn->left = reducing[0];
    }
    return nn;
}

astnode* mkProg(vector<astnode*>& reducing) {
    if (reducing[0]->token.getString() == "Epsilon") {
        delete reducing[0];
        return reducing[1];
    }
    reducing[0]->next = reducing[1];
    return reducing[0];
}

astnode* mkList(vector<astnode*>& reducing) {
    if (reducing[0]->token.getSymbol() == TK_LPAREN && (reducing[1]->token.getSymbol() == TK_RPAREN || reducing[1]->token.getString() == "Epsilon")) {
        delete reducing[1];
        return  reducing[0];
    }
    for (int i = 1; i < reducing.size(); i++) {
            if (reducing[i]->token.getString() != "Epsilon" && reducing[i]->token.getSymbol() != TK_COMMA) {
                astnode* itr = reducing[0];
                while (itr->next) itr = itr->next;
                itr->next = reducing[i];
            } else {
                reducing[i]->next = nullptr;
                reducing[i]->left = nullptr;
                reducing[i]->right = nullptr;
                delete reducing[i];
            }
    }
    return reducing[0];
}

astnode* mkPrint(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = PRINT_STMT;
    nn->left = reducing[1];
    delete reducing[2];
    return nn;
}

astnode* mkIf(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = IF_STMT;
    nn->left = reducing[2];
    if (reducing[5]->token.getString() == "Epsilon") {
        auto tmp = reducing[4];
        nn->right = tmp->left;
        tmp->left = nullptr;
        delete tmp;
        delete reducing[5];
    } else {
        reducing[5]->left = reducing[4]->left;
        nn->right = reducing[5];
        reducing[4]->left = nullptr;
        delete reducing[4];
    }
    delete reducing[1];
    delete reducing[3];
    return nn;
}

astnode* mkElse(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = ELSE_STMT;
    auto tmp = reducing[1];
    nn->right = reducing[1]->left;
    tmp->left = nullptr;
    delete tmp;
    return nn;
}

astnode* mkTern(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    reducing[1]->kind = STMTNODE; reducing[1]->stmt = IF_STMT;
    reducing[3]->kind = STMTNODE; reducing[3]->stmt = ELSE_STMT;
    nn->left = reducing[0];
    nn->right = reducing[3];
    nn->right->left = reducing[2];
    nn->right->right = reducing[4];
    return nn;
}

astnode* mkWhile(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = WHILE_STMT;
    nn->left = reducing[2];
    nn->right = reducing[4]->left;
    delete reducing[1];
    delete reducing[3];
    reducing[4]->left = nullptr;
    delete reducing[4];
    return nn;
}

astnode* mkFor(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    reducing[0]->kind = STMTNODE;
    reducing[0]->stmt = FOREACH_STMT;
    nn->left = reducing[2];
    nn->right = reducing[4]->left;
    reducing[4]->left = nullptr;
    delete reducing[4];
    delete reducing[3];
    delete reducing[1];
    return nn;
}

astnode* mkOf(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE;
    nn->expr = ITERATOR_EXPR;
    reducing[0]->kind = EXPRNODE;
    reducing[0]->expr = ID_EXPR;
    nn->left = reducing[0];
    nn->right = reducing[2];
    return nn;
}

astnode* mkBlock(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = BLOCK_STMT;
    nn->left = reducing[1];
    delete reducing[2];
    return nn;
}

astnode* mkStmtList(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE; nn->stmt = STMT_LIST;
    nn->left = reducing[1];
    return nn;
}

astnode* mkCall(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE;
    nn->expr = FUNC_EXPR;
    nn->left = reducing[0];
    nn->left->kind = EXPRNODE;
    if (nn->left->kind == TEMP_NODE) {
        nn->left->expr = ID_EXPR;
    }
    if (reducing[2]->token.getString() != "Epsilon") {
        nn->right = reducing[2];
    } else {
        nn->right = nullptr;
        delete reducing[2];
    }
    delete reducing[3];
    return nn;
}

astnode* mkLet(vector<astnode*>& reducing) {
    reducing[0]->kind = STMTNODE;
    reducing[0]->stmt = LET_STMT;
    reducing[0]->left = reducing[1];
    if (reducing[0]->left->expr != BIN_EXPR) {
        reducing[0]->left->kind = EXPRNODE;
        reducing[0]->left->expr = ID_EXPR;
    }
    if (reducing.size() == 3) delete reducing[2];
    return reducing[0];
}

astnode* mkRet(vector<astnode*>& reducing) {
    reducing[0]->kind = STMTNODE;
    reducing[0]->stmt = RETURN_STMT;
    reducing[0]->left = reducing[1];
    delete reducing[2];
    return reducing[0];
}


astnode* mkFunc(vector<astnode*>& reducing) {
    astnode* ls = new astnode(LET_STMT, reducing[1]->token);
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = LAMBDA_EXPR;
    if (reducing.size() == 6) {
        if( reducing[3]->token.getString() == "Epsilon") {
            nn->left = nullptr;
            delete reducing[3];
        } else {
            nn->left = reducing[3];
        }
        nn->right = reducing[5]->left;
        reducing[5]->left = nullptr;
        delete reducing[5];
        for (auto m = nn->right; m != nullptr; m = m->next) {
            if (m->token.getSymbol() == TK_LET) {
                if (m->left->expr != BIN_EXPR && m->left->expr != ID_EXPR) {
                    m->left->kind = EXPRNODE;
                    m->left->expr = ID_EXPR;
                }
            }
        }
        astnode* res = new astnode(BIN_EXPR, Token(TK_ASSIGN, ":="));
        res->left = reducing[1];
        res->left->kind = EXPRNODE;
        res->left->expr = ID_EXPR;
        res->right = nn;
        ls->left = res;
        nn = ls;
        delete reducing[2];
        delete reducing[4];
    } 
    return nn;
}

astnode* mkStruct(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = STMTNODE;
    nn->stmt = DEF_CLASS_STMT;
    reducing[1]->kind = EXPRNODE;
    reducing[1]->expr = ID_EXPR;
    nn->left = reducing[1];
    nn->right = reducing[2]->left;
    reducing[2]->left = nullptr;
    delete reducing[2];
    delete reducing[3];
    return nn;
}

astnode* mkInstance(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = BLESS_EXPR;
    nn->left = reducing[1];
    nn->left->kind = EXPRNODE;
    nn->left->expr = ID_EXPR;
    if (reducing[3]->token.getString() == "Epsilon") {
        delete reducing[3];
    } else {
        nn->right = reducing[3];
    }
    delete reducing[2];
    delete reducing[4];
    return nn;
}

astnode* mkLambda(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = LAMBDA_EXPR;
    nn->token.setString("lambda");
    nn->left = reducing[1];
    nn->right = reducing[4]->left;
    reducing[4]->left = nullptr;
    delete reducing[4];
    delete reducing[2];
    delete reducing[3];
    for (auto m = nn->right; m != nullptr; m = m->next) {
        if (m->token.getSymbol() == TK_LET) {
            m->left->kind = EXPRNODE;
            m->left->expr = ID_EXPR;
        }
    }
    return nn;
}

astnode* mkSubscript(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE; nn->expr = SUBSCRIPT_EXPR;
    nn->left = reducing[0];
    nn->right = reducing[2];
    delete reducing[3];
    return nn;
}

astnode* mkDotted(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE;
    nn->expr = FIELD_EXPR;
    astnode* ll = reducing[0];
    astnode* rr = reducing[2]; 
        nn->left = ll;
        nn->right = rr;
    if (rr->expr == LIST_EXPR) nn->expr = LIST_EXPR;
    return nn;
}

astnode* mkListCon(vector<astnode*>& reducing) {
    reducing[0]->kind = EXPRNODE;
    reducing[0]->expr = LISTCON_EXPR;
    if (reducing[1]->token.getString() == "Epsilon") {
        delete reducing[1];
        delete reducing[2];
        reducing[0]->left = nullptr;
        return reducing[0];
    } else {
        for (int i = 1; i < reducing.size()-1; i++) {
            if (reducing[0]->left == nullptr) {
                reducing[0]->left = reducing[1];
            } else {
                if (reducing[i]->token.getSymbol() != TK_COMMA) {
                    astnode* itr = reducing[0]->left;
                    while (itr->next != nullptr) itr = itr->next;
                    itr->next = reducing[i];
                } else {
                    delete reducing[i];
                }
            }
        }
    }
    delete reducing[2];
    return reducing[0];
}

astnode* mkSetComp(vector<astnode*>& reducing) {
    astnode* nn = reducing[1];
    nn->kind = EXPRNODE;
    nn->expr = SETCOMP_EXPR;
    if (nn->token.getSymbol() == TK_IF && reducing[0]->token.getSymbol() == TK_AS) {
        auto it = reducing[0]->right;
        while (it->next != nullptr) it = it->next;
        it->next = reducing[2];
        nn = reducing[0];
        delete reducing[1];
    } else {
        nn->left = reducing[0];
        nn->right = reducing[2];
    }
    return nn;
}

astnode* mkListOp(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = LIST_EXPR;
    if (nn->token.getSymbol() == TK_APPEND || nn->token.getSymbol() == TK_PUSH) {
        nn->left = reducing[2];
        delete reducing[3];
    } else {
        delete reducing[2];
    }
    delete reducing[1];
    return nn;
}

astnode* mkConst(vector<astnode*>& reducing) {
    reducing[0]->kind = EXPRNODE;
    reducing[0]->expr = CONST_EXPR;
    return reducing[0];
}

astnode* mkImport(vector<astnode*>& reducing) {
    reducing[0]->kind = STMTNODE;
    reducing[0]->stmt = IMPORT_STMT;
    reducing[1]->kind = EXPRNODE;
    reducing[1]->expr = ID_EXPR;
    reducing[0]->left = reducing[1];
    delete reducing[2];
    return reducing[0];
}

astnode* mkRandom(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = CONST_EXPR;
    nn->left = reducing[2];
    delete reducing[1];
    delete reducing[3];
    return nn;
}

astnode* mkBuiltin(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = UOP_EXPR;
    nn->left = reducing[2];
    delete reducing[1];
    delete reducing[3];
    return nn;
}

#endif