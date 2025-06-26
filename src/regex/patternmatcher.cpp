#include "patternmatcher.hpp"

void printEdge(Edge& t) {
    if (t.isEpsilon()) {
        cout<<'\t'<<t.getFrom()<<" - ["<<t.getLabel().charachters<<"] ->"<<t.getTo()<<endl;
    } else {
        cout<<'\t'<<t.getFrom()<<" - ("<<t.getLabel().charachters<<") ->"<<t.getTo()<<endl;
    }
}

bool MatchRE::operator()(StringBuffer& text, string pattern, bool trace) {
    compiler.setTrace(trace);
    if (trace) {
        return RegExPatternMatcher(compiler.compile(pattern), trace).printNFA().match(text);
    }
    return RegExPatternMatcher(compiler.compile(pattern), trace).match(text);
}