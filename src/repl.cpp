#include "repl.hpp"

REPL::REPL(bool debug) : loud(debug), builder(debug), interpreter(debug) {

}

void REPL::start() {
    bool running = true;
    string input;
    int lno = 0;
    vector<astnode*> asthistory;
    while (running) {
        cout<<"Owlscript("<<lno++<<")> ";
        getline(cin, input);
        if (input == "quit") {
            running = false;
            continue;
        } else if (input == "clear") {
            for (int i = 0; i < 120; i++) 
                cout<<"\n";
            continue;
        }
        if (!input.empty()) {
            auto ast = builder.build(input);
            if (loud) {
                traverse(ast, &printNode, &nullFunc);
                cout<<"----------------------------"<<endl;
            }
            interpreter.execAST(ast);
            asthistory.push_back(ast);
        }
    }
    for (auto m : asthistory) {
        cleanup(m);
    }
}