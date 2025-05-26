#include "nfa.hpp"

bool operator==(const Transition& s, const Transition& t) {
    return s.from == t.from && s.to == t.to && s.edge == t.edge;
}

bool operator!=(const Transition& s, const Transition& t) {
    return !(s == t);
}