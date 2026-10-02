#ifndef lr_parser_hpp
#define lr_parser_hpp
#include <iostream>
#include <functional>
#include <algorithm>
#include <stack>
#include <climits>
#include "actions.hpp"
#include "parser_tables.hpp"
#include "ast.hpp"
using namespace std;
const static int PARSE_ERR = INT_MIN;
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
        int getNext(const int *table[], int state, int sym);
        vector<astnode*> removeFromStack(int numSym);
        astnode* cleanUpAndAccept();
        astnode* syntaxError(int curr_state, Token curr_token);
    public:
        Parser(bool loud = false) ;
        astnode* parse(vector<Token>& tok);
    };

#endif