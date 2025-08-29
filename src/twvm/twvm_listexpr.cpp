#include "twvm.hpp"
using namespace std;

void TWVM::subscriptAssignment(astnode* node) {
    astnode* tnode = node->child[0];
    evalExpr(tnode->child[0]);
    if (peek(0).type == AS_LIST) {
        List* list = getList(pop());
        evalExpr(tnode->child[1]);
        int pos = 0;
        int indx = pop().data.intval;
        ListNode* itr = list->head;
        while (itr != nullptr && pos < indx) {
            pos++;
            itr = itr->next;
        }
        evalExpr(node->child[1]);
        if (itr != nullptr) itr->info = pop();
        if (list->persist) { }
    } else if (peek(0).type == AS_STRUCT) {
        Struct* st = getStruct(pop());
        string name = tnode->child[1]->token.strval;
        if (st->fields.find(name) == st->fields.end()) {
            cout<<"Object doesnt have field '"<<name<<"'"<<endl;
            return;
        }
        evalExpr(node->child[1]);
        st->fields[name] = pop();
    } else if (peek(0).type == AS_STRING) {
        Object strObj = pop();
        string* str = getString(strObj);
        evalExpr(tnode->child[1]);
        int indx = getInteger(pop());
        if (indx < 0 || indx > str->length()) {
            cout<<"Index out of range: "<<indx<<endl;
            return;
        }
        string back = str->substr(indx+1);
        string front = str->substr(0, indx);
        evalExpr(node->child[1]);
        string toins = *getString(pop());
        str = new string(front+toins+back);
        strObj.data.gcobj->strval = str;
        push(strObj);
    }
}

void TWVM::subscriptExpression(astnode* node) {
    evalExpr(node->child[0]);
    if (peek(0).type == AS_LIST) {
        List* list = getList(pop());
        evalExpr(node->child[1]);
        int i = 0;
        int indx = pop().data.intval;
        ListNode* itr = list->head;
        while (itr != nullptr && i < indx) {
            i++;
            itr = itr->next;
        }
        if (itr != nullptr) push(itr->info);
    } else if (peek(0).type == AS_STRUCT) {
        Struct* st = getStruct(pop());
        string name = node->child[1]->token.strval;
        if (st->fields.find(name) == st->fields.end()) {
            cout<<"Object doesnt have field '"<<name<<"'"<<endl;
            return;
        }
        push(st->fields[name]);
    } else if (peek(0).type == AS_STRING) {
        string* str = getString(pop());
        evalExpr(node->child[1]);
        int indx = getInteger(pop());
        if (indx < 0 || indx > str->length()) {
            cout<<"Index out of range: "<<indx<<endl;
            return;
        }
        char c = str->at(indx);
        string tmp;
        tmp.push_back(c);
        push(cxt.getAlloc().makeString(tmp));
    }
}

void TWVM::fileExpression(astnode* node) {
    evalExpr(node->child[0]);
    string filename = *getString(pop());
    StringBuffer sb; 
    sb.readFromFile(filename);
    List* list = new List();
    for (string str : sb.getLines()) {
        list = appendList(list, cxt.getAlloc().makeString(str));
    }
    list->persist = true;
    push(cxt.getAlloc().makeList(list));
}

ListNode* TWVM::mergesort(ListNode* head, Function* cmp) {
    if (head == nullptr || head->next == nullptr)
        return head;
    ListNode* fast = head->next;
    ListNode* slow = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* back = slow->next;
    slow->next = nullptr;
    return merge(mergesort(head, cmp), mergesort(back, cmp), cmp);
}

void TWVM::doCompare(ListNode* front, ListNode* back, Function* cmp) {
    if (cmp != nullptr) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(front->info), toString(front->info)));
        t->next = makeExprNode(CONST_EXPR, Token(getSymbol(back->info), toString(back->info)));
        funcExpression(cmp, t);
    } else {
        push(gt(back->info, front->info));
    }
}

