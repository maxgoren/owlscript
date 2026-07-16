#include "callframe.hpp"

void freeAR(ActivationRecord* to) {
    if (to != nullptr) {
        delete to;
    }
}