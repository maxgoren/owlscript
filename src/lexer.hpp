#ifndef lexer_hpp
#define lexer_hpp
#include <vector>
#include <unordered_map>
#include "token.hpp"
#include "stack.hpp"
#include "tokenstream.hpp"
#include "stringbuffer.hpp"
using namespace std;

class Lexer {
    private:
        IndexedStack<char> balanceStack;
        unordered_map<string, Token> reserved;
        void skipWhiteSpace(StringBuffer& sb);
        void skipComments(StringBuffer& sb);
        Token extractNumber(StringBuffer& sb);
        Token extractId(StringBuffer& sb);
        Token extractString(StringBuffer& sb);
        Token checkReserved(string id);
        Token checkSpecials(StringBuffer& sb);
    public:
        Lexer();
        TokenStream lex(StringBuffer sb);
};



#endif