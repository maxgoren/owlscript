#include "interpreter.hpp"

void Interpreter::printStmt(ASTNode* node) {
    enter("[print]" + node->data.stringVal);
    Object* obj = expression(node->left);
    if (node->data.stringVal == "println") {
        cout<<toString(obj)<<endl;
    } else {
        cout<<toString(obj)<<flush;
    }
    leave();
}

void Interpreter::readStmt(ASTNode* node) {
    enter("[input]");
    string input;
    getline(cin, input);
    say("read in: " + input);
    int addr = getAddress(node->left->data.stringVal);
    if (addr == 0) {
        addr = memStore.storeAtNextFree(makeStringObject(new string(input)));
    } else {
        memStore.store(addr, makeStringObject(new string(input)));
    }
    leave();
}

void Interpreter::ifStmt(ASTNode* node) {
    enter("[if statement]");
    ASTNode* branch = nullptr;
    say("testing condition");
    auto result = expression(node->left)->boolVal;
    if (result) {
        say("executing matching result");
        branch = node->mid;
    } else {
        say("executing else clause");
        branch = node->right;
    }
    for (ASTNode* t = branch; t != nullptr; t = t->next ) {
        statement(t);
    }
    leave();
}

void Interpreter::loopStmt(ASTNode* node) {
    enter("[loop]");
    while (expression(node->left)->boolVal) {
        ASTNode* t = node->right;
        while (t != nullptr) {
            statement(t);
            t = t->next;
        }
    }
    leave();
}

void Interpreter::assignStmt(ASTNode* node) {
    enter("[assign]");
    string name;
    Object* value;
    if (node->left != nullptr) {
        name = node->left->data.stringVal;
    } else {
        cout<<"Error: missing name"<<endl;
        leave();
        return;
    }
    if (node->right != nullptr) {
        value = expression(node->right);
        int saveAddr = memStore.storeAtNextFree(value);
        if (!callStack.empty()) {
            callStack.top()->env[name] = saveAddr;
        } else {
            st[name] = saveAddr;
        }
    } else {
        cout<<"Error: missing assignment value"<<endl;
    }
    leave();
}

void Interpreter::defineFunction(ASTNode* node) {
    enter("define function");
    Procedure* np = new Procedure;
    np->name = node->data.stringVal;
    np->paramList = node->left;
    np->functionBody = node->right;
    procedures[np->name] = np;
    leave(np->name + " defined.");
}

void Interpreter::returnStmt(ASTNode* node) {
    enter("[return]");
    if (!callStack.empty()) {
        callStack.top()->returnValue = expression(node->left);
        say("Returning: " + toString(callStack.top()->returnValue));
        stopProcedure = true;
    }
    leave();
}