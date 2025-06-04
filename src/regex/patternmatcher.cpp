#include "patternmatcher.hpp"

void printEdge(Transition& t) {
    if (t.edge->isEpsilon()) {
        cout<<'\t'<<t.from<<" - ["<<t.edge->getLabel().charachters<<"] ->"<<t.to<<endl;
    } else {
        cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
    }
}

bool MatchRegEx::operator()(string text, string pattern, bool trace) {
    return RegExPatternMatcher(compiler.compile(pattern), trace).match(text);
}