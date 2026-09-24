#ifndef production_h
#define production_h
#include <vector>
#include <iostream>
using namespace std;

using Symbol = string;

struct SymbolString : vector<Symbol> {
    SymbolString(vector<Symbol> ss) {
        for (auto m : ss) {
            this->push_back(m);
        }
    }
    SymbolString() {

    }
    SymbolString(const SymbolString& ss) {
        for (auto m : ss) {
            this->push_back(m);
        }
    }
    SymbolString subString(int startIndex) {
        SymbolString result;
        for (int i = startIndex; i < this->size(); i++)
            result.push_back((*this)[i]);
        return result;
    }
    SymbolString& operator=(const SymbolString& ss) {
        if (this != &ss) {
            clear();
            for (auto m : ss) {
                this->push_back(m);
            }
        }
        return *this;
    }
    string toString() {
        string str = "";
        for (auto s = begin(); s != end(); s++) {
            str += *s + " ";
        }
        return str;
    }
    bool operator==(const SymbolString& ss) {
        auto oit = ss.begin();
        auto it = this->begin();
        while (oit != ss.end() && it != this->end()) {
            if (*oit != *it)
                return false;
        }
        return (oit == ss.end() && it == this->end());
    }
    bool operator!=(const SymbolString& ss) {
        return !(*this == ss);
    }
};

struct Production {
    int pid; //for executing Actions
    Symbol lhs;
    SymbolString rhs;
    string action;
    Production(int id, Symbol l, SymbolString r, string a) : pid(id), lhs(l), rhs(r), action(a) { }
    Production(int id, Symbol l, SymbolString r) : pid(id), lhs(l), rhs(r) { }
    Production() { }
    string toString() {
        return lhs + " ::= " + rhs.toString(); 
    }
    bool operator==(const Production& op) const {
        return lhs == op.lhs && rhs == op.rhs;
    }
    bool operator!=(const Production& op) const {
        return !(*this==op);
    }
    bool operator<(const Production& op) const {
        return this->lhs < op.lhs;
    }
};

struct ProductionSet : vector<Production> {
    ProductionSet(vector<Production> rhs) {
        for (Production ss : rhs) {
            push_back(ss);
        }
    }
    ProductionSet() {

    }
};


#endif