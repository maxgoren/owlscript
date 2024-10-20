#ifndef regexpengine_hpp
#define regexpengine_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

template <class T>
class Stack {
    private:
        struct snode {
            T info;
            snode* next;
            snode(T i, snode* n) : info(i), next(n) { }
        };
        using link = snode*;
        link head;
        int count;
        void init() {
            head = nullptr;
            count = 0;
        }
    public:
        Stack() {
            init();
        }
        Stack(const Stack& st) {
            init();
            snode d(T(), nullptr); link c = &d;
            for (link it = st.head; it != nullptr; it = it->next) {
                c->next = new snode(it->info, nullptr);
                c = c->next;
            }
            head = d.next;
            count = st.count;
        }
        ~Stack() {
            clear();
        }
        bool empty() const {
            return head == nullptr;
        }
        int size() const {
            return count;
        }
        void push(T info) {
            head = new snode(info, head);
            count++;
        }
        T& top() {
            return head->info;
        }
        T pop() {
            T ret = head->info;
            link x = head;
            head = head->next;
            x->next = x;
            delete x;
            count--;
            return ret;
        }
        void clear() {
            while (!empty()) pop();
        }
        Stack& operator=(const Stack& st) {
            if (this != &st) {
                init();
                snode d(T(), nullptr); link c = &d;
                for (link it = st.head; it != nullptr; it = it->next) {
                    c->next = new snode(it->info, nullptr);
                    c = c->next;
                }
                head = d.next;
                count = st.count;
            }
            return *this;
        }
};

enum RegExSymbol {
    RE_CHAR, RE_LPAREN, RE_RPAREN, RE_LSQUARE, RE_RSQUARE, 
    RE_STAR, RE_PLUS, RE_QUESTION, RE_CONCAT, RE_OR, RE_SPECIFIEDSET, 
    RE_SPECIFIEDRANGE, RE_QUANTIFIER, RE_NONE
};

vector<string> reSymStr = { 
    "TK_CHAR", "TK_LPAREN", "TK_RPAREN", "RE_LSQUARE", "RE_RSQUARE", 
    "RE_STAR", "RE_PLUS", "RE_QUESTION", "RE_CONCAT", "RE_OR", "RE_SPECIFIEDSET", "RE_SPECIFIEDRANGE", "RE_QUANTIFIER", "TK_NONE"
};

struct RegExToken {
    RegExSymbol symbol;
    string charachters;
    RegExToken(RegExSymbol s = RE_NONE, string ch = "nil") : symbol(s), charachters(ch) { }

};

class Tokenizer {
    private:
        bool isChar(char c) {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '.';
        }
        void setToken(RegExToken& nt, char ch, RegExSymbol sym) {
            string buff;
            buff.push_back(ch);
            nt.symbol = sym;
            nt.charachters = buff;
        }
        void setSpecified(RegExToken& nt, string re, int& idx) {
            string buff;
            idx++;
            bool isRange = false;
            while (idx < re.length() && re[idx] != ']') {
                if (re[idx] == '-') 
                    isRange = true;
                buff.push_back(re[idx++]);
            }
            nt.charachters = buff;
            nt.symbol = isRange ? RE_SPECIFIEDRANGE:RE_SPECIFIEDSET;
        }
        void setQuantifier(RegExToken& nt, string re, int& idx) {
            string buff;
            idx++;
            while (idx < re.length() && re[idx] != '}') {
                if (!isdigit(re[idx])) {
                    cout<<"Error: expected a number"<<endl;
                    return;
                }
                buff.push_back(re[idx++]);
            }
            nt.charachters = buff;
            nt.symbol = RE_QUANTIFIER;
        }
    public:
        vector<RegExToken> tokenize(string re) {
            vector<RegExToken> tokens;
            for (int i = 0; i < re.size(); i++) {
                RegExToken nt;
                if (isdigit(re[i]) || isChar(re[i])) {
                    setToken(nt, re[i], RE_CHAR);
                } else {
                    switch (re[i]) {
                        case '@': { setToken(nt, re[i], RE_CONCAT); } break;
                        case '|': { setToken(nt, re[i], RE_OR); } break;
                        case '+': { setToken(nt, re[i], RE_PLUS); } break;
                        case '*': { setToken(nt, re[i], RE_STAR); } break;
                        case '?': { setToken(nt, re[i], RE_QUESTION); } break;
                        case '(': { setToken(nt, re[i], RE_LPAREN); } break;
                        case ')': { setToken(nt, re[i], RE_RPAREN); } break;
                        case '[': { setSpecified(nt, re, i); } break;
                        case ']': break;
                        case '{': { setQuantifier(nt, re, i); } break;
                        case '}': break;
                        default:
                            break;
                    }
                }
                tokens.push_back(nt);
            }
            return tokens;
        }
};