ListNode* TWVM::merge(ListNode* front, ListNode* back, Function* cmp) {  
    ListNode d; ListNode* c = &d;
    while (front != nullptr && back != nullptr) {
        doCompare(front, back, cmp);
        Object result = pop();
        if (result.data.boolval) {
            c->next = front; front = front->next; c = c->next;
        } else {
            c->next = back; back = back->next; c = c->next;
        }
    }
    c->next = (front == nullptr) ? back:front;
    return d.next;
}

void TWVM::doSort(astnode* node) {
    evalExpr(node->child[0]);
    Object listObj = peek(0);
    if (listObj.type != AS_LIST) {
        cout<<"Error: sort expects a list"<<endl;
        return;
    }
    Function* cmp = nullptr;
    if (node->child[1] != nullptr) {
        evalExpr(node->child[1]);
        cmp = getFunction(pop());
    }
    List* list = listObj.data.gcobj->listval;
    if (!listEmpty(list)) {
        list->head = mergesort(list->head, cmp);
        ListNode* x = list->head;
        while (x->next != nullptr) x = x->next;
        list->tail = x;
    }
}

void TWVM::doReverse(astnode* node) {
    evalExpr(node->child[0]);
    Object listObj = peek(0);
    List* list = listObj.data.gcobj->listval;
    ListNode* x = list->head;
    ListNode* prev = nullptr;
    while (x != nullptr) {
        ListNode* t = x;
        x = x->next;
        t->next = prev;
        prev = t;
    }
    list->head = prev;
    x = list->head;
    while (x->next != nullptr) x = x->next;
    list->tail = x;
}

void TWVM::makeAnonymousList(astnode* node) { 
    List* list = new List();
    for (astnode* it = node->child[0]; it != nullptr; it = it->next) {
        evalExpr(it);
        list = appendList(list, pop());
    }
    push(cxt.getAlloc().makeList(list));
}

void TWVM::listComprehension(astnode* node) {
    evalExpr(node->child[0]);
    Object listobj = peek(0);
    if (listobj.type != AS_LIST) {
        cout<<"Error: list comprehensions only work on lists."<<endl;
        return;
    }
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    Function* pred = nullptr;
    if (node->child[2] != nullptr) {
        evalExpr(node->child[2]);
        pred = getFunction(pop());
    }
    List* list = getList(listobj);
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        if (pred != nullptr) {
            funcExpression(pred, t);
            if (pop().data.boolval) {
                funcExpression(func, t);
                result = appendList(result, pop());
            }
        } else {
            funcExpression(func, t);
            result = appendList(result, pop());
        }
    }
    pop();
    push(cxt.getAlloc().makeList(result));
}

void TWVM::doAppendList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    evalExpr(node->child[1]);
    list = appendList(list, pop());
    pop();
}
void TWVM::doPushList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    evalExpr(node->child[1]);
    list = pushList(list, pop());
    pop();
}
void TWVM::getListSize(astnode* node) {
    evalExpr(node->child[0]);
    int size = 0;
    Object m = peek(0);
    if (m.type != AS_LIST && m.type != AS_STRING) {
        cout<<"Error: incorrect type supplied to size()."<<endl;
        pop();
        push(makeNil());
        return;
    }
    switch (m.type) {
        case AS_LIST: size = m.data.gcobj->listval->count; break;
        case AS_STRING: size = m.data.gcobj->strval->size(); break;
        default: break;
    }
    pop();
    push(makeInt(size));
}
void TWVM::getListEmpty(astnode* node) {
    evalExpr(node->child[0]);
    push(makeBool(getList(pop())->head == nullptr));
}
void TWVM::getFirstListElement(astnode* node) {
    evalExpr(node->child[0]);
    push(getList(pop())->head->info);
}
void TWVM::getRestOfList(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    List* nl = new List();
    for (ListNode* it = list->head->next; it != nullptr; it = it->next)
        nl = appendList(nl, it->info);
    pop();
    push(cxt.getAlloc().makeList(nl));
}
Symbol TWVM::getSymbol(Object m) {
    switch (m.type) {
        case AS_INT: return TK_NUM;
        case AS_REAL: return TK_NUM;
        case AS_BOOL: return m.data.boolval ? TK_TRUE:TK_FALSE;
        case AS_STRING: return TK_ID;
        case AS_LIST: return TK_LB;
        case AS_FUNC: return TK_LAMBDA;
        default:
            break;
    }
    return TK_NIL;
}
void TWVM::doMap(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        result = appendList(result, pop());
    }
    pop();
    push(cxt.getAlloc().makeList(result));
}
void TWVM::doFilter(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    List* result = new List();
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        if (pop().data.boolval)
            result = appendList(result, it->info);
    }
    pop();
    push(cxt.getAlloc().makeList(result));
}
void TWVM::doReduce(astnode* node) {
    evalExpr(node->child[0]);
    List* list = getList(peek(0));
    evalExpr(node->child[1]);
    Function* func = getFunction(pop());
    ListNode* it = list->head; 
    Object result = it->info;
    it = it->next;
    while (it != nullptr) {
        astnode* t = makeExprNode(CONST_EXPR, Token(getSymbol(result), toString(result)));
        t->next = makeExprNode(CONST_EXPR, Token(getSymbol(it->info), toString(it->info)));
        funcExpression(func, t);
        result = pop();
        it = it->next;
    }
    pop();
    push(result);
}

