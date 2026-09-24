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
        Token& current() {
            return tokens[tpos];
        }
        void advance() {
            if (tpos < tokens.size()) {
                tpos++;
            }
        }
    public:
        Parser(bool loud = true) {
            initprod();
            initactTab();
            initgoTab();
            initActions();
        }
        void doShift(int next) {
            cout<<"SHIFT "<<current().getString()<<endl;
            st.push(next);
            semStack.push(new astnode(current()));
            advance();
        }
        void doReduce(Production& X) {
            cout<<"REDUCE on '"<<X.toString()<<"'"<<endl;
            vector<astnode*> tmp;
            for (int i = 0; i < X.rhs.size(); i++) {
                st.pop();
                if (!semStack.empty()) {
                    auto m = semStack.top();
                    if (m->token.getString() != "<nil>") {
                        //cout<<tokenStr[m->token.getSymbol()]<<", "<<m->token.getString()<<endl;
                        tmp.push_back(semStack.top());
                    }
                    semStack.pop();
                } else {
                    cout<<"Uh oh: Semantic Stack and Parse Stack have diverged"<<endl;
                }
            }
            reverse(tmp.begin(), tmp.end());
            if (X.action.empty() == false) {
                cout<<"And do: "<<X.action<<endl;
                semStack.push(actions[X.action.substr(1)](tmp));
                preorder(semStack.top(), 1);
            } else {
                //cout<<"<no action>"<<endl;
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
                if (checkAccept(curr_state, curr_token))
                    return semStack.top();
                if (actTab[curr_state].find(tokenStr[curr_token.getSymbol()]) == actTab[curr_state].end()) {
                    cout<<"Hmm, no actions on '"<<tokenStr[curr_token.getSymbol()]<<"'?"<<endl;
                    cout<<"Possible Transitions from Current: "<<endl;
                    int i = 1;
                    for (auto m : actTab[curr_state]) {
                        cout<<i<<": "<<m.first<<": "<<m.second<<endl;
                    }
                    cout<<"Bailing out."<<endl;
                    return nullptr;
                } else {
                  //  printCurrent(curr_state, curr_token);
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