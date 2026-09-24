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
    bool isReady;
    SymbolTableEntry(string n, int adr, int cpi, SymTableType t, int d) : type(t), addr(adr), name(n), depth(d), constPoolIndex(cpi), lineNum(0), isReady(false) { }
    SymbolTableEntry(string n, int adr, int d) : type(LOCALVAR), name(n), addr(adr), depth(d), constPoolIndex(-1), lineNum(0), isReady(false) { }
    SymbolTableEntry() : type(NONE), addr(-1), constPoolIndex(-1), lineNum(0), isReady(false) { }
    SymbolTableEntry(const SymbolTableEntry& e) {
        name = e.name;
        type = e.type;
        addr = e.addr;
        depth = e.depth;
        constPoolIndex = e.constPoolIndex;
        lineNum = e.lineNum;
        isReady = e.isReady;
    }
    SymbolTableEntry& operator=(const SymbolTableEntry& e) {
        if (this != &e) {
            name = e.name;
            type = e.type;
            addr = e.addr;
            depth = e.depth;
            constPoolIndex = e.constPoolIndex;
            lineNum = e.lineNum;
            isReady = e.isReady;
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
        SymbolTableEntry nfSentinel;
    public:
        BlockScope(BlockScope* parent);
        ~BlockScope();
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
        ~ScopingST();
        ConstPool& getConstPool();
        void openObjectScope(string name);
        void copyObjectScope(string instanceName, string objName);
        void openFunctionScope(string name, int L1);
        void closeScope() ;
        void insert(string name) ;
        void makeReady(string name);
        bool existsInScope(string name);
        SymbolTableEntry lookup(string name) ;
        SymbolTableEntry findReady(string name) ; 
        ClassObject* lookupClass(string name);
        BlockScope* scope();
        int depth();
        void print();
};

#endif