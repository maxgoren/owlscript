#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include "token.hpp"
#include "lexer_matrix.h"
#include "../util/buffer.hpp"
using namespace std;

class Lexer {
    private:
        CharBuffer* buffer;
        bool noisey;
        bool in_comment;
        bool shouldSkip(char ch);
        Token makeLexToken(TKSymbol symbol, char* text, int length);
        Token nextToken();
        int get_next(int state, char p);
    public:
        Lexer(bool debug = false);
        vector<Token> lex(CharBuffer* buffer);
};

#endif