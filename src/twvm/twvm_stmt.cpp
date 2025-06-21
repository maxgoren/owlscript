#include "twvm.hpp"

void TWVM::letStatement(astnode* node) {
    string id; int depth;
    if (isExprType(node->child[0], ID_EXPR)) {
        id = node->child[0]->token.strval;
        depth = node->child[0]->token.depth;
    } else if (isExprType(node->child[0], ASSIGN_EXPR)) {
        astnode* x = node;
        while (!isExprType(x, ID_EXPR)) x = x->child[0];
        id = x->token.strval;
        depth = x->token.depth;
    }
    if (cxt.exists(id, depth)) {
        cout<<"Error: the variable name "<<id<<" already exists in this scope."<<endl;
        return;
    } else {
        cxt.put(id, depth, makeNil());
    }
    evalExpr(node->child[0]);
}

void TWVM::ifStatement(astnode* node) {
    evalExpr(node->child[0]);
    if (pop().data.boolval) {
        exec(node->child[1]);
    } else {
        exec(node->child[2]);
    }
}

void TWVM::breakStatement(astnode* node) {
    breakloop = true;
}

void TWVM::whileStatement(astnode* node) {
    evalExpr(node->child[0]);
    breakloop = false;
    while (pop().data.boolval) {
        exec(node->child[1]);
        if (breakloop) {
            break;
        }
        exec(node->child[0]);
    }
    breakloop = false;
}

void TWVM::foreachStatement(astnode* node) {
    evalExpr(node->child[1]);
    if (typeOf(peek(0)) == AS_LIST) {
        List* list = getList(peek(0));
        string itername = node->child[0]->token.strval;
        for (auto it = list->head; it != nullptr; it = it->next) {
            cxt.insert(itername, it->info);
            exec(node->child[2]);
        }
        pop();
        cxt.remove(itername);
    } else if (typeOf(peek(0)) == AS_STRING) {
        string itername = node->child[0]->token.strval;
        for (char c : *getString(peek(0))) {
            string tmpstr; tmpstr.push_back(c);
            cxt.insert(itername, cxt.getAlloc().makeString(tmpstr));
            exec(node->child[2]);
        }
        pop();
        cxt.remove(itername);
    } else {
        cout<<"Error: object isnt iterable"<<endl;
        pop();
    }
}

void TWVM::printStatement(astnode* node) {
    evalExpr(node->child[0]);
    cout<<toString(pop());
    if (node->token.symbol == TK_PRINTLN)
        cout<<endl;
}

void TWVM::defineFunction(astnode* node) {
    Function* func = new Function(copyTree(node->child[0]), copyTree(node->child[1]));
    func->name = node->token.strval;
    func->closure = cxt.getCallStack();
    Object m = cxt.getAlloc().makeFunction(func);
    cxt.insert(func->name, m);
}

void TWVM::defineStruct(astnode* node) {
    Struct* st = new Struct(node->child[0]->token.strval);
    int i = 0;
    for (astnode* it = node->child[1]; it != nullptr; it = it->next) {
        string fieldname = it->child[0]->token.strval;
        st->fields[fieldname] = makeNil();
        st->constructorOrder[i++] = fieldname; 
    }
    cxt.addStructType(st);
}

void TWVM::blockStatement(astnode* node) {
    cxt.openScope();
    exec(node->child[0]);
    cxt.closeScope();
}

void TWVM::expressionStatement(astnode* node) {
    evalExpr(node->child[0]);
}

void TWVM::returnStatement(astnode* node) {
    evalExpr(node->child[0]);
    bailout = true;
}