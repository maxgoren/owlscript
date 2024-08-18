#ifndef repl_hpp
#define repl_hpp
#include <iostream>
#include "eval.hpp"
#include "ast_builder.hpp"
using namespace std;

int rd = 0;
void traverse(astnode* node, void (*pre)(astnode* x), void (*post)(astnode* x)) {
    rd++;
    if (node != nullptr) {
        pre(node);
        for (int i = 0; i < 3; i++)
            traverse(node->child[i], pre, post);
        post(node);
        traverse(node->next, pre, post);
    }
    rd--;
}

class REPL {
    private:
        bool loud;
        ASTBuilder builder;
    public:
        REPL(bool debug = false) {
            loud = debug;
            builder = ASTBuilder(loud);
        }
        void start() {
            bool running = true;
            string input;
            execute(builder.build("let tl := [11,24,86,42,13]"));
            while (running) {
                cout<<"Owlscript> ";
                getline(cin, input);
                if (!input.empty()) {
                    auto ast = builder.build(input);
                    if (loud) {
                        traverse(ast, &printNode, &nullFunc);
                        cout<<"----------------------------"<<endl;
                    }
                    if (input.substr(0, 5) == "print")
                        execute(ast);
                    else
                        cout<<" -> "<<toString(execute(ast))<<endl;
                }
            }
        }
};

#endif