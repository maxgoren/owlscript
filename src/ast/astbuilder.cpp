#include "astbuilder.hpp"

ASTBuilder::ASTBuilder(bool loud) {
    trace = loud;
}

ASTNode* ASTBuilder::build(string text, bool tracing) {
    Parser parser(tracing);
    auto tokens = lexer.lexString(text);
    return parser.parse(tokens);
}

ASTNode* ASTBuilder::fromFile(string filename) {
    Parser parser(trace);
    return parser.parse(lexer.lex(filename));
}