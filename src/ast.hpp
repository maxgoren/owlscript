#ifndef ast_hpp
#define ast_hpp
#include <iostream>
#include "lex.hpp"
using namespace std;

enum NodeType {
    EXPR_NODE,
    STMT_NODE
};

enum ExprType {
    CONST_EXPR, ID_EXPR, BINARYOP_EXPR, UNARYOP_EXPR, RELOP_EXPR, ASSIGN_EXPR, 
    FUNC_EXPR, LAMBDA_EXPR, LIST_EXPR, SUBSCRIPT_EXPR, BLESS_EXPR
};

enum StmtType {
    WHILE_STMT, FOR_STMT, PRINT_STMT, LET_STMT, IF_STMT, 
    EXPR_STMT, DEF_STMT, RETURN_STMT, STRUCT_STMT
};

struct astnode {
    NodeType type;
    union {
        ExprType exprType;
        StmtType stmtType;
    };
    Token attributes;
    astnode* next;
    astnode* child[3];
};

astnode* makeExprNode(ExprType type, Token data);
astnode* makeStmtNode(StmtType type, Token data);
void printToken(Token m);
void printNode(astnode* n);
void traverse(astnode* node, void (*pre)(astnode* x), void (*post)(astnode* x));
void nullFunc(astnode* x);

#endif