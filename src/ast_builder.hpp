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
        Resolve resolver;
    public:
        ASTBuilder(bool debug = false);
        astnode* build(Context& cxt, string str);
        astnode* buildFromFile(Context& cxt, string filename);
};

#endif