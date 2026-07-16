#include "ast.hpp"

void preorder(astnode* node, int d) {
    if (node != nullptr) {
        for (int i = 0; i < d; i++) cout<<" ";
        cout<<"[";
        switch (node->kind) {
            case EXPRNODE: cout<<exprTypeStr[node->expr]; break;
            case STMTNODE: cout<<stmtTypeStr[node->stmt]; break;
        }
        cout<<"] "<<node->token.getString()<<endl;
        preorder(node->left, d + 1);
        preorder(node->right, d + 1);
        preorder(node->next, d);
    }
}

bool isStmtNode(astnode* ast) {
    return ast != nullptr && ast->kind == STMTNODE;
}

bool isExprNode(astnode* ast) {
    return ast != nullptr && ast->kind == EXPRNODE;
}

bool isExprType(astnode* ast, ExprType type) {
    return isExprNode(ast) && ast->expr == type;
}

bool isStmtType(astnode* ast, StmtType type) {
    return isStmtNode(ast) && ast->stmt == type;
}