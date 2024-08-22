#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
#include "ast.hpp"
#include "lex.hpp"
using namespace std;

class Parser {
    private:
        bool loud;
        unordered_set<Symbol> listExprs;
        vector<Token> tokens;
        int tpos;
        Token current;
        inline Symbol currSym();
        Symbol lookahead();
        bool match(Symbol s);
        void advance();
        void init(vector<Token> in);
        astnode* makeLetStatement();
        astnode* makePrintStatement();
        astnode* makeExprStatement();
        astnode* makeWhileStatement();
        astnode* makeForStatement();
        astnode* makeIfStatement();
        astnode* makeReturnStatement();
        astnode* makeStructStatement();
        astnode* paramList();
        astnode* argsList();
        astnode* makeDefStatement();
        astnode* program();
        astnode* statementList();
        astnode* statement();
        astnode* simpleExpr();
        astnode* expr();
        astnode* term();
        astnode* factor();
        astnode* var();
        astnode* makeIDExpr();
        astnode* makeConstExpr();
        astnode* makeListExpr();
    public:
        Parser(bool debug = false);
        astnode* parse(vector<Token> in);
};


#endif