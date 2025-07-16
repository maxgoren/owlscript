#ifndef token_hpp
#define token_hpp
#include <iostream>
#include "re2dfa/lex/lex.h"
using namespace std;

using Symbol = TKSymbol;

struct Token {
    Symbol symbol;
    string strval;
    int depth;
    Token(Symbol s = TK_EOI, string st = " ", int d = -1) : symbol(s), strval(st), depth(d) { }
};

void printToken(Token tk);

#endif