#include "repl.hpp"

REPL::REPL(bool debug) : loud(debug), builder(debug), interpreter(debug) {

}

void REPL::start() {
    bool running = true;
    string input;
    int lno = 0;
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
            auto ast = builder.build(ctx, input);
            if (loud) {
                traverse(ast, &printNode, &nullFunc);
                cout<<"----------------------------"<<endl;
            }
            interpreter.execAST(ctx, ast);
            interpreter.refreshContext(ctx);
        }
    }
}