class RegularExpression {
    public:
        virtual RegExToken getSymbol() = 0;
        virtual RegularExpression* getLeft() = 0;
        virtual RegularExpression* getRight() = 0;
}; 

class ExpressionLiteral : public RegularExpression {
    private:
        RegExToken symbol;
    public:
        ExpressionLiteral(RegExToken sym) { symbol = sym; }
        RegularExpression* getLeft() {
            return nullptr;
        }
        RegularExpression* getRight() {
            return nullptr;
        }
        RegExToken getSymbol() {
            return symbol;
        }
};

class ExpressionOperator : public RegularExpression {
    private:
        RegularExpression* left;
        RegularExpression* right;
        RegExToken symbol;
    public:
        ExpressionOperator(RegExToken c, RegularExpression* ll, RegularExpression* rr) {
            symbol = c;
            left = ll;
            right = rr;
        }
        RegExToken getSymbol() {
            return symbol;
        }
        RegularExpression* getLeft() {
            return left;
        }
        RegularExpression* getRight() {
            return right;
        }
};

void traverse(RegularExpression* h, int d) {
    if (h != nullptr) {
        traverse(h->getLeft(), d+1);
        for (int i = 0; i < d; i++) cout<<"  ";
        cout<<h->getSymbol().charachters<<endl;
        traverse(h->getRight(), d+1);
    }
}

bool isOp(char c) {
    switch (c) {
        case '|': case '@': case '?': case '+': case '*':
            return true;
        default:
            break;
    }
    return false;
}

bool isOp(RegExToken c) {
    switch (c.symbol) {
        case RE_STAR:
        case RE_PLUS:
        case RE_QUESTION: 
        case RE_CONCAT:
        case RE_OR: return true;
        default:
            break;
    }
    return false;
}

class RegExParser {
    private:
        RegularExpression* makeTree(vector<RegExToken> postfix) {
            Stack<RegularExpression*> sf;
            for (RegExToken c : postfix) {
                if (!isOp(c)) {
                    sf.push(new ExpressionLiteral(c));
                } else {
                    auto right = sf.empty() ? nullptr:sf.pop();
                    auto left = sf.empty() ? nullptr:sf.pop();
                    sf.push(new ExpressionOperator(c, left, right));
                }
            }
            return sf.pop();
        }
        int precedence(RegExToken c) {
            switch (c.symbol) {
                case RE_STAR:
                case RE_PLUS:
                case RE_QUESTION: return 50;
                case RE_CONCAT: return 30;
                case RE_OR: return 20;
                default:
                    break;
            }
            return 10;
        }
        bool leftAssociative(RegExToken c) {
            switch (c.symbol) {
                 case RE_STAR:
                case RE_PLUS:
                case RE_QUESTION: 
                case RE_CONCAT:
                case RE_OR: return true;
                default:
                    break;
            }
            return false;
        }
        string addConcatOp(string str) {
            string fixed;
            bool inset = false;
            for (int i = 0; i < str.length(); i++) {
                fixed.push_back(str[i]);
                if (str[i] == '(' || str[i] == '|')
                    continue;
                if (str[i] == '[') {
                    inset = true;
                    continue;
                }
                if (str[i] == ']') {
                    inset = false;
                }
                if (i+1 < str.length() && inset == false) {
                    char p = str[i+1];
                    if (p == '|' || p == '*' || p == '+' || p == ')' || p == '?' || p == ']')
                        continue;
                    fixed.push_back('@');
                }
            }
            return fixed;
        }
        vector<RegExToken> in2post(vector<RegExToken> str) {
            Stack<RegExToken> ops;
            vector<RegExToken> postfix;
            for (int i = 0; i < str.size(); i++) {
                if (str[i].symbol == RE_LPAREN) {
                    ops.push(str[i]);
                } else if (isOp(str[i])) {
                        if (precedence(str[i]) < precedence(ops.top()) || (precedence(str[i]) == precedence(ops.top()) && leftAssociative(str[i]))) {
                            RegExToken c = ops.pop();
                            postfix.push_back(c);
                            ops.push(str[i]);
                        } else {
                            ops.push(str[i]);
                        }
                } else if (str[i].symbol == RE_RPAREN) {
                    while (!ops.empty()) {
                        RegExToken c = ops.pop();
                        if (c.symbol == RE_LPAREN)
                            break;
                        else postfix.push_back(c);
                    }
                } else {
                    postfix.push_back(str[i]);
                }
            }
            while (!ops.empty()) {
                RegExToken c = ops.pop();
                if (c.symbol != RE_LPAREN)
                    postfix.push_back(c);
            }
            return postfix;
        }
    public:
        RegExParser() {

        }
        RegularExpression* parse(string regexp) {
            Tokenizer tz;
            cout<<"Parsing: "<<regexp<<endl;
            regexp = addConcatOp(regexp);
            auto tokens =  tz.tokenize(regexp);
            int i = 0;
            vector<RegExToken> postfix = in2post(tokens);
            cout<<"Postfix: \n";
            for (auto m : postfix) {
                cout<<"("<<i++<<"): "<<m.charachters<<" - "<<reSymStr[m.symbol]<<endl;
            }
            cout<<endl;
            return makeTree(postfix);
        }
};

