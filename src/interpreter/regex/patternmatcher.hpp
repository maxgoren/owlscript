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

class BacktrackingPatternMatcher : public PatternMatcher {
    private:
        NFA nfa;
        struct Node {
            int strPos;
            State state;
            unordered_set<Transition> epsHistory;
            Node(int sp, State s, unordered_set<Transition> t) : strPos(sp), state(s), epsHistory(t) { }
        };
        bool matchbt(string text) {
            cout<<"Attempting to match: "<<text<<", Start state: "<<nfa.getStart()<<", Accept state: "<<nfa.getAccept()<<endl;
            unordered_set<Transition> epsHistory;
            Stack<Node> sf;
            sf.push(Node(0, nfa.getStart(), epsHistory));
            int from = 0;
            while (!sf.empty()) {
                int strPos = sf.top().strPos;
                State currState = sf.top().state;
                epsHistory = sf.top().epsHistory;
                sf.pop();
                char input = text[strPos];
                cout<<"State: "<<currState<<", Input: "<<input<<endl;
                if (currState == nfa.getAccept()) { 
                    cout<<"Found Accept State."<<endl;
                    cout<<text.substr(from, text.length()-strPos);
                    return true;
                }
                for (Transition t : nfa.getTransitions(currState)) {
                    if ((t.edge->matches(input) || t.edge->matches('.')) || t.edge->isEpsilon()) {
                        if (t.edge->isEpsilon()) { 
                            if (epsHistory.find(t) != epsHistory.end()) {
                                cout<<"\nAlready on Stack.\n"<<endl;
                                continue;
                            }
                            epsHistory.insert(t);
                            sf.push(Node(strPos, t.to, epsHistory));
                        } else {
                            epsHistory.clear();
                            sf.push(Node(strPos + 1, t.to, epsHistory));
                        }
                        cout<<t.from<<"-("<<t.edge->getLabel().charachters<<")->"<<t.to<<endl;
                    } else {
                        cout<<"Dead end."<<endl;
                    }
                    cout<<endl;
                }
            }
            return false;
        }
    public:
        BacktrackingPatternMatcher(NFA& fa) {
            nfa = fa;
        }
        void setNFA(NFA& fa) {
            nfa = fa;
        }
        bool match(string text) {
            return matchbt(text);
        }
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
                        if (t.edge->isEpsilon() == false) {
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
            Stack<State> sf;
            for (State s : clist)
                sf.push(s);
            while (!sf.empty()) {
                State s = sf.pop();
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->isEpsilon()) {
                        if (nlist.find(t.to) == nlist.end()) {
                            if (loud)
                                cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
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