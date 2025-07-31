#ifndef augmented_ast_h
#define augmented_ast_h

#ifdef __cplusplus
extern "C" {
#endif

#include "re2ast/src/ast.h"
#include "intset.h"

/*

A Regular Expression is Augmented with a special "END" marker - '#'.
    re   ->  (re)#

An AST is then built for the augmented re.

The nodes are numbered, and the we calculate the following sets:

Firstpos:
Identifies the set of positions (leaf nodes) that can be the first symbol in a string derived from that node.

Lastpos:
Identifies the set of positions that can be the last symbol in a string derived from that node.

Followpos:
Defines how positions relate to each other during DFA state transitions. It determines which positions can follow a given position in a string. 

nullable: returns true for nodes which can represent the empty set (epsilon)


From the Followpos table, we can build a DFA to recognize the provided regular expression.


typedef struct {
    int numleaves;
    int numnodes;
    re_ast** node_table;
} ASTNodeTable;

void initNodeTable(ASTNodeTable* ant, int total, int leaves) {
    ant->numnodes = total;
    ant->numleaves = leaves;
    ant->node_table = (re_ast**)malloc(sizeof(re_ast*)*(total+1));
    for (int i = 0; i < total; i++) 
        ant->node_table[i] = NULL;
}
*/

extern int numleaves;
extern int nonleaves;

int isLeaf(re_ast* node);

void number_nodes(re_ast* node, int pass, re_ast** node_table);

bool nullable(re_ast* node);

void initAttributeSets(int size, re_ast** node_table);

void mergeFirstPos(re_ast* node);

void mergeLastPos(re_ast* node);

void calcFirstandLastPos(re_ast* node, re_ast** node_table);

void calcFollowPos(re_ast* node, re_ast** node_table);

void computeFollowPos(re_ast* node, re_ast*** node_table);

#ifdef __cplusplus
}
#endif

#endif