class Edge {
    public:
        virtual RegExToken getLabel() = 0;
        virtual bool matches(char c) = 0;
        virtual bool positionIs(int index) = 0;
        virtual bool isEpsilon() = 0;
        virtual ~Edge() { }
};

class CharEdge : public Edge {
    private:
        RegExToken label;
        bool checkInRange(char c) {
            char lo, hi;
            bool is_good = false;
            for (int i = 1; i < label.charachters.size()-1; i++) {
                if (i+1 < label.charachters.size() && label.charachters[i] == '-') {
                    lo = label.charachters[i-1];
                    hi = label.charachters[i+1];
                    if (c >= lo && c <= hi) {
                        is_good = true;
                        break;
                    }
                }
            }
            return is_good;
        }
    public:
        CharEdge(RegExToken c) {
            label = c;
        }
        ~CharEdge() {

        }
        bool isEpsilon() {
            return false;
        }
        bool positionIs(int i) {
            return true;
        }
        bool matches(char c) {
            if (label.symbol == RE_SPECIFIEDSET) {
                for (char m : label.charachters) {
                    if (c == m)
                        return true;
                }
                return false;
            } else if (label.symbol == RE_SPECIFIEDRANGE) {
                return checkInRange(c);
            }
            return label.charachters[0] == c;
        }
        RegExToken getLabel() {
            return label;
        }
};

class EpsilonEdge : public Edge {
    public:
        EpsilonEdge() { }
        ~EpsilonEdge() { }
        bool matches(char c) {
            return true;
        }
        bool positionIs(int i) {
            return true;
        }
        bool isEpsilon() {
            return true;
        }
        RegExToken getLabel() {
            return RegExToken(RE_NONE, "&");
        }
};


typedef int State;

struct Transition {
    State from;
    State to;
    Edge* edge;
    Transition(State s, State t, Edge* e) {
        from = s; to = t; edge = e;
    }
    Transition(const Transition& t) {
        from = t.from;
        to = t.to;
        if (t.edge->isEpsilon()) {
            edge = new EpsilonEdge();
        } else {
            edge = new CharEdge(t.edge->getLabel());
        }
    }
    ~Transition() {
        delete edge;
    }
    Transition& operator=(const Transition& t) {
        if (this != &t) {
            from = t.from;
            to = t.to;
            if (t.edge->isEpsilon()) {
                edge = new EpsilonEdge();
            } else {
                edge = new CharEdge(t.edge->getLabel());
            }
        }
        return *this;
    }
};

bool operator==(const Transition& s, const Transition& t) {
    return s.from == t.from && s.to == t.to && s.edge == t.edge;
}

bool operator!=(const Transition& s, const Transition& t) {
    return !(s == t);
}

namespace std {
    template <> struct hash<Transition> {
        std::size_t operator()(const Transition& t) const noexcept {
            string tmp = to_string(t.from);
            tmp += t.edge->getLabel().charachters;
            tmp += to_string(t.to);
            return std::hash<string>()(tmp);
        }
    };
}

