#ifndef lex_h
#define lex_h
#ifdef __cplusplus
extern "C" {
#endif
#include "../src/re2ast/src/ast.h"
#include "../src/re2ast/src/parser.h"
#include "../src/re_to_dfa.h"
#include "lex_token_def.h"

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
    int lineno;
    char* text;
} TKToken;

typedef struct TKTokenListNode_ {
    TKToken* token;
    struct TKTokenListNode_* next;
} TKTokenListNode;

extern int nr;
extern bool in_string;
extern TokenRule rules[];
extern char* symbolStr[];
void tag_final_pos_with_token_id(re_ast* ast, int rulenum);
CombinedRE* combine(int numrules);
CombinedRE* init_lexer_patterns(int numrules);
TKToken* makeTKToken(int rid, int len);
TKToken* nextToken(DFA* dfa, const char* input, re_ast** ast_node_table);
TKTokenListNode* makeTokenListNode(TKToken* token);
TKTokenListNode* lex_input(DFA* dfa, char* input, re_ast** ast_node_table);

#ifdef __cplusplus
}
#endif

#endif