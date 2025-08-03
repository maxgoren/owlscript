#include "util.h"
#include "match.h"

void step_one(char *pattern, char* text) {
    re_ast* ast = re2ast(augmentRE(pattern));
    re_ast** node_table;
    DFA dfa = ast2dfa(pattern, ast, &node_table);
    printDFA(dfa);
    if (simulateDFA(&dfa, text, node_table)) {
        printf("Match Found.\n");
    }
    dfa2json(&dfa);
}


int main(int argc, char* argv[]) {
    step_one(argv[1], argv[2]);
    return 0;   
}