class NFA {
    private:
        State start;
        State accept;
        unordered_map<State, vector<Transition>> states;
    public:
        NFA() {
            start = 0;
            accept = 0;
        }
        NFA(const NFA& nfa) {
            start = nfa.start;
            accept = nfa.accept;
            for (auto m : nfa.states) {
                makeState(m.first);
                for (auto t : m.second) {
                    addTransition(t);
                }
            }
        }
        void makeState(State name) {
            if (states.find(name) == states.end()) {
                states.insert(make_pair(name, vector<Transition>()));
            }
        }
        void setStart(State ss) {
            start = ss;
        }
        void setAccept(State as) {
            accept = as;
        }
        State getStart() {
            return start;
        }
        State getAccept() {
            return accept;
        }
        void addTransition(Transition t) {
            states[t.from].push_back(t);
        }
        int size() {
            return states.size();
        }
        unordered_map<State, vector<Transition>>& getStates() {
            return states;
        }
        vector<Transition>& getTransitions(State state) {
            return states[state];
        }
        NFA& operator=(const NFA& nfa) {
            if (this != &nfa) {
                start = nfa.start;
                accept = nfa.accept;
                for (auto m : nfa.states) {
                    makeState(m.first);
                    for (auto t : m.second) {
                        addTransition(t);
                    }
                }
            }
            return *this;
        }
};

class NFACompiler {
    private:
        int l;
        Stack<NFA> nfaStack;
        int makeStateLabel() {
            return l++;
        }
        void initNextNFA(NFA& nnfa) {
            int nstart = makeStateLabel();
            nnfa.makeState(nstart);
            nnfa.setStart(nstart);
            int nend = makeStateLabel();
            nnfa.makeState(nend);
            nnfa.setAccept(nend);
        }
        void copyTransitions(NFA& nnfa, NFA onfa) {
            for (auto state : onfa.getStates()) {
                nnfa.makeState(state.first);
                for (auto trans : onfa.getTransitions(state.first)) {
                    nnfa.addTransition(trans);
                }
            }
        }
        NFA singleTransitionNFA(Edge* edge) {
            NFA nfa;
            initNextNFA(nfa);
            nfa.addTransition(Transition(nfa.getStart(), nfa.getAccept(), edge)); 
            return nfa;
        }

