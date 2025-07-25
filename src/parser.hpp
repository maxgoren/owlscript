#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <unordered_set>
#include "ast.hpp"
#include "tokenstream.hpp"
using namespace std;

class Parser {
    private:
        unordered_set<Symbol> twoArgListOps;
        unordered_set<Symbol> oneArgListOps;
        bool inListConstructor;
        TokenStream ts;
        Token& current();
        Token& advance();
        Token _currTok;
        Symbol lookahead();
        bool expect(Symbol sym);
        bool match(Symbol sym);
        astnode* paramList();
        astnode* argList();
        astnode* builtIns();
        astnode* primary();
        astnode* val();
        astnode* unopExpression();
        astnode* factor();
        astnode* term();
        astnode* range();
        astnode* relOpExpression();
        astnode* equOpExpression();
        astnode* logicOpExpression();
        astnode* bitwiseAnd();
        astnode* bitwiseOr();
        astnode* bitwiseXor();
        astnode* expression();
        astnode* makeBlock();
        astnode* makeBlockStatement();
        astnode* ifStatement();
        astnode* whileStatement();
        astnode* foreachStatement();
        astnode* funcDefStatement();
        astnode* structDefStatement();
        astnode* letStatement();
        astnode* printStatement();
        astnode* returnStatement();
        astnode* statement();
        astnode* statementList();
        astnode* program();
    public:
        Parser();
        astnode* parse(TokenStream& tokens);
};


#endif