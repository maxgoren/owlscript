#include "interpreter.hpp"


Object* Interpreter::listSize(ASTNode* node) {
    enter("[list size]");
    Object* obj = nullptr;
    int addr, size;
    if (node->left->kind == EXPRNODE && node->left->type.expr == LIST_EXPR) {
        obj = listExpr(node->left);
    } else {
        string name = node->left->data.stringVal;
        if (builtIns.find(name) != builtIns.end()) {
            cout<<"Hit the built in"<<endl;
            obj = callBuiltIn(node->left);
        } else {
            addr = getAddress(name);
            if (addr == 0) {
                cout<<"Error: No List named "<<name<<" found."<<endl;
                return makeNilObject();
            }
            obj = memStore.get(addr);
        }
    }
    if (obj != nullptr) {
        size = obj->list->size;
    }
    leave();
    return makeIntObject(size);
}

Object* Interpreter::carExpr(ASTNode* node) {
    enter("[carExpr]");
    Object* obj;
    int addr;
    if (node->left->kind == EXPRNODE && node->left->type.expr == LIST_EXPR) {
        obj = listExpr(node->left);
    } else {
        string name = node->left->data.stringVal;
        if (builtIns.find(name) != builtIns.end()) {
            cout<<"Hit the built in"<<endl;
            obj = callBuiltIn(node->left);
        } else {
            addr = getAddress(name);
            if (addr == 0) {
                cout<<"Error: No List named "<<name<<" found."<<endl;
                return makeNilObject();
            }
            obj = memStore.get(addr);
        }
    }
    if (obj != nullptr) {
        return obj->list->head->data;
    }
    leave();
    return makeNilObject();
}

Object* Interpreter::cdrExpr(ASTNode* node) {
    enter("[cdrExpr]");
    Object* obj = nullptr;
    int addr;
    if (node->left->kind == EXPRNODE && node->left->type.expr == LIST_EXPR) {
        obj = listExpr(node->left);
    } else {
        string name = node->left->data.stringVal;
        if (builtIns.find(name) != builtIns.end()) {
            cout<<"Hit the built in"<<endl;
            obj = callBuiltIn(node->left);
        } else {
            addr = getAddress(name);
            if (addr == 0) {
                cout<<"Error: No List named "<<name<<" found."<<endl;
                return makeNilObject();
            }
            obj = memStore.get(addr);
        }
    }
    if (obj != nullptr && obj->type == AS_LIST) {
        ListHeader* cdr = new ListHeader;
        cdr->size = obj->list->size - 1;
        cdr->head = obj->list->head->next;
        return makeListObject(cdr);
    }
    leave();
    return makeNilObject();
}


Object* Interpreter::sortList(ASTNode* node) {
    enter("[sort]");
    Object* obj;
    int addr;
    if (node->left->kind == EXPRNODE && node->left->type.expr == LIST_EXPR) {
        obj = listExpr(node->left);
    } else {
        string name = node->left->data.stringVal;
        if (builtIns.find(name) != builtIns.end()) {
            cout<<"Hit the built in"<<endl;
            obj = callBuiltIn(node->left);
        } else {
            addr = getAddress(name);
            if (addr == 0) {
                cout<<"Error: No List named "<<name<<" found."<<endl;
                return makeNilObject();
            }
            obj = memStore.get(addr);
        }
    }
    ListHeader* list = new ListHeader;
    list->head = nullptr;
    list->size = 0;
    if (obj != nullptr && obj->type == AS_LIST) {
        list->head = copyList(obj->list->head); //sort returns a copy of the sorted list, leaving original as-is.
        list->head = mergeSortList(list->head);
        list->size = obj->list->size;
    }
    leave();
    return makeListObject(list);
}

