#ifndef object_hpp
#define object_hpp
#include <iostream>
#include <cmath>
#include <unordered_map>
#include "ast.hpp"
using namespace std;


enum StoreAs {
    AS_INT, AS_REAL, AS_BOOL, AS_CHAR, AS_STRING, AS_STRUCT, AS_FUNC, AS_LIST, AS_MAP, AS_REF, AS_NULL
};

struct List;
struct Function;
struct Struct;
struct WeakRef;
struct ActivationRecord;
struct GCObject;

struct Object {
    StoreAs type;
    union {
        long intval;
        double realval;
        bool boolval;
        char charval;
        GCObject* gcobj;
        WeakRef* reference;
    } data;
    Object(char val) { type = AS_CHAR; data.charval = val; }
    Object(double val) { type = AS_REAL; data.realval = val; }
    Object(bool val) { type = AS_BOOL; data.boolval = val; }
    Object(int val) { type = AS_INT; data.intval = val; }
    Object(long val) { type = AS_INT; data.intval = val; }
    Object(WeakRef* obj) { type = AS_REF; data.reference = obj; }
    Object() { type = AS_NULL; data.intval = 0; }
    Object(const Object& obj) {
        type = obj.type;
        switch (type) {
            case AS_LIST: data.gcobj = obj.data.gcobj; break;
            case AS_STRING: data.gcobj = obj.data.gcobj; break;
            case AS_FUNC: data.gcobj = obj.data.gcobj; break;
            case AS_STRUCT: data.gcobj = obj.data.gcobj; break;
            case AS_CHAR: data.charval = obj.data.charval; break;
            case AS_BOOL: data.boolval = obj.data.boolval; break;
            case AS_INT: data.intval = obj.data.intval; break;
            case AS_REAL: data.realval = obj.data.realval; break;
            case AS_REF: data.reference = obj.data.reference; break;
            default:
                break;
        }
    }
};

struct Function {
    string name;
    astnode* body;
    astnode* params;
    ActivationRecord* closure;
    Function(astnode* par, astnode* code) : params(par), body(code), closure(nullptr) { }
    Function() {
        name = "nil";
        closure = nullptr;
    }
};

struct ListNode {
    Object info;
    ListNode* next;
    ListNode(Object obj = Object(), ListNode* n = nullptr) : info(obj), next(n) { }
};


struct List {
    ListNode* head;
    ListNode* tail;
    int count;
    bool persist;
    List() : head(nullptr), tail(nullptr), persist(false) {
        count = 0;
    }
};

struct COWList {
    List* file;
    string filepath;
    COWList(string name) : filepath(name) {

    }
};

struct Struct {
    string typeName;
    bool blessed;
    unordered_map<int,string> constructorOrder;
    unordered_map<string, Object> fields;
    Struct(string tn) : typeName(tn), blessed(false) { }
    Struct() : typeName("nil"), blessed(false) { }
};

struct WeakRef {
    string identifier;
    int scopelevel;
    WeakRef(string id, int sl) : identifier(id), scopelevel(sl) { }
};


enum GC_TYPE {
    GC_LIST, GC_FILE, GC_STRING, GC_FUNC, GC_STRUCT, GC_EMPTY
};

struct GCObject {
    GC_TYPE type;
    bool marked;
    union {
        string* strval;
        Function* funcval;
        List* listval;
        COWList* file;
        Struct* structval;
    };
    GCObject(string* s) : strval(s), marked(false), type(GC_STRING) { }
    GCObject(string s) : strval(new string(s)), marked(false), type(GC_STRING) { }
    GCObject(List* l) : listval(l), marked(false), type(GC_LIST) { }
    GCObject(COWList* cl) : file(cl), marked(false), type(GC_FILE) { }
    GCObject(Function* f) : funcval(f), marked(false), type(GC_FUNC) { }
    GCObject(Struct* s) : structval(s), marked(false), type(GC_STRUCT) { }
    GCObject(const GCObject& ob) {
        switch (ob.type) {
            case GC_STRING: strval = ob.strval; break;
            case GC_LIST: listval = ob.listval; break;
            case GC_FILE: file = ob.file; break;
            case GC_FUNC: funcval = ob.funcval; break;
            case GC_STRUCT: structval = ob.structval; break;
            default: break;
        }
    }
    GCObject() : marked(false), type(GC_EMPTY) { }
};


//Accessors
bool getBoolean(Object m);
long getInteger(Object m);
int getReal(Object m);
char getChar(Object m);

extern List dummylist;
extern string dummystring;
extern Struct dummystruct;

List* getList(const Object& m);
Function* getFunction(const Object& m);
string* getString(const Object& m);
Struct* getStruct(const Object& m);
WeakRef* getReference(const Object& m);
double getPrimitive(Object obj);


//List operations
List* appendList(List* list, Object obj);
bool listEmpty(List* list);
int listSize(List* list);
List* pushList(List* list, Object obj);
List* updateListAt(List* list, int index, Object obj);
ListNode* getListItemAt(List* list, int index) ;

//stringify
string listToString(List* list);
string toString(GCObject* obj);
string toString(Object obj);

void printGCObject(GCObject* x);
ostream& operator<<(ostream& os, const Object& obj);

StoreAs typeOf(Object obj);
bool compareOrdinal(Object obj) ;


//non-GC type Constructos
Object makeInt(int val) ;
Object makeInt(long val) ;
Object makeReal(double val);
Object makeNumber(double val) ;
Object makeBool(bool val) ;
Object makeReference(string id, int scope) ;
Object makeNil();

Object neg(Object lhs);
//binary operators
Object bwAnd(Object lhs, Object rhs);
Object bwOr(Object lhs, Object rhs);
Object bwXor(Object lhs, Object rhs) ;
Object add(Object lhs, Object rhs) ;
Object sub(Object lhs, Object rhs) ;
Object div(Object lhs, Object rhs) ;
Object mod(Object lhs, Object rhs) ;
Object mul(Object lhs, Object rhs) ;
Object pow(Object lhs, Object rhs);
Object lt(Object lhs, Object rhs) ;
Object lte(Object lhs, Object rhs);
Object gt(Object lhs, Object rhs);
Object gte(Object lhs, Object rhs);
Object equ(Object lhs, Object rhs);
Object neq(Object lhs, Object rhs);
Object logicAnd(Object lhs, Object rhs) ;
Object logicOr(Object lhs, Object rhs);
#endif