#include "closure.hpp"

string closureToString(Closure* closure) {
    return "(closure)" + closure->func.name + ", " + to_string(closure->func.start_ip);
}

void freeClosure(Closure* cl) {
    if (cl != nullptr) {
        delete cl;
    }   
}