//I don't know how I feel about this.
//It has a blinking in and out of reality feel 
Object* Interpreter::mapExpr(ASTNode* node) {
    Object* lambdaObj = lambdaExpr(node->left);
    Object* listObj = nullptr;
    if (node->right->kind == EXPRNODE && node->right->type.expr == LIST_EXPR) {
        listObj = listExpr(node->right);
    } else {
        string name = node->right->data.stringVal;
        if (builtIns.find(name) != builtIns.end()) {
            cout<<"Hit the built in"<<endl;
            listObj = callBuiltIn(node->left);
        } else {
            int addr = getAddress(name);
            if (addr == 0) {
                cout<<"Error: No List named "<<name<<" found."<<endl;
                return makeNilObject();
            }
            listObj = memStore.get(addr);
        }
    }
    ListHeader* resultList = new ListHeader;
    resultList->size = 0;
    resultList->head = nullptr;
    ListNode d;
    ListNode* tmpR = &d;
    for (auto it = listObj->list->head; it != nullptr; it = it->next) {
        ASTNode* t = new ASTNode;
        t->left = makeExprNode(CONST_EXPR, NUMBER, toString(it->data));
        t->right = nullptr; t->mid = nullptr; t->next = nullptr;
        auto result = runClosure(t, lambdaObj);
        tmpR->next = new ListNode;
        tmpR->next->data = result;
        tmpR = tmpR->next;
        tmpR->next = nullptr;
    }
    resultList->head = d.next;
    return makeListObject(resultList);
}

Object* Interpreter::getListItem(ASTNode* node, Object* listObj) {
    enter("listEntry");
    if (node->left == nullptr)
        return listObj;
    Object* tmp = expression(node->left);
    int arrIndex = tmp->realVal;
    say("Index: " + to_string(arrIndex));
    ListNode* x = listObj->list->head;
    if (arrIndex == 0)
        return x->data;
    int i = 0;
    while (i < arrIndex) {
        if (x->next != nullptr)
            x = x->next;
        else break;
        i++;
    }
    leave();
    return x->data;
}

void Interpreter::pushList(ASTNode* node) {
    enter("[push list]");
    Object* listObj;
    int addr;
    auto name = node->left->data.stringVal;
    say("list: " + name);
    Object* value = expression(node->right);
    say("push: " + toString(value));
    ListNode *toAdd = new ListNode;
    toAdd->data = value;
    toAdd->next = nullptr;
    addr = getAddress(name);
    if (addr > 0) {
        addr = st[name];
        listObj = memStore.get(addr);
        toAdd->next = listObj->list->head;
        listObj->list->head = toAdd;
        listObj->list->size += 1;
        memStore.store(addr, listObj);
    }
    leave();
}

void Interpreter::appendList(ASTNode* node) {
    enter("[append list]");
    Object* listObj;
    int addr;
    auto name = node->left->data.stringVal;
    say("list: " + name);
    Object* value = expression(node->right);
    say("push: " + toString(value));
    ListNode *toAdd = new ListNode;
    toAdd->data = value;
    toAdd->next = nullptr;
    addr = getAddress(name);
    if (addr > 0) {
        addr = st[name];
        listObj = memStore.get(addr);
        ListNode* itr = listObj->list->head;
        if (itr == nullptr) {
            listObj->list->head = toAdd;
        } else {
            while (itr->next != nullptr) itr = itr->next;
            itr->next = toAdd;
        }
        listObj->list->size += 1;
        memStore.store(addr, listObj);
    }
    leave();
}

Object* Interpreter::callBuiltIn(ASTNode* node) {
    string name = node->data.stringVal;
    cout<<name<<endl;
    if (name == "first")
        return carExpr(node);
    if (name == "rest")
        return cdrExpr(node);
    if (name == "sort")
        return sortList(node);
    if (name == "map")
        return mapExpr(node);
    return makeNilObject();
}

void Interpreter::popList(ASTNode* node) {
    enter("[pop List]");
    Object* obj;
    int addr;
    string name = node->left->data.stringVal;
    if (builtIns.find(name) != builtIns.end()) {
        cout<<"Hit the built in"<<endl;
        obj = callBuiltIn(node->left);
    } else {
        addr = getAddress(name);
        if (addr == 0) {
            cout<<"Error: No List named "<<name<<" found."<<endl;
            return;
        }
        obj = memStore.get(addr);
    }
    ListNode* x = obj->list->head;
    Object* result;
    if (x != nullptr) {
        obj->list->head = obj->list->head->next;
        result = x->data;
        obj->list->size -= 1;
        memStore.store(addr, obj);
    }
    leave();
}