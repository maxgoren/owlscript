#include "interpreter.hpp"

void Interpreter::enter(string s) {
    recDepth = (recDepth > 60) ? 0:recDepth+1;
    step++;
    say(s);
}

void Interpreter::setLoud(bool l) {
    loud = l;
}

void Interpreter::say(string s) {
    if (loud) {
        for (int i = 0; i < recDepth; i++)
            cout<<"  ";
        cout<<"("<<step<<") "<<s<<endl;
    }
}

void Interpreter::leave(string s) {
    say(s);
    leave();
}

void Interpreter::leave() {
    --recDepth;
    step--;
    if (recDepth < 0) {
        resetRecDepth();
    }
    if (step < 0) step = 0;
}

void Interpreter::resetRecDepth() {
    recDepth = 0;
}

void Interpreter::memstats() {
    memStore.memstats();
}