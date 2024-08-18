#include "ast_builder.hpp"


ASTBuilder::ASTBuilder(bool debug) : loud(debug) {

}

astnode* ASTBuilder::build(string str) {
    Lexer l(loud);
    Parser p(loud);
    l.init(str);
    auto m = l.lex();
    if (loud) {
        for (auto t : m)
            printToken(t);
    }
    return p.parse(m);
}

astnode* ASTBuilder::buildFromFile(string filename) {
    FileBuffer fb;
    Lexer l;
    Parser p;
    auto q = fb.readFile(filename);
    l.init(q);
    auto m = l.lex();
    if (loud) {
        for (auto t : m)
            printToken(t);
    }
    return p.parse(m);

}