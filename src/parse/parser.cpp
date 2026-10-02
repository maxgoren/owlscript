#include "parser.hpp"
using namespace std;

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
int Parser::getNext(const int *table[], int state, int sym) {
    int N = table[state][0];
    for (int i = 1; i < 2*N+1; i+=2) {
        if (table[state][i] == sym || sym == TK_EOI && table[state][i] == DOLLARACCEPT) {
            return i+1;
        }
    }
    return PARSE_ERR;
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

vector<astnode*> Parser::removeFromStack(int numSym) {
    vector<astnode*> tmp;
    for (int i = 0; i < numSym; i++) {
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
    return tmp;
}

void Parser::doReduce(int next) {
   if (debug_noise)
        cout<<"REDUCE "<<endl;
    Production X = prod[abs(next)];
    vector<astnode*> tmp = removeFromStack(X.rhs.size());
    if (!X.actsym.empty()) {
        if (debug_noise) cout<<"And do: "<<X.actsym<<endl;
        string f = X.actsym.substr(1);
        semStack.push(actions.at(f)(tmp));
        if (debug_noise) preorder(semStack.top(), 1);
    } else {
        if (X.rhs.empty()) {
            semStack.push(new astnode(Token(TK_EOI, "Epsilon")));
        } else {
            semStack.push(tmp.front());
        }
    }
    int ns = getNext(goTab, st.top(), X.lhs);
    if (ns != PARSE_ERR) {
        st.push(goTab[st.top()][ns]);
    }
}
void Parser::printCurrent(int state_num, Token& T) {
    cout<<"[ state: "<<state_num<<"][ token: "<<tokenStr[T.getSymbol()]<<"]"<<endl<<"Action: ";
}

astnode* Parser::cleanUpAndAccept() {
    astnode* tmp = semStack.top();
    if (tmp->token.getString() == "Epsilon") {
        auto t = tmp;
        tmp = tmp->next;
        t->next = nullptr;
        delete t;
    }
    return tmp;
}

astnode* Parser::syntaxError(int curr_state, Token curr_token) {
    cout<<"Hmm, no actions on '"<<tokenStr[curr_token.getSymbol()]<<"' from state "<<curr_state<<"?"<<endl;
    int nument = 2*actTab[curr_state][0]+1;
    for (int i = 1; i < nument; i+=2) {
        cout<<actTab[curr_state][i]<<endl;
    }
    cout<<"Bailing out."<<endl;
    while (!semStack.empty()) {
        auto t = semStack.top();
        semStack.pop();
        cleanUpAST(t);
    }
    return nullptr;
}

astnode* Parser::parse(vector<Token>& tok) {
    tokens = tok;
    tpos = 0;
    st.push(0);
    for (;;) {
        Token curr_token = current();
        int curr_state = st.top();
        int action = getNext(actTab, curr_state, curr_token.getSymbol());
        if (action == PARSE_ERR) {
           return syntaxError(curr_state, curr_token);
        } else {
            if (debug_noise) {
                printCurrent(curr_state, curr_token);
            }
            int rule = actTab[curr_state][action];
            if (rule > 0) {
                doShift(rule);
            } else if (rule < 0) {
                doReduce(rule);
            } else {
                return cleanUpAndAccept();
            }
        }
    }
    return nullptr;
}