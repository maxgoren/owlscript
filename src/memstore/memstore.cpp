#include "memstore.hpp"

MemStore::MemStore() {
    nextFreeAddress = 0;
    memstore[0] = new Object;
    memstore[0]->type = AS_NIL;
    memstore[0]->isnull = true;
    freedCount = 0;
}

void MemStore::store(int addr, Object* obj) {
    if (addr < 1 || addr >= MAX_MEM_STORE) {
        cout<<"Error: Invalid memory address: 0x"<<addr<<endl;
        return;
    }
    memstore[addr] = obj;
}

int MemStore::storeAtNextFree(Object* obj) {
    int nextFree = allocate();
    store(nextFree, obj);
    return nextFree;
}

int MemStore::allocate() {
    int addr = 0;
    if (nextFreeAddress+1 == MAX_MEM_STORE) {
        if (freedCount == 0) {
            cout<<"Error: out of memory!"<<endl;
            addr = 0;
        } else {
            addr = freedCells[--freedCount];
        }
    } else {
        addr = ++nextFreeAddress;
    }
    liveCells++;
    return addr;
}

void MemStore::free(int addr) {
    if (addr > 0 && addr < MAX_MEM_STORE) {
        freedCells[freedCount++] = addr;
        liveCells--;
        //GarbageCollector.freeNode(memstore[addr]);
    }
}

Object*& MemStore::get(int addr) {
    if (addr > 0 && addr < MAX_MEM_STORE)
        return memstore[addr];
    cout<<"Error: invalid memory address: 0x"<<addr<<endl;
    return memstore[0];
}

void MemStore::memstats() {
    cout<<"Memstats: "<<endl;
    cout<<"Next Free Addr:  "<<nextFreeAddress<<endl;
    cout<<"Free store size: "<<freedCount<<endl;
    cout<<"Overall Usage: "<<((double)liveCells/MAX_MEM_STORE)<<endl;
}