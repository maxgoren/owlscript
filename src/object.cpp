#include "object.hpp"

bool getBoolean(Object m) {
    return m.data.boolval;
}

long getInteger(Object m) {
    return m.data.intval;
}

int getReal(Object m) {
    return m.data.realval;
}

char getChar(Object m) {
    return m.data.charval;
}

List dummylist;
List* getList(const Object& m) {
    return m.data.gcobj->listval == nullptr ? &dummylist:m.data.gcobj->listval;
}

Function* getFunction(const Object& m) {
    return m.data.gcobj ? m.data.gcobj->funcval:nullptr;
}

string dummystring;
string* getString(const Object& m) {
    return m.data.gcobj->strval == nullptr ? &dummystring:m.data.gcobj->strval;
}

Struct dummystruct;
Struct* getStruct(const Object& m) {
    return m.data.gcobj->structval == nullptr ? &dummystruct:m.data.gcobj->structval;
}

WeakRef* getReference(const Object& m) {
    return m.data.reference;
}

void printGCObject(GCObject* x) {
    switch (x->type) {
        case GC_FUNC:   cout<<x->funcval->name<<endl; break;
        case GC_LIST:   cout<<"(list)"<<endl; break;
        case GC_STRING: cout<<*x->strval<<endl; break;
    }
}

List* appendList(List* list, Object obj) {
    ListNode* t = new ListNode(obj, nullptr);
    if (list->head == nullptr) {
        list->head = t;
    } else {
        list->tail->next = t;
    }
    list->tail = t;
    list->count += 1;
    return list;
}

bool listEmpty(List* list) {
    return list->count == 0;
}

int listSize(List* list) {
    return list == nullptr ? -1:list->count;
}

List* pushList(List* list, Object obj) {
    ListNode* t = new ListNode(obj, list->head);
    if (listEmpty(list)) {
        list->tail = t;
    }
    list->head = t;
    list->count += 1;
    return list;
}

List* updateListAt(List* list, int index, Object obj) {
    int i = 0;
    ListNode* it = list->head; 
    while (it != nullptr && i < index) {
        it = it->next;
        i++;
    }
    it->info = obj;
    return list;
}

ListNode* getListItemAt(List* list, int index) {
    int i = 0;
    ListNode* it = list->head; 
    while (it != nullptr && i < index) {
        it = it->next;
        i++;
    }
    return it;
}

string toString(Object m);

string listToString(List* list) {
    string str;
    str = "[ ";
    if (list != nullptr) {
    for (ListNode* it = list->head; it != nullptr; it = it->next) {
        str += toString(it->info);
        if (it->next != nullptr) 
            str += ", ";
    }
    }
    str += " ]";
    return str;
}

string toString(GCObject* obj) {
    string str;
    switch (obj->type) {
        case GC_STRING: str = *obj->strval; break;
        case GC_LIST:   str = listToString(obj->listval); break;
        case GC_STRUCT: str = obj->structval->typeName; break;
        case GC_FUNC: str = obj->funcval->name; break;
        default:
            str = "(empty)";
    }
    return str;
}

string toString(Object obj) {
    string str;
    switch (obj.type) {
        case AS_INT:    str = to_string(obj.data.intval); break;
        case AS_REAL:   str = to_string(obj.data.realval); break;
        case AS_BOOL:   str = obj.data.boolval ? "true":"false"; break;
        case AS_STRING: str = *(obj.data.gcobj->strval); break;
        case AS_FUNC:   str = obj.data.gcobj->funcval->name; break;
        case AS_REF:    str = "<" + obj.data.reference->identifier + ">"; break;
        case AS_NULL:   str = "(null)"; break;
        case AS_LIST: {
            str = listToString(obj.data.gcobj->listval);
        } break;
        case AS_STRUCT: {
            str = obj.data.gcobj->structval->typeName + " {";
            for (auto m : obj.data.gcobj->structval->fields) {
                str += m.first +": " + toString(m.second) + ", ";
            }
            str += "}";
        } break;
        default: break;
    }
    return str;
}

