#ifndef ast_hpp
#define ast_hpp
#include "token.hpp"
#include <iostream>
#include <set>
using namespace std;


enum NodeType {
    EXPRNODE, STMTNODE
};

enum ExprType {
    CONST_EXPR, ID_EXPR, BIN_EXPR, UOP_EXPR, FUNC_EXPR, LAMBDA_EXPR,
    LISTCON_EXPR, SUBSCRIPT_EXPR, LIST_EXPR, BLESS_EXPR, FIELD_EXPR,
    RANGE_EXPR, SETCOMP_EXPR, TERNARY_EXPR, ITERATOR_EXPR
};

const static string exprTypeStr[] = {
    "CONST_EXPR", "ID_EXPR", "BIN_EXPR", "UOP_EXPR", "FUNC_EXPR", "LAMBDA_EXPR",
    "LISTCON_EXPR", "SUBSCRIPT_EXPR", "LIST_EXPR", "BLESS_EXPR", "FIELD_EXPR",
    "RANGE_EXPR", "SETCOMP_EXPR", "TERNARY_EXPR", "ITERATOR_EXPR"
};


enum StmtType {
    PRINT_STMT, WHILE_STMT, FOREACH_STMT, IF_STMT, ELSE_STMT, STMT_LIST, EXPR_STMT,
    LET_STMT, RETURN_STMT, DEF_CLASS_STMT, BLOCK_STMT, IMPORT_STMT
};

const static string stmtTypeStr[] = { 
    "PRINT_STMT", "WHILE_STMT", "FOREACH_STMT", "IF_STMT", "ELSE_STMT", "STMT_LIST", "EXPR_STMT",
    "LET_STMT", "RETURN_STMT", "DEF_CLASS_STMT", "BLOCK_STMT", "IMPORT_STMT"
    
};

struct astnode {
    NodeType kind;
    union {
        ExprType expr;
        StmtType stmt;
    };
    Token token;
    astnode* left;
    astnode* right;
    astnode* next;
    astnode(ExprType et, Token tk) : expr(et), kind(EXPRNODE), token(tk), left(nullptr), right(nullptr), next(nullptr) { }
    astnode(StmtType st, Token tk) : stmt(st), kind(STMTNODE), token(tk), left(nullptr), right(nullptr), next(nullptr) { }
    astnode() : token(Token(TK_EOI, "fin")), left(nullptr), right(nullptr), next(nullptr) { }
};

void preorder(astnode* node, int d);

bool isStmtNode(astnode* ast);

bool isExprNode(astnode* ast);

bool isExprType(astnode* ast, ExprType type);
bool isStmtType(astnode* ast, StmtType type);
#endif