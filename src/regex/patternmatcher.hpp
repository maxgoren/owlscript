#ifndef patternmatcher_hpp
#define patternmatcher_hpp
#include <iostream>
#include "re_compiler.hpp"
#include "nfa.hpp"
using namespace std;

void printEdge(Transition& t);

//Algorithm 3.4, Simulating

class RegExPatternMatcher {
    private:
        NFA nfa;
        // Gathers a list of states reachable from those in 
        // currStates which have transition that consume ch
        unordered_set<State> move(unordered_set<State> currStates, char ch) {
            unordered_set<State> nextStates;
            if (loud) cout<<ch<<": "<<endl;
            for (State s : currStates) {
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->matches(ch) || t.edge->matches('.')) {
                        if (t.edge->isEpsilon() == false && nextStates.find(t.to) == nextStates.end()) {
                            if (loud) printEdge(t);
                            nextStates.insert(t.to);
                        } 
                    }
                }
            }
            return nextStates;
        }
        //An interesting adaptation of Depth First Search.
        //Gathers a list of states reachable from those in 
        //currStates by using _only_ epsilon transitions.
        unordered_set<State> e_closure(unordered_set<State> currStates) {
            unordered_set<State> nextStates = currStates;
            IndexedStack<State> sf;
            for (State s : currStates)
                sf.push(s);
            while (!sf.empty()) {
                State s = sf.pop();
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->isEpsilon()) {
                        if (nextStates.find(t.to) == nextStates.end()) {
                            if (loud) printEdge(t);
                            nextStates.insert(t.to);
                            sf.push(t.to);
                        }
                    }
                }
            }
            return nextStates;
        }
        int spos;
        string inputText;
        char nextChar() {
            return inputText[spos++];
        }
        char init(const string& text) {
            inputText = text;
            spos = 0;
            return nextChar();
        }
        bool loud;
        const static char eOf = '\0';
    public:
        RegExPatternMatcher(NFA& fa, bool trace = false) : nfa(fa), loud(trace) {

        }
        void setNFA(NFA& fa) {
            nfa = fa;
        }
        bool match(string text) {
            unordered_set<State> curr;
            char a = init(text);
            curr = e_closure({nfa.getStart()});
            while (a != eOf) {
                curr = e_closure(move(curr, a));
                a = nextChar();
            }
            return curr.find(nfa.getAccept()) != curr.end();
        }
};

bool matchre(string text, string pattern);

#endif