#ifndef ast_hpp
#define ast_hpp
#include "token.hpp"
#include <list>
using namespace std;


const int MAX_CHILD = 3;

enum NodeKind {
    EXPR_NODE, STMT_NODE
};

enum ExprType {
    ID_EXPR, CONST_EXPR, 
    UNOP_EXPR, BINOP_EXPR, RELOP_EXPR, LOGIC_EXPR, BITWISE_EXPR,
    REG_EXPR, REF_EXPR, LAMBDA_EXPR, FUNC_EXPR, BLESS_EXPR,
    ASSIGN_EXPR, SUBSCRIPT_EXPR, RANGE_EXPR,
    LIST_EXPR, ZF_EXPR, TERNARY_EXPR
};

enum StmtType {
    PRINT_STMT, EXPR_STMT, BLOCK_STMT, 
    IF_STMT, WHILE_STMT, FOREACH_STMT, LET_STMT, 
    FUNC_DEF_STMT, STRUCT_DEF_STMT, RETURN_STMT
};

struct astnode {
    NodeKind nk;
    union {
        ExprType expr;
        StmtType stmt;
    } type;
    Token token;
    astnode* child[MAX_CHILD];
    astnode* next;
    astnode(NodeKind kind, Token t) : nk(kind), token(t), next(nullptr) { 
        for (int i = 0; i < MAX_CHILD; i++)
            child[i] = nullptr;
    }
};



void preorder(astnode* expr, int d);

astnode* makeExprNode(ExprType type, Token tk);

astnode* makeStmtNode(StmtType type, Token tk);

astnode* copyTree(astnode* node);

void cleanUpTree(astnode* node);

bool isExprType(astnode* node, ExprType type);

bool isStmtType(astnode* node, StmtType type);
#endif