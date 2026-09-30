#include "parser.hpp"

Token& Parser::current() {
    if (tokens[tpos].getSymbol() == TK_OPEN_COMMENT) {
        advance();
    }
    return tokens[tpos];
}
void Parser::advance() {
    if (tpos < tokens.size()) {
        if (tokens[tpos].getSymbol() == TK_OPEN_COMMENT) {
            while (tpos < tokens.size() && tokens[tpos].getSymbol() != TK_CLOSE_COMMENT)
                tpos++;
        }
        tpos++;
    }
}
int Parser::nextState(const int *table[], int state, int sym) {
            int N = table[state][0];
            for (int i = 1; i < 2*N+1; i+=2) {
                if (table[state][i] == sym) {
                    return i+1;
                }
            }
            return -1;
        }

Parser::Parser(bool loud) {
    debug_noise = loud;
}

void Parser::doShift(int next) {
    if (debug_noise)
        cout<<"SHIFT "<<current().getString()<<endl;
    st.push(next);
    semStack.push(new astnode(current()));
    advance();
}

void Parser::doReduce(Production& X) {
   if (debug_noise)
        cout<<"REDUCE "<<endl;
    vector<astnode*> tmp;
    for (int i = 0; i < X.rhs.size(); i++) {
        st.pop();
        if (!semStack.empty()) {
            auto m = semStack.top();
            if (m->token.getString() != "<nil>") {
                tmp.push_back(semStack.top());
            } else {
                delete m;
            }
            semStack.pop();
        } else {
            cout<<"Uh oh: Semantic Stack and Parse Stack have diverged"<<endl;
        }
    }
    reverse(tmp.begin(), tmp.end());
    if (X.actsym.empty() == false) {
        if (debug_noise) cout<<"And do: "<<X.actsym<<endl;
        string f = X.actsym.substr(1);
        semStack.push(actions.at(f)(tmp));
        if (debug_noise)
            preorder(semStack.top(), 1);
    } else {
        if (X.rhs.empty()) {
            semStack.push(new astnode(Token(TK_EOI, "Epsilon")));
        } else {
            semStack.push(tmp.front());
        }
    }
    int ns = nextState(goTab, st.top(), X.lhs);
    if (ns != -1) {
        st.push(goTab[st.top()][ns]);
    }
}
void Parser::printCurrent(int state_num, Token& T) {
    cout<<"[ state: "<<state_num<<"][ token: "<<tokenStr[T.getSymbol()]<<"]"<<actTab[state_num][nextState(actTab, state_num,T.getSymbol())]<<endl<<"Action: ";
}
bool Parser::checkAccept(int state_num, Token& T) {
    if (actTab[state_num] == NULL) {
        return false;
    }
    int N = actTab[state_num][0];
    for (int i = 1; i < 2*N+1; i+=2) {
        if (actTab[state_num][i] == DOLLARACCEPT && actTab[state_num][i+1] == 0) {
            if (debug_noise)
                cout<<"ACCEPT"<<endl;
            preorder(semStack.top(), 1);
            return true;
        }
    }
    return false;
}
astnode* Parser::parse(vector<Token>& tok) {
    tokens = tok;
    tpos = 0;
    st.push(0);
    for (;;) {
        Token curr_token = current();
        int curr_state = st.top();
        if (checkAccept(curr_state, curr_token)) {
            astnode* tmp = semStack.top();
            if (tmp->token.getString() == "Epsilon") {
                auto t = tmp;
                tmp = tmp->next;
                t->next = nullptr;
                delete t;
            }
            return tmp;
        }
        int ns = nextState(actTab, curr_state, curr_token.getSymbol());
        if (ns == -1) {
            cout<<"Hmm, no actions on '"<<tokenStr[curr_token.getSymbol()]<<"' from state "<<curr_state<<"?"<<endl;
            int nument = 2*actTab[curr_state][0]+1;
            for (int i = 1; i < nument; i+=2) {
                cout<<actTab[curr_state][i]<<endl;
            }
            cout<<"Bailing out."<<endl;
            return nullptr;
        } else {
            if (debug_noise)
                printCurrent(curr_state, curr_token);
            int next = actTab[curr_state][ns];
            if (next > 0) {
                    doShift(next);
            } else if (next < 0) {
                    Production p = prod[abs(next)];
                    doReduce(p);
            } else {
                if (checkAccept(curr_state, curr_token)) {
                    astnode* tmp = semStack.top();
                    if (tmp->token.getString() == "Epsilon") {
                        auto t = tmp;
                        tmp = tmp->next;
                        t->next = nullptr;
                        delete t;
                    }
                    return tmp;
                }
            }
        }
    }
    return nullptr;
}