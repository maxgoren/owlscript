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
        REPL();
        void repl();
};

#endif