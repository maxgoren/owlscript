#include "repl.hpp"

REPL::REPL() {
    tracing = false;
}

void REPL::repl() {
    cout<<"[OwlScript 0.1]"<<endl;
    string input;
    interpreter.setLoud(false);
    while (input != ".exit" && input != "quit") {
        cout<<"Owlscript> ";
        getline(cin, input);
        cout<<"   '-> "<<input<<endl;
        if (input == ".trace") {
            tracing = !tracing;
            interpreter.setLoud(tracing);
            continue;
        }
        if (input == ".memstat") {
            interpreter.memstats();
            continue;
        }
        if (input != ".exit" && input != "quit") {
            auto ast = astBuilder.build(input, tracing);
            ASTTracer trace;
            if (tracing)
                trace.traverse(ast);
            interpreter.resetRecDepth();
            interpreter.run(ast);
        }
    }
    cout<<"[Hoot!]"<<endl;
}