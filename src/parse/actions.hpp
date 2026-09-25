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

astnode* through(vector<astnode*>& a) {
    return a[0];
}

astnode* pass(vector<astnode*>& a) {
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

astnode* mkbinop(vector<astnode*>& reducing) {
    astnode* nn = new astnode(BIN_EXPR, reducing[1]->token);
    nn->left = reducing[0];
    nn->right = reducing[2];
    if (nn->token.getSymbol() == TK_RANGE) nn->expr = RANGE_EXPR;
    return nn; 
}
astnode* unary(vector<astnode*>& reducing) {
    astnode* nn = nullptr;
    if (reducing[0]->token.getSymbol() == TK_NOT || reducing[0]->token.getSymbol() == TK_SUB) {
        nn = new astnode(UOP_EXPR, reducing[0]->token);
        nn->left = reducing[1];
    } else if (reducing[1]->token.getSymbol() == TK_INCREMENT || reducing[1]->token.getSymbol() == TK_DECREMENT) {
        nn = new astnode(UOP_EXPR, reducing[1]->token);
        nn->left = reducing[0];
    }
    return nn;
}

astnode* mkProg(vector<astnode*>& reducing) {
    if (reducing[0]->token.getString() == "Epsilon") {
        return reducing[1];
    }
    reducing[0]->next = reducing[1];
    return reducing[0];
}

astnode* mkList(vector<astnode*>& reducing) {
    if (reducing[0]->token.getSymbol() == TK_LPAREN && reducing[1]->token.getSymbol() == TK_RPAREN) {
        return  reducing[0];
    }
    for (int i = 1; i < reducing.size(); i++) {
            astnode* itr = reducing[0];
            while (itr->next) itr = itr->next;
            itr->next = reducing[i]->token.getSymbol() == TK_COMMA ? reducing[i]->left:reducing[i];
    }
    if (reducing[0]->token.getString() == "Epsilon") reducing[0] = reducing[0]->next;
    return reducing[0];
}

astnode* mkPrint(vector<astnode*>& reducing) {
    astnode* nn = new astnode(PRINT_STMT,reducing[0]->token);
    nn->left = reducing[1];
    return nn;
}

astnode* mkIf(vector<astnode*>& reducing) {
    astnode* nn = new astnode(IF_STMT,reducing[0]->token);
    nn->left = reducing[2];
    if (reducing[5]->token.getString() == "Epsilon") {
        nn->right = reducing[4]->left;
    } else {
        reducing[5]->left = reducing[4]->left;
        nn->right = reducing[5];
    }
    return nn;
}

astnode* mkElse(vector<astnode*>& reducing) {
    astnode* nn = new astnode(ELSE_STMT, reducing[0]->token);
    nn->right = reducing[1]->left;
    return nn;
}

astnode* mkTern(vector<astnode*>& reducing) {
    astnode* nn = new astnode(IF_STMT, reducing[1]->token);
    nn->left = reducing[0];
    nn->right = new astnode(ELSE_STMT, reducing[3]->token);
    nn->right->left = reducing[2];
    nn->right->right = reducing[4];
    return nn;
}

astnode* mkWhile(vector<astnode*>& reducing) {
    astnode* nn = new astnode(WHILE_STMT, reducing[0]->token);
    nn->left = reducing[2];
    nn->right = reducing[4]->left;
    return nn;
}

astnode* mkFor(vector<astnode*>& reducing) {
    astnode* nn = new astnode(FOREACH_STMT, reducing[0]->token);
    nn->left = reducing[2];
    nn->right = reducing[4]->left;
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
    astnode* nn = new astnode(BLOCK_STMT, reducing[0]->token);
    nn->left = reducing[1];
    return nn;
}

astnode* mkStmtList(vector<astnode*>& reducing) {
    astnode* nn = new astnode(STMT_LIST, reducing[0]->token);
    nn->left = reducing[1];
    return nn;
}

astnode* mkCall(vector<astnode*>& reducing) {
    cout<<"mk call from: ";
    int i = 0;
    for (auto m : reducing) {
        cout<<++i<<": "<<m->token.getString()<<endl;
    }
    astnode* nn = new astnode(FUNC_EXPR, reducing[0]->token);
    nn->left = reducing[0];
    nn->left->kind = EXPRNODE;
    if (nn->left->kind == TEMP_NODE) {
        nn->left->expr = ID_EXPR;
    }
    if (reducing[2]->token.getString() != "Epsilon")
        nn->right = reducing[2];
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
    return reducing[0];
}

astnode* mkRet(vector<astnode*>& reducing) {
    reducing[0]->kind = STMTNODE;
    reducing[0]->stmt = RETURN_STMT;
    reducing[0]->left = reducing[1];
    return reducing[0];
}


astnode* mkFunc(vector<astnode*>& reducing) {
    astnode* ls = new astnode(LET_STMT, reducing[1]->token);

    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = LAMBDA_EXPR;
    if (reducing.size() == 6) {
        nn->left = reducing[3]->token.getString() == "Epsilon" ? nullptr:reducing[3];
        nn->right = reducing[5]->left;
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
    } else {
        cout<<"Nah man."<<endl;
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
    return nn;
}

astnode* mkInstance(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = BLESS_EXPR;
    nn->left = reducing[1];
    nn->left->kind = EXPRNODE;
    nn->left->expr = ID_EXPR;
    nn->right = reducing[3];
    return nn;
}

astnode* mkLambda(vector<astnode*>& reducing) {
    astnode* nn = reducing[0];
    nn->kind = EXPRNODE;
    nn->expr = LAMBDA_EXPR;
    cout<<"mk lambda from ";
    for (auto m : reducing) {
        cout<<tokenStr[m->token.getSymbol()]<<" "<< m->token.getString()<<endl;
    }
    nn->token.setString("lambda");
    nn->left = reducing[1];
    nn->right = reducing[4]->left;
    for (auto m = nn->right; m != nullptr; m = m->next) {
        if (m->token.getSymbol() == TK_LET) {
            m->left->kind = EXPRNODE;
            m->left->expr = ID_EXPR;
        }
    }
    return nn;
}

astnode* mkSubscript(vector<astnode*>& reducing) {
    astnode* nn = new astnode(SUBSCRIPT_EXPR, reducing[1]->token);
    nn->left = reducing[0];
    nn->right = reducing[2];
    return nn;
}

astnode* mkDotted(vector<astnode*>& reducing) {
    astnode* nn = new astnode(FIELD_EXPR, reducing[1]->token);
    nn->left = reducing[0];
    nn->right = reducing[2];
    return nn;
}

astnode* mkListCon(vector<astnode*>& reducing) {
    reducing[0]->kind = EXPRNODE;
    reducing[0]->expr = LISTCON_EXPR;
    if (reducing[1]->token.getSymbol() == TK_LB)
        return reducing[0];
    else {
        for (int i = 1; i < reducing.size()-1; i++) {
            if (reducing[0]->left == nullptr) {
                reducing[0]->left = reducing[1];
            } else {
                astnode* itr = reducing[0]->left;
                while (itr->next != nullptr) itr = itr->next;
                itr->next = reducing[i];
            }
        }
    }
    return reducing[0];
}

astnode* mkListOp(vector<astnode*>& reducing) {
    astnode* nn = new astnode(LIST_EXPR, reducing[0]->token);
    if (nn->token.getSymbol() == TK_APPEND) nn->left = reducing[2];
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
    return nn;
}

#endif