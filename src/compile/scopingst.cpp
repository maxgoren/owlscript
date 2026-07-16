#include "scopingst.hpp"

BlockScopeIterator::BlockScopeIterator(SymbolTableEntry* s, int n) {
    cpos = s;
    ipos = 0;
    fpos = n;
}
bool BlockScopeIterator::done() {
    return ipos == fpos;
}
SymbolTableEntry& BlockScopeIterator::get() {
    return cpos[ipos];
}
void BlockScopeIterator::next() {
    ipos++;
}

BlockScope::BlockScope(BlockScope* parent = nullptr) {
    n = 0;
    enclosingScope = parent;
}
int BlockScope::size() {
    return n;
}
void BlockScope::insert(string name, SymbolTableEntry st) {
    data[n++] = st;
}
SymbolTableEntry& BlockScope::find(string name) {
    int idx = 0;
    while (idx < n) {
        if (data[idx].name == name) {
            return data[idx];
        }
        idx++;
    }
    return end();
}
SymbolTableEntry& BlockScope::end() {
    return data[n];
}
SymbolTableEntry& BlockScope::operator[](string name) {
    if (find(name) == end())
        insert(name, SymbolTableEntry(name, n, -1));
    return find(name);
}
BlockScopeIterator BlockScope::iter() {
    return BlockScopeIterator(data, n);
}
BlockScope* BlockScope::getEnclosing() {
    return enclosingScope;
}

ScopingST::ScopingST() {
    currentScope = new BlockScope();
    nfSentinel = SymbolTableEntry("not found", -1, -1);
}
ConstPool& ScopingST::getConstPool() {
    return constPool;
}
void ScopingST::openObjectScope(string name) {
    if (currentScope->find(name) != currentScope->end()) {
            if (currentScope->find(name).type == CLASSVAR) {
            BlockScope* ns = constPool.get(currentScope->find(name).constPoolIndex).objval->object->scope;
            currentScope = ns;
            } else {
            BlockScope* ns = new BlockScope(currentScope);
            currentScope = ns;
            }
    } else {
        BlockScope* ns = new BlockScope(currentScope);
        ClassObject* obj = new ClassObject(name, ns);
        objectDefs.insert(make_pair(name, obj));
        int constIdx = constPool.insert(alloc.alloc(obj));
        int envAddr = nextAddr();
        objectDefs[name]->cpIdx = constIdx;
        currentScope->insert(name, SymbolTableEntry(name, envAddr, constIdx, CLASSVAR, depth(currentScope)+1));
        currentScope = ns;
    }
}
void ScopingST::copyObjectScope(string instanceName, string objName) {
    if (objectDefs.find(objName) == objectDefs.end()) {
        cout<<"Attempted to instantiate Unknown class '"<<objName<<"'"<<endl;
    } else {
        currentScope->insert(instanceName, SymbolTableEntry(instanceName, nextAddr(), objectDefs[objName]->cpIdx, CLASSVAR, depth(currentScope)+1));
    }
}
void ScopingST::openFunctionScope(string name, int L1) {
    if (currentScope->find(name) != currentScope->end()) {
        BlockScope* ns = constPool.get(currentScope->find(name).constPoolIndex).objval->closure->func->scope;
        constPool.get(currentScope->find(name).constPoolIndex).objval->closure->func->start_ip = L1;
        currentScope = ns;
    } else {
        BlockScope*  ns = new BlockScope(currentScope);
        int funcId = constPool.insert(alloc.alloc(new Function(name, L1, ns)));
        int constIdx = constPool.insert(alloc.alloc(new Closure(constPool.get(funcId).objval->func, nullptr)));
        int envAddr = nextAddr();
        currentScope->insert(name, SymbolTableEntry(name, envAddr, constIdx, FUNCVAR, depth(currentScope)+1));
        currentScope = ns;
    }
}
void ScopingST::closeScope() {
    if (currentScope != nullptr && currentScope->getEnclosing() != nullptr) {
        currentScope = currentScope->getEnclosing();
    }
}
void ScopingST::insert(string name) {
    currentScope->insert(name, SymbolTableEntry(name, nextAddr(), depth(currentScope)));
}
bool ScopingST::existsInScope(string name) {
    return currentScope->find(name) != currentScope->end();
}
SymbolTableEntry& ScopingST::lookup(string name) {
    BlockScope* x = currentScope;
    while (x != nullptr) {
        if (x->find(name) != x->end())
            return x->find(name);
        x = x->getEnclosing();
    }
    return nfSentinel;
}
ClassObject* ScopingST::lookupClass(string name) {
    if (objectDefs.find(name) != objectDefs.end())
        return objectDefs.at(name);
    return nullptr;
}
int ScopingST::depth() {
    return depth(currentScope);
}
void ScopingST::print() {
    cout<<"Symbol table: \n";
    printST(currentScope, 1);
}

int ScopingST::nextAddr() {
    int na = currentScope->size()+1;
    return na;
}
int ScopingST::depth(BlockScope* s) {
    if (s == nullptr || s->getEnclosing() == nullptr) {
        return -1;
    }
    auto x = s;
    int d = 0;
    while (x != nullptr) {
        d++;
        x = x->getEnclosing();
    }
    return d-1;
}
void ScopingST::printST(BlockScope* s, int d) {
    auto x = s;
    for (int i = 0; i < MAX_LOCALS; i++) {
        auto m = x->data[i];
        if (m.type != NONE) {
            for (int i = 0; i < d; i++) cout<<"  ";
            cout<<m.name<<": "<<m.addr<<", "<<m.depth<<endl;
            if (m.type == 2) {
                printST(constPool.get(m.constPoolIndex).objval->closure->func->scope,d + 1);
            } else if (m.type == 3) {
                printST(constPool.get(m.constPoolIndex).objval->object->scope, d+1);
            }
        }
    }
}