#ifndef scopingst_hpp
#define scopingst_hpp
#include <iostream>
#include <vector>
#include <map>
#include "../vm/stackitem.hpp"
#include "../vm/constpool.hpp"
#include "../vm/callframe.hpp"
#include "../vm/closure.hpp"
using namespace std;

const unsigned int MAX_LOCALS = 255;

struct Scope;

enum SymTableType {
    NONE = 0,
    LOCALVAR = 1,
    FUNCVAR = 2,
    CLASSVAR = 3
};

struct SymbolTableEntry {
    string name;
    SymTableType type;
    int addr;
    int depth;
    int constPoolIndex;
    int lineNum;
    SymbolTableEntry(string n, int adr, int cpi, SymTableType t, int d) : type(t), addr(adr), name(n), depth(d), constPoolIndex(cpi), lineNum(0) { }
    SymbolTableEntry(string n, int adr, int d) : type(LOCALVAR), name(n), addr(adr), depth(d), constPoolIndex(-1), lineNum(0) { }
    SymbolTableEntry() : type(NONE), addr(-1), constPoolIndex(-1), lineNum(0) { }
    SymbolTableEntry(const SymbolTableEntry& e) {
        name = e.name;
        type = e.type;
        addr = e.addr;
        depth = e.depth;
        constPoolIndex = e.constPoolIndex;
        lineNum = e.lineNum;
    }
    SymbolTableEntry& operator=(const SymbolTableEntry& e) {
        if (this != &e) {
            name = e.name;
            type = e.type;
            addr = e.addr;
            depth = e.depth;
            constPoolIndex = e.constPoolIndex;
            lineNum == e.lineNum;
        }
        return *this;
    }
    bool operator==(const SymbolTableEntry& st) const {
        return name == st.name && type == st.type && addr == st.addr && lineNum == st.lineNum;
    }
    bool operator!=(const SymbolTableEntry& st) const {
        return !(*this==st);
    }
};

class BlockScopeIterator {
    private:
        SymbolTableEntry* cpos;
        int ipos;
        int fpos;
    public:
        BlockScopeIterator(SymbolTableEntry* s, int n);
        bool done();
        SymbolTableEntry& get();
        void next();
};


class BlockScope {
    private:
        friend class ScopingST;
        SymbolTableEntry data[MAX_LOCALS];
        int n;
        BlockScope* enclosingScope;
    public:
        BlockScope(BlockScope* parent);
        int size();
        void insert(string name, SymbolTableEntry st);
        SymbolTableEntry& find(string name);
        SymbolTableEntry& end();
        SymbolTableEntry& operator[](string name);
        BlockScopeIterator iter();
        BlockScope* getEnclosing();
};

class ScopingST {
    private:
        BlockScope* currentScope;
        ConstPool constPool;
        SymbolTableEntry nfSentinel;
        unordered_map<string, ClassObject*> objectDefs;
        int nextAddr() ;
        int depth(BlockScope* s);
        void printST(BlockScope* s, int d) ;
    public:
        ScopingST();
        ConstPool& getConstPool();
        void openObjectScope(string name);
        void copyObjectScope(string instanceName, string objName);
        void openFunctionScope(string name, int L1);
        void closeScope() ;
        void insert(string name) ;
        bool existsInScope(string name);
        SymbolTableEntry& lookup(string name) ;
        ClassObject* lookupClass(string name);
        int depth();
        void print();
};

#endif