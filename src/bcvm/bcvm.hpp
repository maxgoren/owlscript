#ifndef bcvm_hpp
#define bcvm_hpp
#include <vector>
#include "../object.hpp"
#include "../stack.hpp"
using namespace std;

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

class BCVM {
    private:
        IndexedStack<Object> stack;
        vector<VMInstruction> instructions;
        VMInstruction current;
        void readNextInstruction() {
            if (ip < instructions.size()) {
                current = instructions[ip++];
            }
        }
        int ip;
    public:
        BCVM() {
            ip = 0;
        }
        void run() {
            ip = 0;
            bool running = true;
            while (running) {
                readNextInstruction();
                switch (current.inst) {
                    case VM_LOAD_CONST: {
                        stack.push(current.operand);
                    } break;
                    case VM_RET:
                    case VM_PRINT: {
                        cout<<toString(current.operand)<<endl;
                    } break;
                    case VM_ADD: {
                        Object rhs = stack.pop();
                        Object lhs = stack.pop();
                        stack.push(add(lhs, rhs));
                    } break;
                    case VM_SUB: {
                        Object rhs = stack.pop();
                        Object lhs = stack.pop();
                        stack.push(sub(lhs, rhs));
                    } break;
                    case VM_MUL: {
                        Object rhs = stack.pop();
                        Object lhs = stack.pop();
                        stack.push(mul(lhs, rhs));
                    } break;
                    case VM_DIV: {
                        Object lhs = stack.pop();
                        Object rhs = stack.pop();
                        stack.push(div(lhs, rhs));
                    } break;
                    case VM_HALT: {
                        running = false;
                    } break;
                    default:
                }
            }
        }
};

#endif