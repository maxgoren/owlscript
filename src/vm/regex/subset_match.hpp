#ifndef subset_match_hpp
#define subset_match_hpp
#include <iostream>
#include <set>
#include "re_compiler.hpp"
using namespace std;

set<NFAState*> move(char ch, set<NFAState*> states);

set<NFAState*> e_closure(set<NFAState*> states);

bool recognizeString(NFA& nfa, string text);

bool matchRegex(string pattern, string text);

#endif