#ifndef resolve_hpp
#define resolve_hpp
#include "ast.hpp"
#include "stack.hpp"
#include <unordered_map>
#include <iostream>
using namespace std;

class ScopeLevelResolver {
    private:
        bool loud;
        typedef unordered_map<string, bool> ScopeMap;
        IndexedStack<ScopeMap> scopes;
        unordered_map<astnode*, int> depthmap;
        void declareVarName(string id);
        void defineVarName(string id);
        void openScope();
        void closeScope();
        void resolveBlockStatement(astnode* node);
        void resolveLetStatement(astnode* node);
        void resolveDefStatement(astnode* node);
        void resolveVariableDepth(astnode* node, string id);
        void resolveExpressionScope(astnode* node);
        void resolveStatementScope(astnode* node);
        void resolve(astnode* node);
    public:
        ScopeLevelResolver(bool debug = false);
        astnode* resolveScope(astnode* node);
};


#endif