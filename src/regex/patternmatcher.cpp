#include "patternmatcher.hpp"

void printEdge(Transition& t) {
    if (t.edge->isEpsilon()) {
        cout<<'\t'<<t.from<<" - ["<<t.edge->getLabel().charachters<<"] ->"<<t.to<<endl;
    } else {
        cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
    }
}

bool matchre(string text, string pattern) {
    NFACompiler compiler;
    NFA nfa = compiler.compile(pattern);
    RegExPatternMatcher pm(nfa);
    return pm.match(text);
}