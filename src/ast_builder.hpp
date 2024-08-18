#ifndef AST_BUILDER_HPP
#define AST_BUILDER_HPP
#include <iostream>
#include <vector>
#include "lex.hpp"
#include "parser.hpp"
#include "ast.hpp"
using namespace std;

class ASTBuilder {
    private:
        bool loud;
    public:
        ASTBuilder(bool debug = false);
        astnode* build(string str);
        astnode* buildFromFile(string filename);
};

#endif