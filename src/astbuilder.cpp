#include "astbuilder.hpp"

ASTBuilder::ASTBuilder(bool debug) {
    loud = debug;
}

astnode* ASTBuilder::build(StringBuffer& sb) {
    TokenStream ts = lexer.lex(sb);
    astnode* ast = parser.parse(ts);
    if (loud) {
        preorder(ast, 0);
    }
    return resolver.resolveScope(ast);
}

astnode* ASTBuilder::build(string str) {
    sb.init(str);
    return build(sb);
}

astnode* ASTBuilder::buildFromFile(string filename) {
    sb.readFromFile(filename);
    return build(sb);
}
