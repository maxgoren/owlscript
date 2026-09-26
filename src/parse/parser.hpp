#ifndef lr_parser_hpp
#define lr_parser_hpp
#include <iostream>
#include <functional>
#include <algorithm>
#include <stack>
#include "actions.hpp"
#include "mgcpgen_out.hpp"
#include "ast.hpp"
using namespace std;

class Parser {
    private:
        stack<astnode*> semStack;
        stack<int> st;
        int tpos;
        vector<Token> tokens;
        bool debug_noise;
        Token& current() {
            if (tokens[tpos].getSymbol() == TK_OPEN_COMMENT) {
                advance();
            }
            return tokens[tpos];
        }
        void advance() {
            if (tpos < tokens.size()) {
                if (tokens[tpos].getSymbol() == TK_OPEN_COMMENT) {
                    while (tpos < tokens.size() && tokens[tpos].getSymbol() != TK_CLOSE_COMMENT)
                        tpos++;
                }
                tpos++;
            }
        }
    public:
        Parser(bool loud = false) {
            initprod();
            initactTab();
            initgoTab();
            initActions();
            debug_noise = loud;
        }
        void doShift(int next) {
            if (debug_noise)
                cout<<"SHIFT "<<current().getString()<<endl;
            st.push(next);
            semStack.push(new astnode(current()));
            advance();
        }
        void doReduce(Production& X) {
            if (debug_noise)
                cout<<"REDUCE on '"<<X.toString()<<"'"<<endl;
            vector<astnode*> tmp;
            for (int i = 0; i < X.rhs.size(); i++) {
                st.pop();
                if (!semStack.empty()) {
                    auto m = semStack.top();
                    if (m->token.getString() != "<nil>") {
                        tmp.push_back(semStack.top());
                    }
                    semStack.pop();
                } else {
                    cout<<"Uh oh: Semantic Stack and Parse Stack have diverged"<<endl;
                }
            }
            reverse(tmp.begin(), tmp.end());
            if (X.action.empty() == false) {
                if (debug_noise) cout<<"And do: "<<X.action<<endl;
                semStack.push(actions[X.action.substr(1)](tmp));
                if (debug_noise)
                    preorder(semStack.top(), 1);
            } else {
                if (X.rhs.empty()) {
                    semStack.push(new astnode(Token(TK_EOI, "Epsilon")));
                } else {
                    semStack.push(tmp.front());
                }
            }
            if (goTab[st.top()].find(X.lhs) != goTab[st.top()].end()) {
                st.push(stoi(goTab[st.top()][X.lhs]));
            }
        }
        void printCurrent(int state_num, Token& T) {
            cout<<"[ state: "<<state_num<<"][ token: "<<tokenStr[T.getSymbol()]<<"]"<<actTab[state_num][T.getString()]<<endl<<"Action: ";
        }
        bool checkAccept(int state_num, Token& T) {
            if (T.getSymbol() == TK_EOI && actTab[state_num]["$"] == "accept") {
                if (debug_noise)
                    cout<<"ACCEPT"<<endl;
                return true;
            }
            return false;
        }
        astnode* parse(vector<Token>& tok) {
            tokens = tok;
            tpos = 0;
            st.push(0);
            for (;;) {
                Token curr_token = current();
                int curr_state = st.top();
                if (checkAccept(curr_state, curr_token)) {
                    astnode* tmp = semStack.top();
                    if (tmp->token.getString() == "Epsilon") {
                        return tmp->next;
                    } else {
                        return tmp;
                    }
                }
                if (actTab[curr_state].find(tokenStr[curr_token.getSymbol()]) == actTab[curr_state].end()) {
                    cout<<"Hmm, no actions on '"<<tokenStr[curr_token.getSymbol()]<<"'?"<<endl;
                    if (debug_noise) {
                        cout<<"Possible Transitions from Current: "<<endl;
                        int i = 1;
                        for (auto m : actTab[curr_state]) {
                            cout<<i<<": "<<m.first<<": "<<m.second<<endl;
                        }
                    }
                    cout<<"Bailing out."<<endl;
                    return nullptr;
                } else {
                    if (debug_noise)
                        printCurrent(curr_state, curr_token);
                    string act = actTab[curr_state].at(tokenStr[curr_token.getSymbol()]);
                    int next = stoi(act.substr(1));
                    switch (act[0]) {
                        case 's': {
                            doShift(next);
                        } break;
                        case 'r': {
                            Production p = prod[next];
                            doReduce(p);
                        } break;
                        default:
                            cout<<"Syntax Error: "<<tokenStr[curr_token.getSymbol()]<<endl;
                            return nullptr;
                    }
                }
            }
            return nullptr;
        }
    };

#endif