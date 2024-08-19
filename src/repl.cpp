#include "repl.hpp"

REPL::REPL(bool debug) : loud(debug), builder(debug), interpreter(debug) {

}

void REPL::start() {
    bool running = true;
    string input;
    int lno = 0;
    interpreter.execAST(builder.build("let C := &(i) -> i;"));
    interpreter.execAST(builder.build("def a(var k, var x1, var x2, var x3, var x4, var x5) { def b() { println \"in b\"; k := k - 1; println \"k is \" + k; println \"calling a\"; return a(k, C(b), x1, x2, x3, x4); }; println \"in a, k is \" + k;  if (k <= 0) { println \"adding x4 and x5\"; return x4() + x5(); } else { println \"calling b\"; return b(); } }"));
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
            if (input.substr(0, 5) == "print")
                interpreter.execAST(ast);
            else
                cout<<" -> "<<toString(interpreter.execAST(ast))<<endl;
        }
    }
}