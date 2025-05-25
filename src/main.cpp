#include <iostream>
#include "astbuilder.hpp"
#include "twvm.hpp"
using namespace std;

void runScript(string filename) {
    ASTBuilder astbuilder;
    TWVM vm(false);
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
        cout<<"OwlScript("<<i++<<")> ";
        getline(cin, input);
        if (input == "quit") {
            running = false;
        } else {
            astnode* ast = astbuilder.build(input);
            vm.exec(ast);
            cleanUpTree(ast); 
        }
    }
    cout<<"[hoot!]"<<endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        repl(false);
    } else {
        if (argv[1][0] != '-')
            runScript(argv[1]);
        else repl(true);
    }
    return 0;
}