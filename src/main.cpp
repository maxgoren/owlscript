#include <iostream>
#include "astbuilder.hpp"
#include "twvm.hpp"
using namespace std;

void runScript(string filename, bool trace) {
    ASTBuilder astbuilder(trace);
    TWVM vm(trace);
    vm.exec(astbuilder.buildFromFile(filename));
    if (vm.context().existsInScope("main")) {
        vm.exec(astbuilder.build("main();"));
    }
}


void repl(bool debug) {
    cout<<"[OwlScript 0.6b]"<<endl;
    bool running = true;
    string input;
    ASTBuilder astbuilder(debug);
    TWVM vm(debug);
    int i = 1;
    while (running) {
        cout<<"OwlScript("<<i<<")> ";
        getline(cin, input);
        if (!input.empty()) {
            if (input == "quit") {
                running = false;
            } else {
                astnode* ast = astbuilder.build(input);
                vm.exec(ast);
                cleanUpTree(ast); 
            }
            i++;
        }
    }
    cout<<"[hoot!]"<<endl;
}

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