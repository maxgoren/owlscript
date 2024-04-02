#ifndef lexer_hpp
#define lexer_hpp
#include "../buffer/stringbuffer.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;

enum TOKENS {
    ERROR, WHITESPACE, EOFTOKEN,
    DEF, NUMBER, STRING, ID, NIL, TRUE, FALSE,
    PLUS, MINUS, MULTIPLY, DIVIDE, MOD, ASSIGN,
    SEMI, COLON, PERIOD, COMA, QUOTE, NOT,
    LSQ, RSQ, LPAREN, RPAREN, LCURLY, RCURLY,
    LESS, GREATER, EQUAL, NOTEQUAL, LTE, GTE,
    IF, ELSE, PRINT, READ, LOOP, RETURN, LAMBDA,
    PUSH, POP, APPEND, LENGTH, SORT, MAP, FIRST, REST,
    CLASS, THIS, HASH
};

inline string tokenString[] = {
    "ERROR", "WHITESPACE", "EOFTOKEN",
    "DEF", "NUMBER", "STRING", "ID", "NIL", "TRUE", "FALSE",
    "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MOD", "ASSIGN",
    "SEMI", "COLON", "PERIOD", "COMA", "QUOTE", "NOT",
    "LSQ", "RSQ", "LPAREN", "RPAREN", "LCURLY", "RCURLY",
    "LESS", "GREATER", "EQUAL", "NOTEQUAL", "LTE", "GTE",
    "IF", "ELSE", "PRINT", "READ", "LOOP", "RETURN", "LAMBDA", 
    "PUSH", "POP", "APPEND", "LENGTH", "SORT", "MAP", "FIRST", "REST",
    "CLASS", "THIS", "HASH"
};

struct Lexeme {
    TOKENS tokenVal;
    string stringVal;
    int lineNumber;
    Lexeme(TOKENS t = ID, string sv = "(empty)", int ln = 0) {
        tokenVal = t;
        stringVal = sv;
        lineNumber = ln;
    }
};

class Lexer {
    private:
        StringBuffer sb;
        unordered_map<string, TOKENS> reservedWords;
        vector<Lexeme> lexemes;
        Lexeme extractWord();
        Lexeme extractNumber();
        Lexeme checkSpecials();
        Lexeme extractStringLiteral();
        vector<Lexeme>& start();
        void initReserved();
    public:
        Lexer();
        vector<Lexeme>& lex(string filename);
        vector<Lexeme>& lexString(string str);
};


#endif