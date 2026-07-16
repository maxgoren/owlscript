#ifndef gc_hpp
#define gc_hpp
#include <vector>
#include <memory>
#include "constpool.hpp"
#include "stackitem.hpp"
#include "instruction.hpp"
using namespace std;

class GarbageCollector {
    private:
        void markObject(GCItem*& curr);
        void markItem(StackItem* si) ;
        void markAR(ActivationRecord* callframe);
        void sweep() ;
        void markOpStack(StackItem ops[], int sp);
        void markConstPool(ConstPool* constPool);
        void markRoots(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool);
        unsigned int GC_LIMIT;
    public:
        GarbageCollector();
        bool ready();
        void run(ActivationRecord* callstk, StackItem opstk[], int sp, ConstPool* constPool);
};

#endif