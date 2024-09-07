#ifndef AST_BUILDER_HPP
#define AST_BUILDER_HPP
#include <iostream>
#include <vector>
#include "lex.hpp"
#include <unordered_map>
#include "parser.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "resolve.hpp"
using namespace std;

class ASTBuilder {
    private:
        bool loud;
        ResolveScope resolver;
    public:
        ASTBuilder(bool debug = false);
        astnode* build(string str);
        astnode* buildFromFile(string filename);
};

#endif