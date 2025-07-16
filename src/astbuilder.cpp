#include "astbuilder.hpp"
#include "re2dfa/ex/lex.h"
#include "re2dfa/ex/util.h"

ASTBuilder::ASTBuilder(bool debug) {
    loud = debug;
    cre = init_lex_dfa(nr-1);
    dfa = re2dfa(cre->pattern, cre->ast);
}

astnode* ASTBuilder::build(char* str) {
    printf("Lex input\n");
    TKTokenListNode* tks = lex_input(&dfa, str);
    TokenStream ts(tks);
    cout<<"Swapped token streams."<<endl;
    return resolver.resolveScope(parser.parse(ts));
}

astnode* ASTBuilder::build(string str) {
    sb.init(str);
    return build(sb);
}

astnode* ASTBuilder::buildFromFile(string filename) {
    char* buff = slurp_file(filename.data());
    printf("Try this: %s\n", buff);
    return build(buff);
}
