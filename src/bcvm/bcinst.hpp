#ifndef bcinst_hpp
#define bcinst_hpp
#include "../object.hpp"

enum VMInstSymbol {
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_NOP,
    VM_LOAD_CONST,
    VM_STOR_CONST,
    VM_LOAD_LOCAL,
    VM_STOR_LOCAL,
    VM_LOAD_GLOBAL,
    VM_STOR_GLOBAL,
    VM_PRINT,
    VM_RET,
    VM_HALT
};

struct VMInstruction {
    VMInstSymbol inst;
    Object operand;
    VMInstruction(VMInstSymbol sym = VM_HALT, Object obj = makeNil()) : inst(sym), operand(obj) { }
};

#endif