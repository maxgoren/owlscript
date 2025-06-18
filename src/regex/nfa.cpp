#include "nfa.hpp"

bool operator<(const Edge& s, const Edge& t) {
    return s.getFrom() < t.getFrom() && s.getTo() < t.getTo();
}

bool operator==(const Edge& s, const Edge& t) {
    return s.getFrom() == t.getFrom() && s.getTo() == t.getTo();
}

bool operator!=(const Edge& s, const Edge& t) {
    return !(s == t);
}