#ifndef resolve_hpp
#define resolve_hpp
#include <iostream>
#include <unordered_map>
#include "ast.hpp"
#include "object.hpp"
#include "context.hpp"
using namespace std;

/*
enum ExprType {
    CONST_EXPR, ID_EXPR, BINARYOP_EXPR, UNARYOP_EXPR, RELOP_EXPR, ASSIGN_EXPR, 
    FUNC_EXPR, LAMBDA_EXPR, LIST_EXPR, SUBSCRIPT_EXPR, BLESS_EXPR
};

enum StmtType {
    WHILE_STMT, FOR_STMT, PRINT_STMT, LET_STMT, IF_STMT, 
    EXPR_STMT, DEF_STMT, RETURN_STMT, STRUCT_STMT, BLOCK_STMT
};

*/


class ResolveScope {
    private:
        bool loud;
        typedef unordered_map<string, bool> ScopeMap;
        IndexedStack<ScopeMap> scopes;
        unordered_map<astnode*, int> depthmap;
        void declareVarName(string id);
        void defineVarName(string id);
        void openScope();
        void closeScope();
        void resolveVariableDepth(astnode* node, string id);
        void resolveExpressionScope(astnode* node);
        void resolveStatementScope(astnode* node);
        void resolve(astnode* node);
    public:
        ResolveScope(bool debug = false);
        void resolveScope(astnode* node);
};

#endif