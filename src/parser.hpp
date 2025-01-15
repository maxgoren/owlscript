#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
#include "ast.hpp"
#include "lex.hpp"
#include "tokenstream.hpp"
using namespace std;

class Parser {
    private:
        bool loud;
        unordered_set<Symbol> listExprs;
        unordered_set<Symbol> constExprs;
        unordered_set<Symbol> builtInExprs;
        TokenStream ts;
        Token current;
        inline Symbol currSym();
        bool match(Symbol s);
        void advance();
        void init(TokenStream& in);
        Lexer lexer;
        astnode* makeLetStatement();
        astnode* makeBlock();
        astnode* makeBlockStatement();
        astnode* makePrintStatement();
        astnode* makeExprStatement();
        astnode* makeWhileStatement();
        astnode* makeForStatement();
        astnode* makeIfStatement();
        astnode* makeReturnStatement();
        astnode* makeStructStatement();
        astnode* makeDefStatement();
        astnode* paramList();
        astnode* argsList();
        astnode* program();
        astnode* statementList();
        astnode* statement();
        astnode* simpleExpr();
        astnode* compExpr();
        astnode* expr();
        astnode* term();
        astnode* factor();
        astnode* var();
        astnode* range();
        astnode* subscript();
        astnode* primary();
        astnode* makeBultInsExpr();
        astnode* makeConstExpr();
        astnode* makeListExpr();
        astnode* makeLambdaExpr();
    public:
        Parser(bool debug = false);
        astnode* parse(TokenStream& in);
};


#endif