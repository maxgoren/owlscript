#include "ast.hpp"

void printExprNode(astnode* expr) {
    switch (expr->type.expr) {
        case ID_EXPR:      cout<<"[id expr]"; break;
        case CONST_EXPR:   cout<<"[cosnt expr]"; break;
        case UNOP_EXPR:    cout<<"[unop expr]"; break;
        case BINOP_EXPR:   cout<<"[binop expr]"; break;
        case RELOP_EXPR:   cout<<"[relop expr]"; break;
        case LOGIC_EXPR:   cout<<"[logic expr]"; break;
        case FUNC_EXPR:    cout<<"[func expr]"; break;
        case ASSIGN_EXPR:  cout<<"[assign expr]"; break;
        case LAMBDA_EXPR:  cout<<"[lambda expr]"; break;
        case REG_EXPR:     cout<<"[regular expr]"; break;
        case REF_EXPR:     cout<<"[reference expr]"; break;
        case LIST_EXPR:    cout<<"[list expr]"; break;
        case RANGE_EXPR:   cout<<"[range expr]"; break;
        case ZF_EXPR:      cout<<"[list comprehension]"; break;
        case BLESS_EXPR:   cout<<"[bless expr]"; break;
        case TERNARY_EXPR: cout<<"[ternary expr]"; break;
        case SUBSCRIPT_EXPR: cout<<"[subscript expr]"; break;
        default:
            break;
    }
}

void printStmtNode(astnode* stmt) {
    switch (stmt->type.stmt) {
        case IF_STMT:     cout<<"[if statement]"; break;
        case LET_STMT:    cout<<"[let statement]"; break;
        case EXPR_STMT:   cout<<"[expr statement]"; break;
        case PRINT_STMT:  cout<<"[print statement]"; break;
        case WHILE_STMT:  cout<<"[while statement]";break;
        case BREAK_STMT:  cout<<"[break statement]"; break;
        case CONTINUE_STMT: cout<<"[continue statement]"; break;
        case FOREACH_STMT: cout<<"[foreach statement]"; break;
        case BLOCK_STMT:  cout<<"[block statement]"; break;
        case RETURN_STMT: cout<<"[return statement]"; break;
        case FUNC_DEF_STMT: cout<<"[def statement]"; break;
        case STRUCT_DEF_STMT: cout<<"[struct definition statement]"; break;
        default:
            break;
    }
}

void printNode(astnode* node, int d) {
    for (int i = 0; i < d; i++) cout<<" ";
    switch(node->nk) {
        case EXPR_NODE: printExprNode(node); break;
        case STMT_NODE: printStmtNode(node); break;
        default:
            break;
    }
    printToken(node->token);
}

void preorder(astnode* node, int d) {
    if (node != nullptr) {
        printNode(node, d);
        for (int i = 0; i < MAX_CHILD; i++)
            preorder(node->child[i], d+1);
        preorder(node->next, d);
    }
}

astnode* makeExprNode(ExprType type, Token tk) {
    astnode* node = new astnode(EXPR_NODE, tk);
    node->type.expr = type;
    return node;
}

astnode* makeStmtNode(StmtType type, Token tk) {
    astnode* node = new astnode(STMT_NODE, tk);
    node->type.stmt = type;
    return node;
}

astnode* copyTree(astnode* node) {
    if (node == nullptr)
        return nullptr;
    astnode* t = new astnode(node->nk, node->token);
    t->type = node->type;
    for (int i = 0; i < MAX_CHILD; i++)
        t->child[i] = copyTree(node->child[i]);
    t->next = copyTree(node->next);
    return t;
}

void cleanUpTree(astnode* node) {
    if (node == nullptr)
        return;
    for (int i = 0; i < MAX_CHILD; i++)
        cleanUpTree(node->child[i]);
    cleanUpTree(node->next);
    delete node;
}

bool isExprType(astnode* node, ExprType type) {
    if (node == nullptr)
        return false;
    return node->nk == EXPR_NODE && node->type.expr == type;
}

bool isStmtType(astnode* node, StmtType type) {
    if (node == nullptr)
        return false;
    return node->nk == STMT_NODE && node->type.stmt == type;
}