ostream& operator<<(ostream& os, const Object& obj) {
    os<<toString(obj);
    return os;
}


StoreAs typeOf(Object obj) {
    return obj.type;
}

bool compareOrdinal(Object obj) {
    switch (obj.type) {
        case AS_REAL: 
        case AS_BOOL: 
        case AS_INT:  return true;
        default:
            break;
    }
    return false;
}

double getPrimitive(Object obj) {
    double a = 0;
    switch (obj.type) {
        case AS_REAL: { a = obj.data.realval;  }break;
        case AS_BOOL: { a = obj.data.boolval; } break;
        case AS_INT:  { a = obj.data.intval; } break;
        default:
            break;
    }
    return a;
}

Object makeInt(int val) {
    Object m;
    m.type = AS_INT;
    m.data.intval = val;
    return m;
}

Object makeInt(long val) {
    Object m;
    m.type = AS_INT;
    m.data.intval = val;
    return m;
}

Object makeReal(double val) {
    Object m;
    if (std::floor(val) == val) {
        m.type = AS_INT; 
        m.data.intval = (int)val;
        return m;
    }
    m.type = AS_REAL;
    m.data.realval = val;
    return m;
}

Object makeNumber(double val) {
    return makeReal(val);
}

Object makeBool(bool val) {
    return Object(val);
}

Object makeReference(string id, int scope) {
    return Object(new WeakRef(id, scope));
}

Object makeNil() {
    return Object();
}

Object bwAnd(Object lhs, Object rhs) {
    int lhn = getPrimitive(lhs);
    int rhn = getPrimitive(rhs);
    return makeInt(lhn & rhn);
}

Object bwOr(Object lhs, Object rhs) {
    int lhn = getPrimitive(lhs);
    int rhn = getPrimitive(rhs);
    return makeInt(lhn | rhn);
}

Object bwXor(Object lhs, Object rhs) {
    int lhn = getPrimitive(lhs);
    int rhn = getPrimitive(rhs);
    return makeInt(lhn ^ rhn);
}

Object add(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    return makeReal(lhn+rhn);
}

Object sub(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    return makeReal(lhn-rhn);
}

Object div(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    if (rhn == 0.0) {
        cout<<"Error: divide by 0"<<endl;
        return makeReal(0);
    }
    return makeReal(lhn/rhn);
}

Object mod(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    return makeInt((int)lhn % (int)rhn);
}

Object mul(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    return makeNumber(lhn*rhn);
} 

Object pow(Object lhs, Object rhs) {
    double lhn = getPrimitive(lhs);
    double rhn = getPrimitive(rhs);
    return makeNumber(pow(lhn, rhn));
}

Object neg(Object lhs) {
    double val = getPrimitive(lhs);
    return makeNumber(-val);
}

Object lt(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn < rhn);
    }
    return makeBool(toString(lhs) < toString(rhs));
}

Object lte(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn <= rhn);
    }
    return makeBool(toString(lhs) <= toString(rhs));
}

Object gt(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn > rhn);
    }
    return makeBool(toString(lhs) > toString(rhs));
}

Object gte(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn >= rhn);
    }
    return makeBool(toString(lhs) >= toString(rhs));
}

Object equ(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn == rhn);
    }
    return makeBool(toString(lhs) == toString(rhs));
}

Object neq(Object lhs, Object rhs) {
    if (compareOrdinal(lhs) && compareOrdinal(rhs)) {
        double lhn = getPrimitive(lhs);
        double rhn = getPrimitive(rhs);
        return makeBool(lhn != rhn);
    }
    return makeBool(toString(lhs) != toString(rhs));
}

Object logicAnd(Object lhs, Object rhs) {
    return makeBool(getPrimitive(lhs) && getPrimitive(rhs));
}

Object logicOr(Object lhs, Object rhs) {
    return makeBool(getPrimitive(lhs) || getPrimitive(rhs));
}