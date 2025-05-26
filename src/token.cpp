#include "token.hpp"

void printToken(Token tk) {
    cout<<"["<<symbolStr[tk.symbol]<<", "<<tk.strval<<"]"<<endl;
}