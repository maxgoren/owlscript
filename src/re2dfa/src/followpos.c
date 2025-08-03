#include "followpos.h"

int isLeaf(re_ast* node) {
    return (node->left == NULL && node->right == NULL);
}

void number_nodes(re_ast* node, int pass, re_ast** ast_node_table) {
    if (node != NULL) {
        number_nodes(node->left, pass, ast_node_table);
        number_nodes(node->right, pass, ast_node_table);
        if (isLeaf(node)) {
            if (pass == 1) {
                node->number = ++numleaves;
                ast_node_table[node->number] = node;
            }
        } else {
            if (pass == 2) {
                node->number = ++nonleaves;
                ast_node_table[node->number] = node;
            }
        }
    }
}

bool nullable(re_ast* node) {
    if (node == NULL)
        return false;
    if (isLeaf(node) && node->token.symbol == RE_EPSILON)
        return true;
    if (isLeaf(node) && (node->token.symbol == RE_CHAR || node->token.symbol == RE_PERIOD))
        return false;
    if (node->token.symbol == RE_OR)
        return nullable(node->left) || nullable(node->right);
    if (node->token.symbol == RE_CONCAT) {
        return nullable(node->left) && nullable(node->right);
    }
    if (node->token.symbol == RE_STAR)
        return true;
    return false;
}
int numleaves;
int nonleaves;


void initAttributeSets(int size, re_ast** ast_node_table) {
    for (int i = 0; i < size+1; i++) {
        if (ast_node_table[i] != NULL) {
            ast_node_table[i]->firstpos = createSet(size+1);
            ast_node_table[i]->lastpos = createSet(size+1);
            ast_node_table[i]->followpos = createSet(size+1);
        }
    }
}

void mergeFirstPos(re_ast* node) {
    if (node->left) {
        node->firstpos = setUnion(node->firstpos, node->left->firstpos);
    }
    if (node->right) {
        node->firstpos = setUnion(node->firstpos, node->right->firstpos);
    }
}

void mergeLastPos(re_ast* node) {
    if (node->left) {
        node->lastpos = setUnion(node->lastpos, node->left->lastpos);
    }
    if (node->right) {
        node->lastpos = setUnion(node->lastpos, node->right->lastpos);
    }
}

/*

Firstpos:
Identifies the set of positions (leaf nodes) that can be the first symbol in a string derived from that node.
Lastpos:
Identifies the set of positions that can be the last symbol in a string derived from that node.

*/

void calcFirstandLastPos(re_ast* node, re_ast** ast_node_table) {
    if (node != NULL) {
        calcFirstandLastPos(node->left, ast_node_table);
        calcFirstandLastPos(node->right, ast_node_table);
        if (isLeaf(node)) {
            setAdd(node->firstpos, node->number);
            setAdd(node->lastpos, node->number);
        } else if (node->token.symbol == RE_OR) {
            mergeFirstPos(node);
            mergeLastPos(node);
        } else if (node->token.symbol == RE_CONCAT) {
            if (nullable(node->left)) {
                mergeFirstPos(node);
            } else if (node->left) {
                node->firstpos = setUnion(node->firstpos, node->left->firstpos);
            }
            if (nullable(node->right)) {
                mergeLastPos(node);
            } else if (node->right) {
                node->lastpos = setUnion(node->lastpos, node->right->lastpos);
            }
        } else if (node->token.symbol == RE_STAR && node->left) {
            node->firstpos = setUnion(node->firstpos, node->left->firstpos);
            node->lastpos = setUnion(node->lastpos, node->left->lastpos);
        }
    }
}

/*
Followpos:
Defines how positions relate to each other during DFA state transitions. It determines which positions can follow a given position in a string. 
*/

void calcFollowPos(re_ast* node, re_ast** ast_node_table) {
    if (node == NULL)
        return;
    if (node->token.symbol == RE_CONCAT) {
        if (!node->left || !node->right) {
           // printf("ERROR: RE_CONCAT node %d/%d missing children (left=%p, right=%p)\n",
           //       node->number, nonleaves, (void*)node->left, (void*)node->right);
           //die quietly, seems not to matter. 
           return;
        }
    }
    calcFollowPos(node->left, ast_node_table);
    calcFollowPos(node->right, ast_node_table);
    if (node->token.symbol == RE_CONCAT) {
        if (node->left == NULL || node->right == NULL)
            return;
        for (int i = 0; i < node->left->lastpos->n; i++) {
            int t = node->left->lastpos->members[i];
            ast_node_table[t]->followpos = setUnion(ast_node_table[t]->followpos, node->right->firstpos);
        }
    }
    if (node->token.symbol == RE_STAR) {
        for (int i = 0; i < node->lastpos->n; i++) {
            int t = node->lastpos->members[i];
            ast_node_table[t]->followpos = setUnion(ast_node_table[t]->followpos, node->firstpos);
        }
    }
}

void computeFollowPos(re_ast* node, re_ast*** ast_node_table) {
    *ast_node_table = malloc(sizeof(re_ast*)*(MAX_NODE_COUNT));
    numleaves = 0;
    number_nodes(node, 1, *ast_node_table);
    nonleaves = numleaves;
    number_nodes(node, 2, *ast_node_table);
    initAttributeSets(nonleaves+1, *ast_node_table);
    calcFirstandLastPos(node, *ast_node_table);
    calcFollowPos(node, *ast_node_table);
}