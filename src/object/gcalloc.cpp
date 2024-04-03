#include "object.hpp"
//This obviously very much a work in progress, just wiring this in early 
//so it wornt require major surgery later.

GC_Allocator::GC_Allocator() {

}

Object* GC_Allocator::allocNode() {
    if (freed.empty()) {
        Object* nn = new Object;
        nn->gcLive = false; //will be flipped to true when the mark phase knows to be reachable
        created.push_back(nn);
        return nn;
    }
    Object* nn = freed.back();
    freed.pop_back();
    nn->gcLive = false;
    return nn;
}

void GC_Allocator::freeNode(Object* obj) {
    if (obj != nullptr) {
        freed.push_back(obj);
    }
}

void GC_Allocator::mark() {

}

void GC_Allocator::sweep() {

}

void GC_Allocator::mark_node(Object* obj) {
    if (obj != nullptr) {
        obj->gcLive = true;
    }
}