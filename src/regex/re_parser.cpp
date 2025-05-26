#include "re_parser.hpp"

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
        case RE_QUANTIFIER:
        case RE_CONCAT:
        case RE_OR: return true;
        default:
            break;
    }
    return false;
}