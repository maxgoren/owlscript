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
        ASTBuilder(bool loud = false) {
            trace = loud;
        }
        ASTNode* build(string text) {
            Lexer lexer;
            Parser parser(trace);
            return parser.parse(lexer.lexString(text));
        }
        ASTNode* fromFile(string filename) {
            Lexer lexer;
            Parser parser(trace);
            return parser.parse(lexer.lex(filename));
        }
};

#endif