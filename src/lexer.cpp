#include "lexer.hpp"

Lexer::Lexer() {
    cre = init_lexer_patterns(nr-1);
    dfa = ast2dfa(cre->pattern, cre->ast, &ast_node_table);
}

TKTokenListNode* Lexer::lex(char* text) {
    return lex_input(&dfa, text, ast_node_table);
}