void TWVM::getType(astnode* node) {
    evalExpr(node);
    string typeName = "nil";
    switch (pop().type) {
        case AS_BOOL: typeName = "boolean"; break;
        case AS_INT: typeName = "integer"; break;
        case AS_REAL: typeName = "real"; break;
        case AS_LIST: typeName = "list"; break;
        case AS_STRING: typeName = "string"; break;
        case AS_STRUCT: typeName = "struct"; break;
        case AS_FUNC:   typeName = "function"; break;
        case AS_NULL:
        default:
            break;
    }
    push(cxt.getAlloc().makeString(typeName));
}

void TWVM::constExpr(astnode* node) {
    switch (node->token.symbol) {
        case TK_TRUE: push(makeBool(true)); break;
        case TK_FALSE: push(makeBool(false)); break;
        case TK_NUM: push(makeNumber(stoi(node->token.strval))); break;
        case TK_STR: push(expandInterpolation(node->token.strval, node->token.depth)); break;
        case TK_NIL: push(cxt.nil()); break;
        case TK_TYPEOF: getType(node->child[0]); break;
        default: 
            break;
    }
}

bool checkInterpolation(Object m) {
    if (typeOf(m) != AS_STRING)
        return false;
    string raw = *getString(m);
    for (int i = 0; i < raw.length(); i++) {
        if (raw[i] == '$' && raw[i+1] == '{') {
            int j = i+2;
            while (j < raw.length()) {
                if (raw[j] == '}')
                    return true;
                j++;
            }
        }
    }
    return false;
}

Object TWVM::expandInterpolation(string raw, int scope) {
    string outstring;
    for (int i = 0; i < raw.length(); i++) {
        if (raw[i] == '$' && raw[i+1] == '{') {
            int j = i+2;
            string varname;
            while (j < raw.length()) {
                if (raw[j] != '}') {
                    varname.push_back(raw[j]);
                    j++;
                } else {
                    break;
                }
            }
            Object t = cxt.get(varname, scope);
            outstring += toString(t);
            i = j;
        } else {
            outstring.push_back(raw[i]);
        }
    }
    return cxt.getAlloc().makeString(outstring);
}

void TWVM::rangeExpression(astnode* node) {
    evalExpr(node->child[0]);
    Object lhs = pop();
    int l = lhs.data.intval;
    evalExpr(node->child[1]);
    Object rhs = pop();
    int r = rhs.data.intval;
    List* nl = new List();
    if (l < r) {
        for (int i = l; i <= r; i++)
            nl = appendList(nl, makeInt(i));
    } else {
        for (int i = l; i >= r; i--)
            nl = appendList(nl, makeInt(i));
    }
    push(cxt.getAlloc().makeList(nl));
}