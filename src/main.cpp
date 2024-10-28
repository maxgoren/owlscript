#include <iostream>
#include "repl.hpp"
#include "ast_builder.hpp"
#include "ast_interpreter.hpp"
#include "version.hpp"
using namespace std;

void start_owlscript(bool debug);
void exec_file(string filename);

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        start_owlscript(false);
    } else {
        if (argv[1][0] == '-') {
            switch (argv[1][1]) {
                case 'v':
                    start_owlscript(true);
                    break;
                case 'f':
                    exec_file(argv[2]);
                    break;
                case '-': {
                    if (string(argv[1]).substr(2) == "version") {
                        cout<<"[ "<<NAME<<" "<<MAJOR_VERSION<<"."<<MINOR_VERSION<<" ]"<<endl;
                        return 0;
                    }
                }
                default:
                    cout<<"Invalid option '"<<argv[1]<<"'"<<endl;  
                    break;
            }
        } else {
            exec_file(argv[1]);
        }
    }
    return 0;
}

void start_owlscript(bool debug) {
    REPL repl(debug);

    repl.start();
}

void exec_file(string filename) {
    ASTBuilder builder(true);
    ASTInterpreter interpreter(false);
    cout<<"Loading: "<<filename<<endl;
    auto ast = builder.buildFromFile(filename);
    traverse(ast, &printNode, &nullFunc);
    cout<<"----------------------------"<<endl;
    interpreter.execAST(ast);
    if (interpreter.getContext().globals.find("main") != interpreter.getContext().globals.end()) {
        ast = builder.build("main();");
        traverse(ast, &printNode, &nullFunc);
        cout<<"----------------------------"<<endl;
        interpreter.execAST(ast);
    }
    cleanup(ast);
}