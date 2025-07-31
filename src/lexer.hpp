#ifndef lexer_hpp
#define lexer_hpp
#include "re2dfa/lex/lex.h"
#include "re2dfa/lex/util.h"

//C++ interface to C Lexer
struct Lexer {
    CombinedRE* cre;
    re_ast** ast_node_table;
    DFA dfa;
    Lexer();
    TKTokenListNode* lex(char* text);
};


#endif