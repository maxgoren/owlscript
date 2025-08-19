#ifndef codegen_hpp
#define codegen_hpp
#include <iostream>
#include <vector>
#include "bcinst.hpp"
#include "../ast.hpp"
#include "../object.hpp"
using namespace std;

class CodeGen {
    private:
        vector<VMInstruction> codepage;
        int ip;
        void emit(VMInstruction inst) {
            codepage[ip++] = inst;
        }
        int emitSkip(int spaces) {
            ip += spaces;
            return ip;
        }
    public:
        CodeGen() {
            ip = 0;
        }
};

#endif