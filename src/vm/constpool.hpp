#ifndef constpool_hpp
#define constpool_hpp
#include <unordered_map>
#include <queue>
#include "stackitem.hpp"
#include "closure.hpp"
using namespace std;

// the items stored in this table are _not_ considered 
// roots for the GC. If an object was not reached during the mark phase it should be reclaimed 
// as garbage

class ConstPool {
    private:
    friend class GarbageCollector;
        unordered_map<string, int> stringPool;
        StackItem* data;
        queue<int> freeList;
        int n;
        int maxN;
        void grow();
        int nextAddress() ;
    public:
        ConstPool();
        ~ConstPool();
        ConstPool(const ConstPool& cp);
        ConstPool& operator=(const ConstPool& cp) ;
        int insert(StackItem item) ;
        StackItem& get(int indx);
        int size();
};


#endif