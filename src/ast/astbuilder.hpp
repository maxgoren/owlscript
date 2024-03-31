#ifndef astbuilder_hpp
#define astbuilder_hpp
#include "ast.hpp"
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"
#include <iostream>
using std::string;

class ASTBuilder {
    private:
        bool trace;
    public:
        ASTBuilder(bool loud = false);
        ASTNode* build(string text, bool tracing);
        ASTNode* fromFile(string filename);
};

#endif