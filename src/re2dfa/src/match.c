#include <stdio.h>
#include <stdlib.h>
#include "re_to_dfa.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <re> <text>\n", argv[0]);
        return 0;
    }
    if (matchDFA(argv[1], argv[2])) {
        printf("Match found.\n");
    }
    return 0;
}