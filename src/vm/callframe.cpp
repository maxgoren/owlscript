#include "callframe.hpp"

void freeAR(ActivationRecord* to) {
    if (to != nullptr && to->marked == false) {
        delete to;
    }
}