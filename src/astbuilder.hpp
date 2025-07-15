#ifndef astbuilder_hpp
#define astbuilder_hpp
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "stringbuffer.hpp"
#include "resolve.hpp"

class ASTBuilder {
    private:
        bool loud;
        StringBuffer sb;
        CombinedRE* cre;
        DFA dfa;
        Lexer lexer;
        Parser parser;
        ScopeLevelResolver resolver;
    public:
        ASTBuilder(bool debug = false);
        astnode* build(StringBuffer& sb);
        astnode* build(string str);
        astnode* buildFromFile(string filename);
        astnode* build(char* str);
};


#endif