#include "object.hpp"

bool operator==(const Object& a, const Object b) {
    return toString(a) == toString(b);
}
bool operator!=(const Object& a, const Object b) {
    return !(a == b);
}

std::ostream& operator<<(std::ostream& out, const StringObj& str) {
    out<<str.str;
    return out;
}

StringObj* makeStringObj(string str) {
    StringObj* s = new StringObj;
    s->length = str.size();
    s->str = new char[s->length+1];
    int i = 0;
    for (char c : str) {
        s->str[i++] = c;
    }
    s->str[i] = '\0';
    return s;
}

StringObj* getString(Object m) {
    return m.objval->stringObj;
}
 
LambdaObj* getLambda(Object m) {
    return m.type == AS_LAMBDA ? m.objval->lambdaObj:nullptr;
}

ListObj* getList(Object m) {
    if (typeOf(m) == AS_FILE) {
        return m.objval->fileObj->lines;
    }
    return m.objval->listObj;
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

FileObj* makeFileObj(StringObj* fname) {
    FileObj* fo = new FileObj;
    fo->fname = fname;
    return fo;
}

ObjBase* makeObjBase(ObjType ot) {
    ObjBase* m = new ObjBase;
    m->type = ot;
    m->mark = false;
    m->refCount = 0;
    return m;
}

Object makeIntObject(int intVal) {
    Object o(AS_INT);
    o.intval = intVal;
    return o;
}

Object makeRealObject(double val) {
    Object o(AS_REAL);
    if (isRealAnInteger(val)) {
        o.type = AS_INT;
        o.intval = (int)val;
        return o;
    }
    o.realval = val;
    return o;
}

Object makeBoolObject(bool val) {
    Object o(AS_BOOL);
    o.boolval = val;
    return o;
}

Object makeStringObject(string val) {
    Object o(AS_STRING);
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

Object makeFileObject(FileObj* fobj) {
    Object o(AS_FILE);
    o.objval = makeObjBase(OT_FILE);
    o.objval->fileObj = fobj;
    return o;
}

Object makeReferenceObject(Object* objPtr) {
    Object o(AS_REF);
    o.refVal = objPtr;
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

bool isRealAnInteger(double val) {
    string num = to_string(val);
    int i = 0;
    while (i < num.size() && num[i++] != '.');
    while (i < num.size() && num[i++] == '0');
    return i == num.size();
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

VarList* makeVarList(string key, Object val, VarList* list) {
    VarList* nn = new VarList;
    nn->key = key;
    nn->value = val;
    nn->next = list;
    return nn;
}

ListNode* makeListNode(Object& m) {
    return new ListNode(m);
}

bool listEmpty(ListObj* list) {
    return list->head == nullptr;
}

int listLength(ListObj* list) {
    return list->length;
}

ListObj*   updateListItem(ListObj* list, int index, Object m) {
    if (index > list->length || index < 0) {
        cout<<"Error: subscript out of range"<<endl;
        return list;
    }
    ListNode* it = list->head;
    for (int i = 0; i < index; i++) {
        it = it->next;
    }
    it->info = m;
    return list;
}

Object getListItem(ListObj* list, int subscript) {
    if (subscript > list->length || list->length == 0) {
        cout<<"Error: Subscript out of range."<<endl;
        return makeNilObject();
    }
    ListNode* it = list->head;
    for (int i = 0; i < subscript; i++) {
        it = it->next;
    }
    return it->info;
}


ListObj* appendToList(ListObj* list, Object m) {
    ListNode* ln = makeListNode(m);
    if (listEmpty(list)) {
        list->head = ln;
    } else {
        list->tail->next = ln;
    }
    list->tail = ln;
    list->length += 1;
    return list;
}

void pushToList(ListObj* list, Object m) {
    ListNode* ln = makeListNode(m);
    if (listEmpty(list)) {
        list->head = ln;
        list->tail = ln;
    } else {
        ln->next = list->head;
        list->head = ln;
    }
    list->length += 1;
}

ListObj* reverseList(ListObj* list) {
    ListNode* tmp = nullptr;
    ListNode* x = list->head;
    while (x != nullptr) {
        ListNode* t = x;
        x = x->next;
        t->next = tmp;
        tmp = t;
    }
    list->head = tmp;
    x = list->head;
    while (x != nullptr && x->next != nullptr) x = x->next;
    list->tail = x;
    return list;
}

Object popList(ListObj* list) {
    Object m = list->head->info;
    ListNode* t = list->head;
    list->head = list->head->next;
    delete t;
    list->length -= 1;
    return m;
}

Object popBackList(ListObj* list) {
    Object m  = list->tail->info;
    if (list->length == 1) {
        ListNode* t = list->head;
        list->head = nullptr;
        list->length = 0;
        delete t;
        return m;
    }
    ListNode* x = list->head;
    while (x->next != nullptr && x->next != list->tail) {
        x = x->next;
    }
    ListNode* t = x->next;
    x->next = NULL;
    list->tail = x;
    list->length -= 1;
    return m;
}

string listToString(Object obj) {
    string liststr = "[ ";
    for (auto it = getList(obj)->head; it != nullptr; it = it->next) {
        liststr += toString(it->info);
        if (it->next != nullptr)
            liststr += ", ";
    }
    liststr += "]";
    return liststr;
}

string structToString(Object obj) {
    StructObj* sobj = getStruct(obj);
    string ret = "\n{\n";
    if (sobj != nullptr) {
        int i = 0;
        for (auto np : sobj->bindings) {
            ret += "\t" + np.first + ": " + toString(np.second);
            if (i+1 < sobj->bindings.size()) {
                ret += ",\n";
                i++;
            }
        }
    } else {
        ret += "(nil)";
    }
    ret += "\n}";
    return ret;
}

string toString(Object obj) {
    switch (obj.type) {
        case AS_INT:    return to_string(obj.intval); 
        case AS_REAL:   return to_string(obj.realval);
        case AS_BOOL:   return obj.boolval ? "true":"false";
        case AS_STRING:    return getString(obj)->str;
        case AS_LIST:   return listToString(obj);
        case AS_LAMBDA: return "(lambda)";
        case AS_STRUCT: return structToString(obj);
        case AS_FILE: {
            string tmp;
            for (ListNode* it = obj.objval->fileObj->lines->head; it != nullptr; it = it->next) {
                tmp.append(toString(it->info));
                tmp.append("\n");
            }
            return tmp;
        } break;
        case AS_NIL:  
        default: 
            break;
    }
    return "(nil)";
}

string getTypeName(Object obj) {
    switch (obj.type) {
        case AS_INT:    return "integer"; 
        case AS_REAL:   return "real";
        case AS_BOOL:   return "boolean";
        case AS_STRING: return "string";
        case AS_LIST:   return "list";
        case AS_LAMBDA: return "lambda";
        case AS_STRUCT: return "struct";
        case AS_FILE:   return "file";
        case AS_NIL:  
        default: 
            break;
    }
    return "nil";
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

StructObj* getStruct(Object m) {
    return m.objval->structObj;
}

void destroyList(ListObj* list) {
    if (list == nullptr)
        return;
    while (list->head != nullptr) {
        ListNode* t = list->head;
        list->head = list->head->next;
        delete t;
    }
    delete list;
}

void destroyString(StringObj* str) {
    if (str == nullptr)
        return;
    if (str->str != nullptr)
        delete str->str;
    delete str;
}

void destroyLambda(LambdaObj* lambda) {
    if (lambda == nullptr)
        return;
    while (lambda->freeVars != nullptr) {
        VarList* x = lambda->freeVars;
        lambda->freeVars = lambda->freeVars->next;
        delete x;
    }
    delete lambda;
}

void destroyStruct(StructObj* sobj) {
    if (sobj == nullptr)
        return;
    delete sobj;
}

void destroyObject(ObjBase* object) {
    if (object == nullptr)
        return;
    switch (object->type) {
        case OT_LAMBDA: 
                //cout<<"[lambda]"<<endl;
                destroyLambda(object->lambdaObj);
                break;
        case OT_STRUCT:
                //cout<<"[struct]"<<endl;
                destroyStruct(object->structObj);
                break;
        case OT_LIST:
                //cout<<"[list]"<<endl;
                destroyList(object->listObj);
                break;
        case OT_STR:
                //cout<<"[string]"<<endl;
                destroyString(object->stringObj);
                break;
        default:
                //cout<<"uhmmm... what?"<<endl;
            break;
    }
    delete object;
}