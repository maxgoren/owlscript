#ifndef memstore_hpp
#define memstore_hpp
#include "../object/object.hpp"
inline const int MAX_MEM_STORE = 1500;

class MemStore {
    private:
        friend GC_Allocator;
        Object* memstore[MAX_MEM_STORE];
        int nextFreeAddress;
        int freedCells[MAX_MEM_STORE];
        int freedCount;
        void rungc();
    public:
        MemStore();
        void free(int addr);
        void store(int adrr, Object* obj);
        int storeAtNextFree(Object* obj);
        int allocate();
        Object*& get(int addr);
};

#endif