#include <iostream>
#include "owlscript.hpp"
using namespace std;


void parseOptions(int argc, char* argv[]) {
    bool trace_eval = false;
    if (argv[1][0] == '-') {
        switch (argv[1][1]) {
            case 'v': { trace_eval = true; } break;
            default: break;
        }
        if (argc == 3) {
            runScript(argv[2], trace_eval);
        } else repl(trace_eval);
    } else {
        runScript(argv[1], trace_eval);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        repl(false);
    } else {
        parseOptions(argc, argv);
    }
    return 0;
}