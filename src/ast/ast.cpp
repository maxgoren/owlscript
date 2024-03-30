#include "ast.hpp"

ASTNode* makeExprNode(ExprK type, TOKENS token, string str) {
    ASTNode* node = new ASTNode;
    node->kind = EXPRNODE;
    node->type.expr = type;
    node->data.stringVal = str;
    node->data.tokenVal = token;
    node->left = nullptr;
    node->mid = nullptr;
    node->right = nullptr;
    node->next = nullptr;
    return node;
}

ASTNode* makeStmtNode(StmtK type, TOKENS token, string str) {
    ASTNode* node = new ASTNode;
    node->kind = STMTNODE;
    node->type.stmt = type;
    node->data.stringVal = str;
    node->data.tokenVal = token;
    node->left = nullptr;
    node->mid = nullptr;
    node->right = nullptr;
    node->next = nullptr;
    return node;
}

ASTTracer::ASTTracer() {
    depth = 0;
}

void ASTTracer::traverse(ASTNode* x) {
    depth++;
    for (auto node = x; node != nullptr; node = node->next) {
        if (node != nullptr)
            printNode(node);
        if (node->left != nullptr)
            traverse(node->left);
        if (node->mid != nullptr)
            traverse(node->mid);
        if (node->right != nullptr)
            traverse(node->right);
    }
    depth--;
}

void ASTTracer::printNode(ASTNode* node) {
    if (node == nullptr) return;
    for (int i = 0; i < depth; i++)
        cout<<"  ";
    if (node->kind == EXPRNODE) {
        switch (node->type.expr) {
            case ID_EXPR:    cout<<"[ID_EXPR]"<<endl; break;
            case CONST_EXPR: cout<<"[CONST_EXPR]"<<endl; break;
            case OP_EXPR:    cout<<"[OP_EXPR]"<<endl; break;
            case FUNC_EXPR:  cout<<"[FUNC_EXPR]"<<endl; break;
            case LAMBDA_EXPR: cout<<"[LAMBDA_EXPR]"<<endl; break;
            case STRINGLIT_EXPR: cout<<"[STRINGLIT_EXPR]"<<endl; break;
            case LIST_EXPR: cout<<"[LIST_EXPR]"<<endl; break;
            case LISTLEN_EXPR: cout<<"[LISTLEN_EXPR]"<<endl; break;
            case SORT_EXPR: cout<<"[SORT_EXPR]"<<endl; break;
            case CAR_EXPR: cout<<"[CAR_EXPR]"<<endl; break;
            case CDR_EXPR: cout<<"[CDR_EXPR]"<<endl; break;
            default:
                break;
        }
    } else if (node->kind == STMTNODE) {
        switch (node->type.stmt) {
            case IF_STMT:     cout<<"[IF_STMT]"<<endl; break;
            case LOOP_STMT:   cout<<"[LOOP_STMT]"<<endl; break;
            case PRINT_STMT:  cout<<"[PRINT_STMT]"<<endl; break;
            case ASSIGN_STMT: cout<<"[ASSIGN_STMT]"<<endl; break;
            case DEF_STMT:    cout<<"[DEF_STMT]"<<endl; break;
            case RETURN_STMT: cout<<"[RETURN_STMT]"<<endl; break;
            case PUSH_STMT:   cout<<"[PUSH_STMT]"<<endl; break;
            case APPEND_STMT: cout<<"[APPEND_STMT]"<<endl; break;
            case POP_STMT:    cout<<"[POP_STMT]"<<endl; break;
            default:
                break;
        }
    }
}
