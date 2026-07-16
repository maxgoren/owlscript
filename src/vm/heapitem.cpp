#include "heapitem.hpp"
#include "stackitem.hpp"

string listToString(deque<StackItem>* list) {
    string str = "[";
    for (auto m : *list) {
        str += m.toString() + " ";
    }
    return str + "]";
}

string classToString(ClassObject* obj) {
    if (obj == nullptr) {
        return "nil";
    }
    if (obj->instantiated) {
        string str = obj->name + "{ ";
        for (auto m : obj->fields) {
            str += m.first + ": " + m.second.toString() +" ";
        }
        str += "} ";
        return str;
    }
    return obj->name;
}

void freeClass(ClassObject* obj) {
    if (obj != nullptr) {
        for (auto & m : obj->fields) {
            if (m.second.type == OBJECT)
                alloc.free(m.second.objval);
        }
        delete obj;
    }
}