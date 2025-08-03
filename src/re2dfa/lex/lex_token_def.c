#include "lex_token_def.h"
#include "lex.h"

//for debugging
char* symbolStr[] = {
    "TK_ID", "TK_NUM", "TK_REALNUM", "TK_STR", "TK_TRUE", "TK_FALSE", "TK_NIL", "TK_LP", "TK_RP", "TK_LC", "TK_RC", "TK_LB", "TK_RB",
    "TK_ADD", "TK_MUL", "TK_SUB", "TK_DIV", "TK_MOD", "TK_POW", "TK_SQRT", "TK_POST_INC", "TK_POST_DEC", "TK_PRE_INC", "TK_PRE_DEC",
    "TK_LT", "TK_GT", "TK_LTE", "TK_GTE", "TK_EQU", "TK_NEQ", "TK_NOT", "TK_AND", "TK_OR", 
    "TK_BIT_AND", "TK_BIT_OR", "TK_BIT_XOR", "TK_BLESS", "TK_BREAK", "TK_CONTINUE",
    "TK_PERIOD", "TK_COMA", "TK_COLON", "TK_SEMI", "TK_QM", "TK_REF","TK_LAMBDA", "TK_RANGE",
    "TK_ASSIGN", "TK_ASSIGN_SUM", "TK_ASSIGN_DIFF", "TK_ASSIGN_PROD", "TK_ASSIGN_DIV", 
    "TK_QUOTE", "TK_FUNC", "TK_PRODUCES", "TK_STRUCT", "TK_NEW", "TK_FREE", "TK_IN",
    "TK_LET", "TK_VAR", "TK_PRINT", "TK_PRINTLN", "TK_WHILE", "TK_FOREACH", "TK_RETURN", "TK_IF", "TK_ELSE",
    "TK_PUSH", "TK_APPEND", "TK_EMPTY", "TK_SIZE", "TK_FIRST", "TK_REST", "TK_MAP", "TK_FILTER", 
    "TK_REDUCE", "TK_REVERSE", "TK_SORT", "TK_MATCHRE", "TK_REMATCH", "TK_TYPEOF", "TK_FOPEN",
    "TK_ERR", "TK_EOI"
};