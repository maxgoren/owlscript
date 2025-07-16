#ifndef lex_spec_h
#define lex_spec_h
#ifdef __cplusplus
extern "C" {
#endif
#include "lex.h"

//This file is used for specifying TokenRules
//TokenRules are a regex pattern, and an associated TKSymbol
//when DFA recognizes pattern, the TKSymbol is assigned 
//to the recognized string.


//For outputting symbol names as string
char* symbolStr[] = {
    "TK_ID", "TK_NUM", "TK_REALNUM", "TK_STR", "TK_TRUE", "TK_FALSE", "TK_NIL", "TK_LP", "TK_RP", "TK_LC", "TK_RC", "TK_LB", "TK_RB",
    "TK_ADD", "TK_MUL", "TK_SUB", "TK_DIV", "TK_MOD", "TK_POW", "TK_SQRT", "TK_POST_INC", "TK_POST_DEC", "TK_PRE_INC", "TK_PRE_DEC",
    "TK_LT", "TK_GT", "TK_LTE", "TK_GTE", "TK_EQU", "TK_NEQ", "TK_NOT", "TK_AND", "TK_OR", 
    "TK_BIT_AND", "TK_BIT_OR", "TK_BIT_XOR", "TK_BLESS", "TK_BREAK", "TK_CONTINUE",
    "TK_PERIOD", "TK_COMA", "TK_SEMI", "TK_COLON", "TK_AMPER", "TK_QM", "TK_REF","TK_LAMBDA", "TK_RANGE",
    "TK_ASSIGN", "TK_ASSIGN_SUM", "TK_ASSIGN_DIFF", "TK_ASSIGN_PROD", "TK_ASSIGN_DIV", 
    "TK_QUOTE", "TK_FUNC", "TK_PRODUCES", "TK_STRUCT", "TK_NEW", "TK_FREE", "TK_IN",
    "TK_LET", "TK_VAR", "TK_PRINT", "TK_PRINTLN", "TK_WHILE", "TK_FOREACH", "TK_RETURN", "TK_IF", "TK_ELSE",
    "TK_PUSH", "TK_APPEND", "TK_EMPTY", "TK_SIZE", "TK_FIRST", "TK_REST", "TK_MAP", "TK_FILTER", 
    "TK_REDUCE", "TK_REVERSE", "TK_SORT", "TK_PIPE", "TK_MATCHRE", "TK_REMATCH", "TK_TYPEOF", "TK_FOPEN",
    "TK_ERR", "TK_EOI"
};


int nr = 73;

TokenRule rules[] = {
    {"(\\d+)\\.(\\d+)",TK_REALNUM},
    {"[0-9]+", TK_NUM},
    {"if", TK_IF},
    {"in", TK_IN},
    {"let", TK_LET},
    {"var", TK_LET},
    {"def", TK_FUNC},
    {"map", TK_MAP},
    {"nil", TK_NIL},
    {"else",TK_ELSE},
    {"push", TK_PUSH},
    {"size", TK_SIZE},
    {"sort", TK_SORT},
    {"rest", TK_REST},
    {"true", TK_TRUE},
    {"false", TK_FALSE},
    {"break", TK_BREAK},
    {"fopen", TK_FOPEN},
    {"first", TK_FIRST},
    {"while", TK_WHILE},
    {"print", TK_PRINT},
    {"empty", TK_EMPTY},
    {"bless", TK_BLESS},
    {"lambda", TK_LAMBDA},
    {"filter", TK_FILTER},
    {"reduce", TK_REDUCE},
    {"return", TK_RETURN},
    {"struct", TK_STRUCT},
    {"typeOf", TK_TYPEOF},
    {"append", TK_APPEND},
    {"matchre", TK_MATCHRE},
    {"println", TK_PRINTLN},
    {"foreach", TK_FOREACH},
    {"reverse", TK_REVERSE},
    {"+=", TK_ASSIGN_SUM},
    {"[A-Za-z][A-Za-z0-9_]*", TK_ID},
    {"\\\".*\\\"", TK_STR},
    {"\\(", TK_LP},
    {"\\)", TK_RP},
    {"\\[", TK_LB},
    {"\\]", TK_RB},
    {"{", TK_LC},
    {"}", TK_RC},
    {",", TK_COMA},
    {";", TK_SEMI},
    {"!",TK_NOT},
    {"&", TK_BIT_AND},
    {"\\|", TK_BIT_OR},
    {"&&", TK_AND},
    {"\\|\\|", TK_OR},
    {"\\+", TK_ADD},
    {"\\+\\+", TK_POST_INC},
    {"\\+=", TK_ASSIGN_SUM},
    {"-", TK_SUB},
    {"--", TK_POST_DEC},
    {"-=", TK_ASSIGN_DIFF},
    {"\\*", TK_MUL},
    {"\\*=", TK_ASSIGN_PROD},
    {"/", TK_DIV},
    {"%", TK_MOD},
    {"<", TK_LT},
    {">", TK_GT},
    {"=~", TK_REMATCH},
    {"==", TK_EQU},
    {"!=", TK_NEQ},
    {"<=", TK_LTE},
    {">=", TK_GTE},
    {":=", TK_ASSIGN},
    {"->", TK_PRODUCES},
    {"&\\(", TK_LAMBDA},
    {"\\.", TK_PERIOD},
    {"\\.\\.", TK_RANGE},
    {"<fin.>", TK_EOI}
};

#ifdef __cplusplus
}
#endif

#endif