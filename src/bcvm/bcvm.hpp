#ifndef bcvm_hpp
#define bcvm_hpp
#include <vector>
#include "../object.hpp"
#include "../stack.hpp"
#include "bcinst.hpp"
using namespace std;

class BCVM {
    private:
        IndexedStack<Object> stack;
        vector<VMInstruction> instructions;
        VMInstruction current;
        VMInstruction& readNextInstruction() {
            if (ip < instructions.size()) {
                current = instructions[ip++];
            }
            return current;
        }
        bool running;
        int ip;
        void executeInstruction(VMInstruction& instruction) {
            switch (current.inst) {
                case VM_LOAD_CONST: {
                    stack.push(current.operand);
                } break;
                case VM_RET:
                case VM_PRINT: {
                    cout<<toString(stack.pop())<<endl;
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
                    break;
            }
        }
    public:
        BCVM() {
            ip = 0;
        }
        void run(vector<VMInstruction> program) {
            instructions = program;
            ip = 0;
            running = true;
            while (running) {
                executeInstruction(readNextInstruction());
            }
        }
};

#endif