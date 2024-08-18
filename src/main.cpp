#include <iostream>
#include "repl.hpp"
using namespace std;

int main() {
    REPL repl(true);
    repl.start();
    return 0;
}