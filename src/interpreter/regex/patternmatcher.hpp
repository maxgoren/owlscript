#ifndef patternmatcher_hpp
#define patternmatcher_hpp
#include <iostream>
#include "nfa.hpp"
using namespace std;

class PatternMatcher {
    public:
        virtual void setNFA(NFA& nfa) = 0;
        virtual bool match(string text) = 0;
};

class RegExPatternMatcher : public PatternMatcher {
    private:
        NFA nfa;
        //Gathers a list of states reachable from those in clist, which have transition which consumes ch
        unordered_set<State> move(unordered_set<State> clist, char ch) {
            unordered_set<State> nlist;
            if (loud) cout<<ch<<": "<<endl;
            for (State s : clist) {
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->matches(ch) || t.edge->matches('.')) {
                        if (t.edge->isEpsilon() == false && nlist.find(t.to) == nlist.end()) {
                            if (loud) cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
                            nlist.insert(t.to);
                        } 
                    }
                }
            }
            return nlist;
        }
        //An interesting adaptation of Depth First Search.
        //Gathers a list of states reachable from those in clist using _only_ epsilon transitions.
        unordered_set<State> e_closure(unordered_set<State> clist) {
            unordered_set<State> nlist = clist;
            IndexedStack<State> sf;
            for (State s : clist)
                sf.push(s);
            while (!sf.empty()) {
                State s = sf.pop();
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->isEpsilon()) {
                        if (nlist.find(t.to) == nlist.end()) {
                            if (loud)
                                cout<<'\t'<<t.from<<" - ["<<t.edge->getLabel().charachters<<"] ->"<<t.to<<endl;
                            nlist.insert(t.to);
                            sf.push(t.to);
                        }
                    }
                }
            }
            return nlist;
        }
        bool loud;
    public:
        RegExPatternMatcher(NFA& nf, bool trace) : nfa(nf), loud(trace) {

        }
        void setNFA(NFA& fa) {
            nfa = fa;
        }
        bool match(string text) {
            unordered_set<State> curr, next;
            next.insert(nfa.getStart());
            curr = e_closure(next);
            for (int i = 0; i < text.length(); i++) {
                next = move(curr, text[i]);
                curr = e_closure(next);
            }
            return curr.find(nfa.getAccept()) != curr.end();
        }
};


#endif