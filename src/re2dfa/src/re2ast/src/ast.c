#include "ast.h"

re_ast* makeNode(int type, REToken tk) {
    re_ast* node = malloc(sizeof(re_ast));
    node->type = type;
    node->token = tk;
    node->tk_token_id = -1;
    node->firstpos = NULL;
    node->lastpos = NULL;
    node->followpos = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void printAST(re_ast* node, int d) {
    if (node != NULL) {
        for (int i = 0; i < d; i++) printf("  ");
        if (node->token.symbol == RE_CCL) {
            printf("<%s, %s> {%d}\n", RESymbolStr[node->token.symbol], node->token.ccl, node->number);
        } else {
            printf("<%s, %c> {%d}\n", RESymbolStr[node->token.symbol], node->token.ch, node->number);
        }
        printAST(node->left, d+1);
        printAST(node->right, d+1);
    }
}

re_ast* cloneTree(re_ast* node) {
    if (node == NULL)
        return NULL;
    re_ast* t = makeNode(node->type, node->token);
    t->type = node->type;
    t->token = node->token;
    t->left = cloneTree(node->left);
    t->right = cloneTree(node->right);
    return t;
}

void freeTree(re_ast* node) {
    if (node != NULL) {
        freeTree(node->left);
        freeTree(node->right);
        free(node);
    }
}