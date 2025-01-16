#ifndef repl_hpp
#define repl_hpp
#include <iostream>
#include "interpreter/ast_interpreter.hpp"
#include "ast_builder.hpp"
using namespace std;

class REPL {
    private:
        bool loud;
        ASTBuilder builder;
        ASTInterpreter interpreter;
    public:
        REPL(bool debug = false);
        void start();
};

#endif