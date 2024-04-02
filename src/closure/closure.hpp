#ifndef CLOSURE_HPP
#define CLOSURE_HPP
#include <iostream>
#include <unordered_map>
#include "../ast/ast.hpp"
using namespace std;

typedef unordered_map<string, int> Environment;

struct Lambda {
    ASTNode* paramList;
    ASTNode* functionBody;
    Environment env;
    bool isClosure;
};

Lambda* makeLambda(ASTNode* funcBody, ASTNode* paramList, Environment env, bool isClosure);

#endif