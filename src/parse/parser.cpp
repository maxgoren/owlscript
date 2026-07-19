#include "parser.hpp"


Parser::Parser(bool debug) {
    noisey = debug;
    in_list_consxr = false;
}

astnode* Parser::parse(vector<Token> tokens) {
    init(tokens);
    astnode* p = stmt_list();
    if (noisey)
        preorder(p, 1);
    return p;
}

void Parser::init(vector<Token>& tk) {
    tokens = tk;
    tpos = 0;
}
void Parser::advance() {
    tpos++;
}
bool Parser::done() {
    return lookahead() == TK_EOI;
}
bool Parser::expect(TKSymbol symbol) {
    return symbol == lookahead();
}
void Parser::match(TKSymbol symbol) {
    if (expect(symbol)) {
        advance();
        return;
    }
    cout<<"Mismatched token: "<<current().getString()<<endl;
    advance();
}
Token& Parser::current() {
    return tokens[tpos];
}
TKSymbol Parser::lookahead() {
    return tokens[tpos].getSymbol();
}