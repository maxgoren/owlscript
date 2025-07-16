#ifndef lex_h
#define lex_h
#ifdef __cplusplus
extern "C" {
#endif
#include "../re2ast/src/ast.h"
#include "../re2ast/src/parser.h"
#include "../src/re_to_dfa.h"

typedef enum TKSymbol {
    TK_ID, TK_NUM, TK_REALNUM, TK_STR, TK_TRUE, TK_FALSE, TK_NIL, TK_LP, TK_RP, TK_LC, TK_RC, TK_LB, TK_RB,
    TK_ADD, TK_MUL, TK_SUB, TK_DIV, TK_MOD, TK_POW, TK_SQRT, TK_POST_INC, TK_POST_DEC, TK_PRE_INC, TK_PRE_DEC,
    TK_LT, TK_GT, TK_LTE, TK_GTE, TK_EQU, TK_NEQ, TK_NOT, TK_AND, TK_OR, TK_BIT_AND, TK_BIT_OR, TK_BIT_XOR, TK_BLESS, 
    TK_BREAK, TK_CONTINUE, TK_PERIOD, TK_COMA, TK_SEMI, TK_COLON, TK_AMPER, TK_QM, TK_REF, TK_LAMBDA, TK_RANGE,
    TK_ASSIGN, TK_ASSIGN_SUM, TK_ASSIGN_DIFF, TK_ASSIGN_PROD, TK_ASSIGN_DIV, 
    TK_QUOTE, TK_FUNC, TK_PRODUCES, TK_STRUCT, TK_NEW, TK_FREE, TK_IN,
    TK_LET, TK_VAR,  TK_PRINT, TK_PRINTLN, TK_WHILE, TK_FOREACH, TK_RETURN, TK_IF, TK_ELSE,
    TK_PUSH, TK_APPEND, TK_EMPTY, TK_SIZE, TK_FIRST, TK_REST, TK_MAP, TK_FILTER, TK_REDUCE,
    TK_REVERSE, TK_SORT, TK_PIPE, TK_MATCHRE, TK_REMATCH, TK_TYPEOF, TK_FOPEN,
    TK_ERR, TK_EOI

} Symbol;

typedef struct {
    char* pattern;
    enum TKSymbol token;
} TokenRule;

typedef struct {
    char* pattern;
    int patlen;
    re_ast* ast;
} CombinedRE;

typedef struct {
    int rule_id;
    int length;
    char* text;
} TKToken;

typedef struct TKTokenListNode_ {
    TKToken* token;
    struct TKTokenListNode_* next;
} TKTokenListNode;

extern int nr;
extern bool in_string;
extern TokenRule rules[]; 

void tag_final_pos_with_token_id(re_ast* ast, int rulenum);
CombinedRE* combine(int numrules);
CombinedRE* init_lex_dfa(int numrules);
TKToken* makeTKToken(int rid, int len);
TKToken* nextToken(DFA* dfa, const char* input);
TKTokenListNode* makeTokenListNode(TKToken* token);
TKTokenListNode* lex_input(DFA* dfa, char* input);

#ifdef __cplusplus
}
#endif

#endif