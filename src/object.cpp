#include "object.hpp"

std::ostream& operator<<(std::ostream& out, const StringObj& str) {
    out<<str.str;
    return out;
}

StringObj* makeStringObj(string str) {
    StringObj* s = new StringObj;
    s->length = str.size();
    s->str = new char[s->length];
    int i = 0;
    for (char c : str) {
        s->str[i++] = c;
    }
    s->str[i] = '\0';
    return s;
}

StructObj* makeStructObj() {
    StructObj* so = new StructObj;
    return so;
}

LambdaObj* makeLambdaObj(astnode* body, astnode* params) {
    LambdaObj* lam = new LambdaObj;
    lam->body = body;
    lam->params = params;
    lam->freeVars = nullptr;
    return lam;
}

ListObj* makeListObj() {
    ListObj* list = new ListObj;
    list->length = 0;
    list->head = nullptr;
    list->tail = nullptr;
    return list;
}

ObjBase* makeObjBase(ObjType ot) {
    ObjBase* m = new ObjBase;
    m->type = ot;
    return m;
}

Object makeIntObject(int intVal) {
    Object o(AS_INT);
    o.intval = intVal;
    return o;
}

Object makeRealObject(double val) {
    Object o(AS_REAL);
    o.realval = val;
    return o;
}

Object makeBoolObject(bool val) {
    Object o(AS_BOOL);
    o.boolval = val;
    return o;
}

Object makeStringObject(string val) {
    Object o(AS_STR);
    o.objval = makeObjBase(OT_STR);
    o.objval->stringObj = makeStringObj(val);
    return o;
}

Object makeLambdaObject(LambdaObj* lambda) {
    Object o(AS_LAMBDA);
    o.objval = makeObjBase(OT_LAMBDA);
    o.objval->lambdaObj = lambda;
    return o;
}

Object makeListObject(ListObj* list) {
    Object o(AS_LIST);
    o.objval = makeObjBase(OT_LIST);
    o.objval->listObj = list;
    return o;
}

Object makeStructObject(StructObj* sobj) {
    Object o(AS_STRUCT);
    o.objval = makeObjBase(OT_STRUCT);
    o.objval->structObj = sobj;
    return o;
}

Object makeNilObject() {
    Object o(AS_NIL);
    o.intval = 0;
    return o;
}

ObjectType typeOf(Object obj) {
    return obj.type;
}

bool comparesAsOrdinal(Object m) {
    switch (m.type) {
        case AS_INT:
        case AS_REAL:
        case AS_BOOL:
            return true;
        default:
            break;
    }
    return false;
}

double getAsReal(Object m) {
    switch (m.type) {
        case AS_INT: return (double)m.intval;
        case AS_BOOL: return (double)m.boolval;
        case AS_REAL: return (double)m.realval;
        default:
            break;
    }
    return 0.0;
}

LambdaObj* getLambda(Object m) {
    return m.objval->lambdaObj;
}

VarList* makeVarList(string key, Object val, VarList* list) {
    VarList* nn = new VarList;
    nn->key = key;
    nn->value = val;
    nn->next = list;
    return nn;
}

ListNode* makeListNode(Object& m) {
    ListNode* ln = new ListNode;
    ln->info = m;
    ln->next = nullptr;
    return ln;
}

ListObj* getList(Object m) {
    return m.objval->listObj;
}

bool listEmpty(ListObj* list) {
    return list->head == nullptr;
}

int listLength(ListObj* list) {
    return list->length;
}

void appendToList(ListObj* list, Object m) {
    ListNode* ln = makeListNode(m);
    if (listEmpty(list)) {
        list->head = ln;
    } else {
        list->tail->next = ln;
    }
    list->tail = ln;
    list->length += 1;
}

void pushToList(ListObj* list, Object m) {
    ListNode* ln = makeListNode(m);
    ln->next = list->head;
    list->head = ln;
    if (list->tail == nullptr)
        list->tail = ln;
    list->length += 1;
}

Object popList(ListObj* list) {
    Object m = list->head->info;
    ListNode* t = list->head;
    list->head = list->head->next;
    delete t;
    list->length -= 1;
    return m;
}

string toString(Object obj) {
    switch (obj.type) {
        case AS_INT:    return to_string(obj.intval); 
        case AS_REAL:   return to_string(obj.realval);
        case AS_BOOL:   return obj.boolval ? "true":"false";
        case AS_STR:    return string(obj.objval->stringObj->str);
        case AS_LIST: {
                string liststr = "[ ";
                for (auto it = getList(obj)->head; it != nullptr; it = it->next) {
                    liststr += toString(it->info);
                    if (it->next != nullptr)
                        liststr += ", ";
                }
                liststr += "]";
                return liststr;
            }
        case AS_LAMBDA: return "(lambda)";
        case AS_STRUCT: {
                StructObj* sobj = getStruct(obj);
            return "(struct)";
            }
        case AS_NIL:  
        default: 
            break;
    }
    return "(nil)";
}

std::ostream& operator<<(std::ostream& out, const Object& obj) {
    out<<toString(obj);
    return out;
}

bool compareUnknownTypes(Object a, Object b) {
    bool a_is_winner = false;
    if (comparesAsOrdinal(a) && comparesAsOrdinal(b)) {
        a_is_winner = (getAsReal(b) > getAsReal(a));
    } else {
        a_is_winner = (toString(b) > toString(a));
    }
    return a_is_winner;
}

ListNode* merge(ListNode* a, ListNode* b) {
    ListNode d; ListNode* c = &d;
    while (a && b) {
        if (compareUnknownTypes(a->info, b->info)) {
            c->next = a; a = a->next; c = c->next;
        } else {
            c->next = b; b = b->next; c = c->next;
        }
    }
    c->next = (a == nullptr) ? b:a;
    return d.next;
}

ListNode* mergesort(ListNode* list) {
    if (list == nullptr || list->next == nullptr)
        return list;
    ListNode* slow = list, *fast = list->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* front = list, *back = slow->next;
    slow->next = nullptr;
    return merge(mergesort(front), mergesort(back));
}

StructObj* getStruct(Object m) {
    return m.objval->structObj;
}