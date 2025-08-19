#include <iostream>
#include "bcvm.hpp"
#include "../object.hpp"
using namespace std;


int main(int argc, char* argv[]) {
    BCVM vm;
    vector<VMInstruction> program = {
        {VM_LOAD_CONST, makeInt(5)},
        {VM_LOAD_CONST, makeInt(8)},
        {VM_ADD},
        {VM_PRINT},
        {VM_HALT}
    };
    vm.run(program);
}