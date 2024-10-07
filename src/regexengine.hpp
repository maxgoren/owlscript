#ifndef regexpengine_hpp
#define regexpengine_hpp
#include <iostream>
#include <vector>
#include "stack.hpp"
using namespace std;

/*
    The following code has been adapted from Sedgewick, R. and Wayne, K. "Algorithms" 4th ed. 2008
*/


template <class T>
struct Bag : public vector<T> {
    Bag& add(T info) {
        vector<T>::push_back(info);
        return *this;
    }
};

class Digraph {
    private:
        int _v;
        Bag<int>* adjlist;
    public:
        Digraph(int numv = 5) {
            adjlist = new Bag<int>[numv];
            _v = numv;
        }
        Digraph(const Digraph& o) {
            _v = o._v;
            adjlist = new Bag<int>[o._v];
            for (int i = 0; i < o.V(); i++) {
                if (!o.adjlist[i].empty()) {
                    for (int u : o.adjlist[i]) {
                        addEdge(i, u);
                    }
                }
            }
        }
        ~Digraph() {
            if (adjlist != nullptr) 
                delete [] adjlist;
        }
        void addEdge(int v, int w) {
            adjlist[v].add(w);
        }
        Bag<int>& adj(int v) {
            return adjlist[v];
        }
        int V() const {
            return _v;
        }
        Digraph operator=(const Digraph& o) {
            _v = o._v;
            adjlist = new Bag<int>[o._v];
            for (int i = 0; i < o.V(); i++) {
                if (!o.adjlist[i].empty()) {
                    for (int u : o.adjlist[i]) {
                        addEdge(i, u);
                    }
                }
            }
            return *this;
        }
};

class DirectedDFS {
    private:
        bool* seen;
        int numv;
        void search(Digraph& G, int v) {
            seen[v] = true;
            for (int u : G.adj(v))
                if (!seen[u])
                    search(G, u);
        }
        void initseen(size_t size) {
            numv = size;
            seen = new bool[size];
            for (int i = 0; i < size; i++)
                seen[i] = false;
        }
    public:
        DirectedDFS(Digraph& G, int v) {
            initseen(G.V());
            search(G, v);
        }
        DirectedDFS(Digraph& G, Bag<int>& src) {
            initseen(G.V());
            for (int v : src)
                if (!seen[v])
                    search(G, v);
        }
        DirectedDFS(const DirectedDFS& o) {
            if (o.numv > 0) {
                numv = o.numv;
                seen = new bool[numv];
                for (int i = 0; i < numv; i++)
                    seen[i] = o.seen[i];
            }
        }
        ~DirectedDFS() {
            if (seen != nullptr) 
                delete [] seen;
        }
        bool marked(int v) {
            return seen[v];
        }
        DirectedDFS operator=(const DirectedDFS& o) {
            if (this == &o) {
                return *this;
            }
            if (o.numv > 0) {
                numv = o.numv;
                seen = new bool[numv];
                for (int i = 0; i < numv; i++)
                    seen[i] = o.seen[i];
            }
            return *this;
        }
};

class NFA {
     private:
        bool loud;
        Digraph G;
        string re;
        int m;
        void buildEpsilonGraph() {
            InspectableStack<int> ops;
            for (int i = 0; i < m; i++) {
                int lp = i;
                if (re[i] == '(' || re[i] == '|')
                    ops.push(i); 
                else if (re[i] == ')') {
                    int ro = ops.pop();
                    if (re[ro] == '|') {
                        lp = ops.pop();
                        G.addEdge(lp, ro+1);
                        G.addEdge(ro, i);
                    } else if (re[ro] == '(') { lp = ro;
                    } else lp = i;
                } 
                if (i < m - 1 && re[i+1] == '*') {
                    G.addEdge(lp, i+1);
                    G.addEdge(i+1, lp);
                }
                if (i < m-1 && (re[i+1] == '+'))
                    G.addEdge(i+1, lp);
                if (re[i] == '(' || re[i] == '*' ||  re[i] == '+' || re[i] == ')')
                    G.addEdge(i, i+1); 
            }
        }
        void showNFA() {
            for (int i = 0; i < G.V(); i++) {
                if (i < re.size()) {
                    cout<<i<<"("<<re[i]<<"): "<<endl;
                    for (int u : G.adj(i)) {
                        if (u < re.size()) {
                            cout<<"  --> "<<u<<"("<<re[u]<<") "<<endl;
                        }
                    }
                } else {
                    cout<<"$ (Match)."<<endl;
                }
            }
        }
     public:
        NFA(string regexp = ".", bool trace = false) {
            loud = trace;
            re = "(" + regexp + ")";
            m = re.length();
            G = Digraph(m+1);
            buildEpsilonGraph();
            if (trace) {
                showNFA();
            }
        }  
        bool match(string text) {
            DirectedDFS dfs(G, 0);
            Bag<int> pc;
            for (int v = 0; v < G.V(); v++)
                if (dfs.marked(v))
                    pc.add(v); 
            for (int i = 0; i < text.length(); i++) {
                Bag<int> states;
                for (int v : pc)
                    if (v < m)
                        if (re[v] == text[i] || re[v] == '.') {
                            if (loud) {
                                cout<<"Match at: "<<text[i]<<" == "<<re[v]<<endl;
                            }
                            states.add(v+1); 
                        }
                pc.clear();
                dfs = DirectedDFS(G, states);
                for (int v = 0; v < G.V(); v++)
                    if (dfs.marked(v))
                        pc.add(v);
                if (pc.empty())
                    return false;
            }
            for (int v : pc) 
                if (v == m) 
                    return true;
            
            return false;      
        }
};

#endif