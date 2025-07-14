#include "astbuilder.hpp"
#include "re2dfa/ex/lex.h"


ASTBuilder::ASTBuilder(bool debug) {
    loud = debug;
}

astnode* ASTBuilder::build(char* str) {
    printf("Get stream...\n");
    TKTokenListNode* tks = lex_input(str);
    printf("Ok, no re-stream\n");
    TokenStream ts;
    for (auto it = tks; it != NULL; it = it->next) {
        cout<<"Append: "<<symbolStr[rules[it->token->rule_id].token]<<" as: "<<it->token->text<<endl;
        ts.append(Token(rules[it->token->rule_id].token, it->token->text));
    }
    cout<<"Swapped token streams."<<endl;
    return resolver.resolveScope(parser.parse(ts));
}

astnode* ASTBuilder::build(StringBuffer& sb) {
    TokenStream ts = lexer.lex(sb);
    astnode* ast = parser.parse(ts);
    if (loud) {
        preorder(ast, 0);
    }
    return resolver.resolveScope(ast);
}

astnode* ASTBuilder::build(string str) {
    sb.init(str);
    return build(sb);
}

astnode* ASTBuilder::buildFromFile(string filename) {
    sb.readFromFile(filename);
    return build(sb);
}
