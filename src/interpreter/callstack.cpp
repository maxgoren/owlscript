#include "interpreter.hpp"

CallStack::CallStack() {
    p = 0;
    max = 255;
    stack = new ActivationRecord*[max];
}

CallStack::~CallStack() {

}

void CallStack::grow() {
    ActivationRecord** ns = new ActivationRecord*[2*max];
    for (int i = 0; i < max; i++)
        ns[i] = stack[i];
    delete [] stack;
    stack = ns;
    max *= 2;
}

bool CallStack::empty() {
    return p == 0;
}

int CallStack::size() {
    return p;
}

void CallStack::push(ActivationRecord* ar) {
    if (p+1 == max) grow();
    stack[p++] = ar;
}

void CallStack::pop() {
    if (p-1 == 0) {
        return;
    }
    ActivationRecord* t = top();
    p--;
    delete t;
}
ActivationRecord* CallStack::top() {
    return stack[p-1];
}