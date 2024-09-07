#ifndef token_hpp
#define token_hpp
#include <iostream>
using namespace std;

enum Symbol {
    TK_NONE, TK_NUM, TK_REALNUM, TK_ID, TK_STRING, TK_TRUE, TK_FALSE, TK_STRUCT, TK_MAKE,
    TK_LPAREN, TK_RPAREN, TK_LCURLY, TK_RCURLY, TK_LSQUARE, TK_RSQUARE,
    TK_PERIOD, TK_COMMA, TK_QUOTE, TK_LOGIC_NOT, TK_AMPER, TK_LAMBDA, TK_PRODUCE,
    TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_MOD, TK_POW, TK_SQRT, TK_LOGIC_AND, TK_LOGIC_OR,
    TK_LT, TK_GT, TK_LTE, TK_GTE, TK_EQU, TK_NOTEQU,
    TK_ASSIGN, TK_SEMI, TK_COLON,
    TK_LET, TK_DEF, TK_RETURN, TK_WHILE, TK_FOR, TK_VAR,
    TK_IF, TK_ELSE, TK_PRINT, 
    TK_APPEND, TK_PUSH, TK_POP, TK_LENGTH, TK_EMPTY, TK_SORT, TK_FIRST, TK_REST, TK_MAP, 
    TK_OPEN_COMMENT, TK_CLOSE_COMMENT, TK_NIL, TK_PIPE,
    TK_EOF, TK_ERROR
};


inline string symbolAsString[] = {
    "TK_NONE", "TK_NUM", "TK_REALNUM", "TK_ID", "TK_STRING", "TK_TRUE", "TK_FALSE", "TK_STRUCT", "TK_MAKE",
    "TK_LPAREN", "TK_RPAREN", "TK_LCURLY", "TK_RCURLY", "TK_LSQUARE", "TK_RSQUARE",
    "TK_PERIOD", "TK_COMMA", "TK_QUOTE", "TK_LOGIC_NOT", "TK_AMPER", "TK_LAMBDA", "TK_PRODUCE",
    "TK_ADD", "TK_SUB", "TK_MUL", "TK_DIV", "TK_MOD", "TK_POW", "TK_SQRT","TK_LOGIC_AND", "TK_LOGIC_OR",
    "TK_LT", "TK_GT", "TK_LTE", "TK_GTE", "TK_EQU", "TK_NOTEQU",
    "TK_ASSIGN", "TK_SEMI", "TK_COLON",
    "TK_LET", "TK_DEF", "TK_RETURN", "TK_WHILE", "TK_FOR", "TK_VAR",
    "TK_IF", "TK_ELSE", "TK_PRINT", 
    "TK_APPEND", "TK_PUSH", "TK_POP", "TK_LENGTH", "TK_EMPTY", "TK_SORT", "TK_FIRST", "TK_REST", "TK_MAP",
    "TK_OPEN_COMMENT", "TK_CLOSE_COMMENT", "TK_NIL", "TK_PIPE",
    "TK_EOF", "TK_ERROR"
};

struct Token {
    Symbol symbol;
    string strval;
    int lineNumber;  
    int depth;
    Token(Symbol s = TK_NONE, string st = " ") {
        symbol = s;
        strval = st;
        lineNumber = 0;
        depth = -1;
    }
};

bool isRelOp(Symbol s);

#endif