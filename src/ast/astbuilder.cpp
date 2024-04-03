#include "astbuilder.hpp"

ASTBuilder::ASTBuilder(bool loud) {
    trace = loud;
}

ASTNode* ASTBuilder::build(string text, bool tracing) {
    Parser parser(tracing);
    auto tokens = lexer.lexString(text);
    if (tracing) {
        for (auto m : tokens) {
            cout<<m.lineNumber<<": <"<<tokenString[m.tokenVal]<<", "<<m.stringVal<<">"<<endl;
        }
    }
    return parser.parse(tokens);
}

ASTNode* ASTBuilder::fromFile(string filename) {
    Parser parser(trace);
    return parser.parse(lexer.lex(filename));
}