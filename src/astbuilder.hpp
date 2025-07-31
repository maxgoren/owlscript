#ifndef astbuilder_hpp
#define astbuilder_hpp
#include "ast.hpp"
#include "parser.hpp"
#include "resolve.hpp"
#include "lexer.hpp"

class ASTBuilder {
    private:
        bool loud;
        Lexer lexer;
        Parser parser;
        ScopeLevelResolver resolver;
    public:
        ASTBuilder(bool debug = false);
        astnode* build(string str);
        astnode* buildFromFile(string filename);
        astnode* build(char* str);
};


#endif