#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include "ast.hpp"
#include "tokenstream.hpp"
using namespace std;

class Parser {
    private:
        bool inListConstructor;
        TokenStream ts;
        Token& current();
        Token& advance();
        Symbol lookahead();
        bool expect(Symbol sym);
        bool match(Symbol sym);
        astnode* paramList();
        astnode* argList();
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
        astnode* statement();
        astnode* statementList();
        astnode* program();
    public:
        Parser();
        astnode* parse(TokenStream& tokens);
};


#endif