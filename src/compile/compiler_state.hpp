#ifndef compiler_state_hpp
#define compiler_state_hpp
#include <iostream>
using namespace std;

enum COMPILER_STATE {
    READY, LEX, PARSE, BUILD_ST, CODE_GEN, ERROR, DONE
};

static const string stateNames[] = { "Ready", "Lexical Analysis", "Parsing", "Building Symbol Table", "Generating Bytecode", "Error", "Done" };

struct CompilerState {
    COMPILER_STATE current;
    COMPILER_STATE previous;
    CompilerState() {
        current = READY;
        previous = READY;
    }
    void setState(COMPILER_STATE state) {
        previous = current;
        current = state;
    }
    string what() {
        return (stateNames[previous] + " " + stateNames[current]);
    }
    CompilerState& operator=(COMPILER_STATE state) {
        setState(state);
        return *this;
    }
};

#endif