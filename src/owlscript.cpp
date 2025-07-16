#include <iostream>
#include "owlscript.hpp"
#include "astbuilder.hpp"
#include "twvm/twvm.hpp"
using namespace std;

void runScript(string filename, bool trace) {
    ASTBuilder astbuilder(trace);
    TWVM vm(trace);
    vm.exec(astbuilder.buildFromFile(filename));
    if (vm.context().existsInScope("main")) {
        vm.exec(astbuilder.build(string("main();").data()));
    }
}


void repl(bool debug) {
    cout<<"[OwlScript "<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"]"<<endl;
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
                astnode* ast = astbuilder.build(input.data());
                vm.exec(ast);
                cleanUpTree(ast); 
            }
            i++;
        }
    }
    cout<<"[hoot!]"<<endl;
}