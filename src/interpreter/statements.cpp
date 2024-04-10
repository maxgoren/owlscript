#include "interpreter.hpp"

void Interpreter::letStmt(ASTNode* node) {
    enter("[LET STMT]");
    assignStmt(node->left);
    leave();
}

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
    } else {
        cout<<"Error: missing assignment value"<<endl;
        return;
    }
    if (!callStack.empty() && callStack.top()->env.find(name) == callStack.top()->env.end()) { 
        callStack.top()->env[name] = memStore.storeAtNextFree(value);
        leave();
        return;
    }
    int saveAddr = getAddress(name);
    if (saveAddr == 0) {
        saveAddr = memStore.storeAtNextFree(value);
        if (!callStack.empty()) {
            callStack.top()->env[name] = saveAddr;
        } else {
            st[name] = saveAddr;
        }
    } else {
        Object* obj = memStore.get(saveAddr);
        if (obj->type == AS_LIST) {
            int arrIndex = 0;
            if (node->left->left){
                arrIndex =  atoi(node->left->left->data.stringVal.c_str());
            }
            say("Index: " + to_string(arrIndex));
            ListNode* x = obj->list->head;
            if (arrIndex == 0) {
                x->data = value;
            } else {
                int i = 0;
                while (i < arrIndex) {
                    if (x->next != nullptr)
                        x = x->next;
                    else break;
                    i++;
                }
            }
            x->data = value;
        } else {
            obj = value;
        }
        memStore.store(saveAddr,obj);
        if (!callStack.empty()) {
            callStack.top()->env[name] = saveAddr;
        } else {
            st[name] = saveAddr;
        }
        
    }
    leave();
}

void Interpreter::defineFunction(ASTNode* node) {
    enter("define function");
    Procedure* np = new Procedure;
    np->name = node->data.stringVal;
    np->paramList = node->left;
    np->functionBody = node->right;
    if (!callStack.empty()) {
        callStack.top()->nestedProc[np->name] = np;
        say(np->name + " defined as nested procedure.");
    } else {
        procedures[np->name] = np;
        say(np->name + " defined globally.");
    }
    leave();
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