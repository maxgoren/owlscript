#ifndef regexpengine_hpp
#define regexpengine_hpp
#include <iostream>
#include <unordered_set>
#include "queue.hpp"
#include "stack.hpp"
#include "bag.hpp"
#include <vector>
using namespace std;

/*
    Based on code from Sedgewick, R. and Wayne, K. "Algorithms" 4th ed. 2008

*/

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
            if (this != &o) {
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
            return *this;
        }
};

class GraphSearch {
    private:
        int* pred;
        bool* seen;
        int numv;
        bool loud;
        void search(Digraph& G, int v) {
            queue<int> fq;
            fq.push(v);
            pred[v] = v;
            seen[v] = true;
            while (!fq.empty()) {
                int curr = fq.front();
                fq.pop();
                if (loud) cout<<" -> "<<curr<<" ";
                for (int u : G.adj(curr)) {
                    if (!seen[u]) {
                        seen[u] = true;
                        pred[u] = v;
                        fq.push(u);
                    }
                }
            }
        }
        void initseen(size_t size) {
            numv = size;
            seen = new bool[size];
            pred = new int[size];
            for (int i = 0; i < size; i++) {
                seen[i] = false;
                pred[i] = -1;
            }
        }
    public:
        GraphSearch(Digraph& G, int v, bool debug) {
            loud = debug;
            initseen(G.V());
            search(G, v);
            if (loud) cout<<endl;
        }
        GraphSearch(Digraph& G, Bag<int>& src, bool debug) {
            loud = debug;
            initseen(G.V());
            for (int v : src)
                if (!seen[v]) {
                    search(G, v);
                    if (loud) cout<<endl;
                }
        }
        GraphSearch(const GraphSearch& o) {
            if (o.numv > 0) {
                numv = o.numv;
                seen = new bool[numv];
                loud = o.loud;
                for (int i = 0; i < numv; i++)
                    seen[i] = o.seen[i];
            }
        }
        ~GraphSearch() {
            if (seen != nullptr) 
                delete [] seen;
        }
        bool marked(int v) {
            return seen[v];
        }
        int previous(int v) {
            return pred[v];
        }
        GraphSearch operator=(const GraphSearch& o) {
            if (this == &o) {
                return *this;
            }
            if (o.numv > 0) {
                loud = o.loud;
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
        unordered_map<int, int> specifiedSetIndex;
        void buildEpsilonGraph() {
            InspectableStack<int> ops;
            for (int i = 0; i < m; i++) {
                int lp = i;
                if (re[i] == '(' || re[i] == '|' || re[i] == '[')
                    ops.push(i); 
                else if (re[i] == ')') {
                    unordered_set<int> orOpIdx;
                    while (re[ops.top()] == '|') {
                        orOpIdx.insert(ops.pop());
                    }
                    lp = ops.pop();
                    for (int ro : orOpIdx) {
                       G.addEdge(lp, ro+1);
                       G.addEdge(ro, i);
                    } 
                } else if (re[i] == ']') {
                    lp = ops.pop();
                    for (int inside = lp+1; inside < i; inside++) {
                        G.addEdge(lp, inside);
                        specifiedSetIndex[inside] = i;
                    }
                }
                if (i < m - 1 && re[i+1] == '*') {
                    G.addEdge(lp, i+1);
                    G.addEdge(i+1, lp);
                }
                if (i < m-1 && (re[i+1] == '+'))
                    G.addEdge(i+1, lp);
                if (re[i] == '(' || re[i] == '*' ||  re[i] == '+' || re[i] == ')' || re[i] == '[' || re[i] == ']')
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
        Bag<int> collectMatchedStates(char curr, Bag<int>& reached) {
            Bag<int> matches;
            for (int state : reached) {
                if (state < m)
                    if (re[state] == curr || re[state] == '.') {
                        if (specifiedSetIndex.find(state) != specifiedSetIndex.end()) 
                            matches.add(specifiedSetIndex[state]);
                        else matches.add(state+1);
                        if (loud) cout<<"Match at: "<<curr<<" == "<<re[state]<<endl;
                    }
            }
            return matches;
        }
        void collectReachedStates(GraphSearch& search, Bag<int>& reached) {
            for (int v = 0; v < G.V(); v++)
                if (search.marked(v))
                    reached.add(v); 
        }
        bool reachedAcceptState(Bag<int>& reached) {
            for (int v : reached)
                if (v == m)
                    return true;
            return false;
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
            GraphSearch search(G, 0, loud);
            Bag<int> reached;
            collectReachedStates(search, reached);
            for (int i = 0; i < text.length(); i++) {
                Bag<int> matches = collectMatchedStates(text[i], reached);
                if (matches.empty())
                    continue;
                reached = Bag<int>();
                search = GraphSearch(G, matches, loud);
                collectReachedStates(search, reached);
                if (reached.empty())
                    return false;
            }
            return reachedAcceptState(reached);      
        }
};

#endif