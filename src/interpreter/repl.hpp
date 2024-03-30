#ifndef repl_hpp
#define repl_hpp
#include <iostream>
#include "interpreter.hpp"
#include "../ast/astbuilder.hpp"
using std::string;
using std::cout;
using std::endl;


class REPL {
    private:
        ASTBuilder astBuilder;
        Interpreter interpreter;
        bool tracing;
    public:
        REPL() {
            tracing = false;
        }
        void repl() {
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
                if (input != ".exit" && input != "quit") {
                    auto ast = astBuilder.build(input, false);
                    ASTTracer trace;
                    if (tracing)
                        trace.traverse(ast);
                    interpreter.run(ast);
                }
            }
        }
};

#endif