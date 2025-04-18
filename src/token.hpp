#ifndef token_hpp
#define token_hpp
#include <iostream>
using namespace std;

enum Symbol {
    TK_NONE, TK_NUM, TK_REALNUM, TK_ID, TK_STRING, 
    TK_TRUE, TK_FALSE, TK_NIL, TK_STRUCT, 
    TK_MAKE,TK_LPAREN, TK_RPAREN, 
    TK_LCURLY, TK_RCURLY, TK_LSQUARE, TK_RSQUARE,
    TK_PERIOD, TK_COMMA, TK_QUOTE, TK_LOGIC_NOT, 
    TK_AMPER, TK_LAMBDA, TK_PRODUCE,
    TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_MOD, TK_POW, TK_SQRT, 
    TK_LOGIC_AND, TK_LOGIC_OR,
    TK_BIT_AND, TK_BIT_OR, TK_BIT_XOR,
    TK_LT, TK_GT, TK_LTE, TK_GTE, TK_EQU, TK_NOTEQU, TK_ASSIGN, 
    TK_POST_INC, TK_PRE_INC, TK_POST_DEC, TK_PRE_DEC, TK_SEMI, 
    TK_COLON, TK_QMARK, TK_KVPAIR, 
    TK_LET, TK_VAR, TK_DEF, TK_RETURN, TK_WHILE, TK_FOR, TK_IF, TK_ELSE,
    TK_REF, TK_INOUT, TK_LAZY, 
    TK_PRINT, TK_MATCH, TK_RANGE, TK_ELIPSE, TK_OF, 
    TK_APPEND, TK_PUSH, TK_POP, TK_SHIFT, TK_UNSHIFT, 
    TK_LENGTH, TK_EMPTY, TK_SORT, TK_FIRST, TK_REST, TK_MAP, TK_FILTER,
    TK_OPEN_COMMENT, TK_CLOSE_COMMENT, 
    TK_PIPE, TK_FOPEN, TK_EVAL, 
    TK_TYPEOF, TK_EOF, TK_ERROR
};


inline string symbolAsString[] = {
    "TK_NONE", "TK_NUM", "TK_REALNUM", "TK_ID", "TK_STRING", 
    "TK_TRUE", "TK_FALSE", "TK_NIL", "TK_STRUCT", 
    "TK_MAKE", "TK_LPAREN", "TK_RPAREN", 
    "TK_LCURLY", "TK_RCURLY", "TK_LSQUARE", "TK_RSQUARE",
    "TK_PERIOD", "TK_COMMA", "TK_QUOTE", "TK_LOGIC_NOT", 
    "TK_AMPER", "TK_LAMBDA", "TK_PRODUCE",
    "TK_ADD", "TK_SUB", "TK_MUL", "TK_DIV", "TK_MOD", "TK_POW", "TK_SQRT",
    "TK_LOGIC_AND", "TK_LOGIC_OR",
    "TK_BIT_AND", "TK_BIT_OR", "TK_BIT_XOR",
    "TK_LT", "TK_GT", "TK_LTE", "TK_GTE", 
    "TK_EQU", "TK_NOTEQU","TK_ASSIGN", 
    "TK_POST_INC", "TK_PRE_INC", 
    "TK_POST_DEC", "TK_PRE_DEC", 
    "TK_SEMI", "TK_COLON", "TK_QMARK", 
    "TK_KVPAIR",
    "TK_LET", "TK_VAR", "TK_DEF", "TK_RETURN", 
    "TK_WHILE", "TK_FOR", "TK_IF", "TK_ELSE", 
    "TK_REF", "TK_INOUT", "TK_LAZY",
    "TK_PRINT", "TK_MATCH", 
    "TK_RANGE", "TK_ELIPSE", 
    "TK_OF", 
    "TK_APPEND", "TK_PUSH", "TK_POP", "TK_SHIFT", "TK_UNSHIFT", "TK_LENGTH", "TK_EMPTY", "TK_SORT", "TK_FIRST", "TK_REST", "TK_MAP", "TK_FILTER",
    "TK_OPEN_COMMENT", "TK_CLOSE_COMMENT", 
    "TK_PIPE", "TK_FOPEN", "TK_EVAL", 
    "TK_TYPEOF", "TK_EOF", "TK_ERROR"
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
bool isEqualityOp(Symbol s);
#endif