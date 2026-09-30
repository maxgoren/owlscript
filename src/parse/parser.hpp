#ifndef lr_parser_hpp
#define lr_parser_hpp
#include <iostream>
#include <functional>
#include <algorithm>
#include <stack>
#include "actions.hpp"
#include "parser_tables.hpp"
#include "ast.hpp"
using namespace std;

class Parser {
    private:
        stack<astnode*> semStack;
        stack<int> st;
        int tpos;
        vector<Token> tokens;
        bool debug_noise;
        Token& current();
        void advance();
        int nextState(const int *table[], int state, int sym);
    public:
        Parser(bool loud = false) ;
        void doShift(int next);
        void doReduce(Production& X);
        void printCurrent(int state_num, Token& T);
        bool checkAccept(int state_num, Token& T) ;
        astnode* parse(vector<Token>& tok);
    };

#endif