        NFA emptyExpr() {
            return singleTransitionNFA(new EpsilonEdge());
        }
        NFA atomicNFA(RegExToken c) {
            return singleTransitionNFA(new CharEdge(c));
        }
        NFA concatNFA(NFA first, NFA second) {
            NFA nnfa;
            copyTransitions(nnfa, first);
            copyTransitions(nnfa, second);
            nnfa.setStart(first.getStart());
            nnfa.setAccept(second.getAccept());
            nnfa.addTransition(Transition(first.getAccept(), second.getStart(), new EpsilonEdge()));
            return nnfa;
        }
        NFA kleeneNFA(NFA torepeat, bool mustMatch) {
            NFA nnfa;
            initNextNFA(nnfa);
            copyTransitions(nnfa, torepeat);
            nnfa.addTransition(Transition(torepeat.getAccept(), nnfa.getStart(), new EpsilonEdge()));
            nnfa.addTransition(Transition(nnfa.getStart(), torepeat.getStart(), new EpsilonEdge()));
            nnfa.addTransition(Transition(torepeat.getAccept(), nnfa.getAccept(), new EpsilonEdge()));
            if (!mustMatch)
                nnfa.addTransition(Transition(nnfa.getStart(), nnfa.getAccept(), new EpsilonEdge()));
            return nnfa;
        }
        NFA alternateNFA(NFA first, NFA second) {
            //create new NFA with start and end state.
            NFA nnfa;
            initNextNFA(nnfa);
            //copy states and transitions from both NFA's to new NFA
            copyTransitions(nnfa, first);
            copyTransitions(nnfa, second);
            //Add new E-transitions from new start state to first and second NFAs
            nnfa.addTransition(Transition(nnfa.getStart(), first.getStart(), new EpsilonEdge()));
            nnfa.addTransition(Transition(nnfa.getStart(), second.getStart(), new EpsilonEdge()));
            //Add new E-transitions from first and second accept state to new accept state.
            nnfa.addTransition(Transition(first.getAccept(), nnfa.getAccept(), new EpsilonEdge()));
            nnfa.addTransition(Transition(second.getAccept(), nnfa.getAccept(), new EpsilonEdge()));
            return nnfa;
        }
        NFA zeroOrOnce(NFA onfa) {
            NFA nnfa;
            initNextNFA(nnfa);
            copyTransitions(nnfa, onfa);
            //wire in match choice
            nnfa.addTransition(Transition(nnfa.getStart(), onfa.getStart(), new EpsilonEdge()));
            nnfa.addTransition(Transition(onfa.getAccept(), nnfa.getAccept(), new EpsilonEdge()));
            //wire in epsilon choice.
            nnfa.addTransition(Transition(nnfa.getStart(), nnfa.getAccept(), new EpsilonEdge()));
            return nnfa;
        }
        NFA buildOperatorNFA(RegularExpression* ast) {
            switch (ast->getSymbol().charachters[0]) {
                case '@': {
                    NFA b = nfaStack.pop();
                    NFA a = nfaStack.pop();
                    return concatNFA(a, b);
                }
                case '|': {
                    NFA b = nfaStack.pop();
                    NFA a = nfaStack.pop();
                    return alternateNFA(a, b);
                }
                case '*': {
                    NFA a = nfaStack.pop();
                    return kleeneNFA(a, false);
                }
                case '+': {
                    NFA a = nfaStack.pop();
                    return kleeneNFA(a, true);
                }
                case '?': {
                    NFA a = nfaStack.pop();
                     return zeroOrOnce(a);
                }
                default:
                    break;
            }
            return NFA();
        }
        void gen_nfa(RegularExpression* ast) {
            if (ast != nullptr) {
                gen_nfa(ast->getLeft());
                gen_nfa(ast->getRight());
                if (!isOp(ast->getSymbol())) {
                    nfaStack.push(atomicNFA(ast->getSymbol()));
                } else {
                    nfaStack.push(buildOperatorNFA(ast));
                }
            }
        }
        
    public:
        NFACompiler() {
            l = 0;
        }
        NFA compile(string pattern) {
            RegExParser parser;
            string re = "(" + pattern + ")";
            auto ast = parser.parse(re);
            traverse(ast, 1);
            gen_nfa(ast);
            return nfaStack.pop();
        }
};

class RegExPatternMatcher {
    private:
        NFA nfa;
        bool trace;
        //Gathers a list of states reachable from those in clist, which have transition which consumes ch
        unordered_set<State> move(unordered_set<State> clist, char ch) {
            unordered_set<State> nlist;
            if (trace)
                cout<<ch<<": "<<endl;
            for (State s : clist) {
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->matches(ch) || t.edge->matches('.')) {
                        if (t.edge->isEpsilon() == false) {
                           if (trace) cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
                            nlist.insert(t.to);
                        } 
                    }
                }
            }
            return nlist;
        }
        //An interesting adaptation of Depth First Search.
        //Gathers a list of states reachable from those in clist using _only_ epsilon transitions.
        unordered_set<State> e_closure(unordered_set<State> clist) {
            unordered_set<State> nlist = clist;
            Stack<State> sf;
            for (State s : clist)
                sf.push(s);
            while (!sf.empty()) {
                State s = sf.pop();
                for (Transition t : nfa.getTransitions(s)) {
                    if (t.edge->isEpsilon()) {
                        if (nlist.find(t.to) == nlist.end()) {
                            if (trace) cout<<'\t'<<t.from<<" - ("<<t.edge->getLabel().charachters<<") ->"<<t.to<<endl;
                            nlist.insert(t.to);
                            sf.push(t.to);
                        }
                    }
                }
            }
            return nlist;
        }
    public:
        RegExPatternMatcher(NFA& nf, bool loud = false) : nfa(nf), trace(loud) {

        }
        void setNFA(NFA& fa) {
            nfa = fa;
        }
        bool match(string text) {
            unordered_set<State> curr, next;
            next.insert(nfa.getStart());
            curr = e_closure(next);
            for (int i = 0; i < text.length(); i++) {
                next = move(curr, text[i]);
                curr = e_closure(next);
            }
            return curr.find(nfa.getAccept()) != curr.end();
        }
};

#endif