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
        ASTNode* build(string text, bool tracing) {
            Lexer lexer;
            Parser parser(tracing);
            auto tokens = lexer.lexString(text);
            if (tracing) {
                for (auto m : tokens) {
                    cout<<m.lineNumber<<": <"<<tokenString[m.tokenVal]<<", "<<m.stringVal<<">"<<endl;
                }
            }
            return parser.parse(tokens);
        }
        ASTNode* fromFile(string filename) {
            Lexer lexer;
            Parser parser(trace);
            return parser.parse(lexer.lex(filename));
        }
};

#endif