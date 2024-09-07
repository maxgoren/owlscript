#include "ast_builder.hpp"

ASTBuilder::ASTBuilder(bool debug) : loud(debug) {

}

astnode* ASTBuilder::build(string str) {
    Lexer l(false);
    Parser p(loud);
    l.init(str);
    auto m = l.lex();
    if (loud) {
        for (auto t : m)
            printToken(t);
    }
    astnode* ast = p.parse(m);
    resolver.resolveScope(ast);
    return ast;
}

astnode* ASTBuilder::buildFromFile(string filename) {
    FileBuffer fb;
    Lexer l(false);
    Parser p(loud);
    auto q = fb.readFile(filename);
    l.init(q);
    auto m = l.lex();
    /*if (loud) {
        for (auto t : m)
            printToken(t);
    }*/
    auto ast = p.parse(m);
    resolver.resolveScope(ast);
    return ast;
}