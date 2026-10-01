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
        void doShift(int next);
        void doReduce(int next);
        void printCurrent(int state_num, Token& T);
        bool checkAccept(int state_num, Token& T);
        int nextState(const int *table[], int state, int sym);
        astnode* cleanUp();
        astnode* syntaxError(int curr_state, Token curr_token);
    public:
        Parser(bool loud = false) ;
        astnode* parse(vector<Token>& tok);
    };

#endif