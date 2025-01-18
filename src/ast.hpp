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
    FUNC_EXPR, LAMBDA_EXPR, LIST_EXPR, SUBSCRIPT_EXPR, OBJECT_DOT_EXPR, BLESS_EXPR, REG_EXPR, REF_EXPR,
    RANGE_EXPR, LISTCOMP_EXPR, FILE_EXPR, META_EXPR
};

enum StmtType {
    WHILE_STMT, FOR_STMT, FOREACH_STMT, PRINT_STMT, LET_STMT, IF_STMT, 
    EXPR_STMT, DEF_STMT, RETURN_STMT, STRUCT_STMT, BLOCK_STMT,
    REF_STMT
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
bool isExprNode(astnode* node);
bool isStmtNode(astnode* node);
bool isExprType(astnode* node, ExprType type);
bool isStmtType(astnode* node, StmtType type);
Token& getAttributes(astnode* node);
void printToken(Token m);
void printNode(astnode* n);
void traverse(astnode* node, void (*pre)(astnode* x), void (*post)(astnode* x));
void nullFunc(astnode* x);
void cleanup(astnode* x);

#endif