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
        ASTBuilder(bool debug = false) {
            loud = debug;
        }
        astnode* build(string str) {
            Lexer l(loud);
            Parser p(loud);
            auto m = l.lex(str);
            if (loud) {
                for (auto t : m)
                    printToken(t);
            }
            return p.parse(m);
        }
};

#endif