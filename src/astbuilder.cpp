#include "astbuilder.hpp"
#include "re2dfa/lex/lex.h"
#include "re2dfa/lex/util.h"

ASTBuilder::ASTBuilder(bool debug) {
    loud = debug;
    cre = init_lex_dfa(nr-1);
    dfa = re2dfa(cre->pattern, cre->ast);
    if (debug)
        printDFA(dfa);
}

astnode* ASTBuilder::build(char* str) {
    TKTokenListNode* tks = lex_input(&dfa, str);
    TokenStream ts(tks);
    return resolver.resolveScope(parser.parse(ts));
}

astnode* ASTBuilder::build(string str) {
    return build(str.data());
}

astnode* ASTBuilder::buildFromFile(string filename) {
    char* buff = slurp_file(filename.data());
    return build(buff);
}
