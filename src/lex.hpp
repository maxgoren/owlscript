#ifndef lex_hpp
#define lex_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "tokenstream.hpp"
#include "stringbuffer.hpp"
using namespace std;

enum DFA_STATES {
    START, SCANNING, IN_COMMENT, IN_NUM, IN_ID, IN_STRING, IN_SPECIAL, ERROR, DONE
};
inline string dfastate[] = { 
    "START", "SCANNING", "IN_COMMENT", "IN_NUM", "IN_ID", "IN_STRING", "IN_SPECIAL", "ERROR", "DONE"
};
class Lexer {
    private:
        bool inComment;
        DFA_STATES state;
        unordered_map<string, Symbol> reserved;
        StringBuffer sb;
        Token extractNumber();
        Token extractIdentifier();
        Token extractString();
        Token checkSpecials();
        bool shouldSkip(char c);
        void skipWhiteSpace();
        Token nextToken();
        bool loud;
    public:
        Lexer(bool debug = false);
        void init(string str);
        void init(vector<string> lines);
        TokenStream lex();
};

#endif