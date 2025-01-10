#include "token.hpp"

bool isRelOp(Symbol s) {
    switch (s) {
        case TK_LT:
        case TK_GT:
        case TK_LTE:
        case TK_GTE:
        case TK_LOGIC_AND:
        case TK_LOGIC_OR:
            return true;
    }
    return false;
}

bool isEqualityOp(Symbol s) {
    switch (s) {
        case TK_EQU:
        case TK_NOTEQU:
            return true;
        default: break;
    }
    return false;
}