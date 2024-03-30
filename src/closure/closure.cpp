#include "closure.hpp"

Lambda* makeLambda(ASTNode* funcBody, ASTNode* paramList, Environment env, bool asClose) {
    Lambda* clos = new Lambda;
    clos->paramList = paramList;
    clos->functionBody = funcBody;
    for (auto m : env) {
        clos->env[m.first] = m.second;
    }
    clos->isClosure = asClose;
    return clos;
}