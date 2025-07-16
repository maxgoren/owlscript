#include "ast.hpp"

string exprNodeToString(astnode* expr) {
    switch (expr->type.expr) {
        case ID_EXPR:      return "[id expr]"; break;
        case CONST_EXPR:   return "[cosnt expr]"; break;
        case UNOP_EXPR:    return "[unop expr]"; break;
        case BINOP_EXPR:   return "[binop expr]"; break;
        case RELOP_EXPR:   return "[relop expr]"; break;
        case LOGIC_EXPR:   return "[logic expr]"; break;
        case FUNC_EXPR:    return "[func expr]"; break;
        case ASSIGN_EXPR:  return "[assign expr]"; break;
        case LAMBDA_EXPR:  return "[lambda expr]"; break;
        case REG_EXPR:     return "[regular expr]"; break;
        case REF_EXPR:     return "[reference expr]"; break;
        case LIST_EXPR:    return "[list expr]"; break;
        case RANGE_EXPR:   return "[range expr]"; break;
        case ZF_EXPR:      return "[list comprehension]"; break;
        case BLESS_EXPR:   return "[bless expr]"; break;
        case TERNARY_EXPR: return "[ternary expr]"; break;
        case SUBSCRIPT_EXPR: return "[subscript expr]"; break;
        default:
            break;
    }
    return "";
}

string stmtNodeToString(astnode* stmt) {
    switch (stmt->type.stmt) {
        case IF_STMT:     return "[if stmt]"; break;
        case LET_STMT:    return "[let stmt]"; break;
        case EXPR_STMT:   return "[expr stmt]"; break;
        case PRINT_STMT:  return "[print stmt]"; break;
        case WHILE_STMT:  return "[while stmt]";break;
        case BREAK_STMT:  return "[break stmt]"; break;
        case CONTINUE_STMT: return "[continue stmt]"; break;
        case FOREACH_STMT: return "[foreach stmt]"; break;
        case BLOCK_STMT:  return "[block stmt]"; break;
        case RETURN_STMT: return "[return stmt]"; break;
        case FUNC_DEF_STMT: return "[func def stmt]"; break;
        case STRUCT_DEF_STMT: return "[struct def stmt]"; break;
        default:
            break;
    }
    return "";
}

void printNode(astnode* node, int d) {
    for (int i = 0; i < d; i++) cout<<" ";
    switch(node->nk) {
        case EXPR_NODE: cout<<exprNodeToString(node); break;
        case STMT_NODE: cout<<stmtNodeToString(node); break;
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