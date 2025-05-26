#include "astbuilder.hpp"

ASTBuilder::ASTBuilder(bool debug) {
    loud = debug;
}

astnode* ASTBuilder::build(string str) {
    sb.init(str);
    TokenStream ts = lexer.lex(sb);
    if (loud) {
        for (ts.start(); !ts.done(); ts.advance()) {
            printToken(ts.get());
        }
    }
    astnode* ast = parser.parse(ts);
    if (loud) {
        preorder(ast, 0);
    }
    return resolver.resolveScope(ast);
}

astnode* ASTBuilder::buildFromFile(string filename) {
    sb.readFromFile(filename);
    TokenStream ts = lexer.lex(sb);
    astnode* ast = parser.parse(ts);
    return resolver.resolveScope(ast);
}
