#ifndef object_hpp
#define object_hpp
#include <iostream>
#include "ast.hpp"
using namespace std;

enum ObjectType {
    AS_INT, AS_REAL, AS_BOOL, AS_STR, AS_LIST, AS_LAMBDA, AS_STRUCT, AS_NIL
};

enum ObjType {
    OT_STR, OT_LIST, OT_LAMBDA, OT_STRUCT
};

struct StringObj {
    char* str;
    int length;
};

struct VarList;
struct LambdaObj {
    VarList* freeVars;
    astnode* params;
    astnode* body;
};

struct ListNode;
struct ListObj {
    ListNode* head;
    ListNode* tail;
    int length;
};


struct StructObj;

struct ObjBase {
    ObjType type;
    union {
        ListObj* listObj;
        StringObj* stringObj;
        LambdaObj* lambdaObj;
        StructObj* structObj;
    };
};

struct Object {
    ObjectType type;
    union {
        int intval;
        double realval;
        bool boolval;
        ObjBase* objval;
    };
    Object(ObjectType t = AS_INT) {
        type = t;
    }
    Object(const Object& ob) {
        type = ob.type;
        switch (type) {
            case AS_INT: {  intval = ob.intval; } break;
            case AS_REAL: { realval = ob.realval; } break;
            case AS_BOOL: {  boolval = ob.boolval; } break;
            case AS_STR: { objval = ob.objval; } break;
            case AS_LIST: { objval = ob.objval; } break;
            case AS_LAMBDA: { objval = ob.objval; } break;
            case AS_STRUCT: { objval = ob.objval; } break;
            case AS_NIL: intval = 0; break;
            default: 
                break;
        }
    }
    ~Object() { }
    Object& operator=(const Object& ob) {
        type = ob.type;
        switch (type) {
            case AS_INT: {  intval = ob.intval; } break;
            case AS_REAL: {  realval = ob.realval; } break;
            case AS_BOOL: { boolval = ob.boolval; } break;
            case AS_STR: {  objval = ob.objval; } break;
            case AS_LIST: { objval = ob.objval; } break;
            case AS_LAMBDA: { objval = ob.objval; } break;
            case AS_STRUCT: { objval = ob.objval; } break;
            case AS_NIL: intval = 0; break;
            default: 
                break;
        }
        return *this;
    }
};

struct VarList {
    string key;
    Object value;
    VarList* next;
};

struct ListNode {
    Object info;
    ListNode* next;
};

struct StructObj {
    string name;
    unordered_map<string, Object> bindings;
    bool blessed;
};

StructObj* makeStructObj();
LambdaObj* makeLambdaObj(astnode* body, astnode* params);
StringObj* makeStringObj(string str);
ListObj*   makeListObj();
ObjBase*   makeObjBase(ObjType ot);
ListNode*  makeListNode(Object& m);
VarList*   makeVarList(string key, Object val, VarList* list);

Object makeIntObject(int intVal);
Object makeRealObject(double val);
Object makeBoolObject(bool val);
Object makeStringObject(string val);
Object makeLambdaObject(LambdaObj* lambda);
Object makeListObject(ListObj* list);
Object makeStructObject(StructObj* structobj);
Object makeNilObject();

ListObj* getList(Object m);
LambdaObj* getLambda(Object m);
StructObj* getStruct(Object m);
double getAsReal(Object m);

bool comparesAsOrdinal(Object m);
string     toString(Object obj);
ObjectType typeOf(Object obj);

bool   listEmpty(ListObj* list);
int    listLength(ListObj* list);
void   appendToList(ListObj* list, Object m);
void   pushToList(ListObj* list, Object m);
Object popList(ListObj* list);
bool   compareUnknownTypes(Object a, Object b);
ListNode* merge(ListNode* a, ListNode* b);
ListNode* mergesort(ListNode* list);

std::ostream& operator<<(std::ostream& out, const StringObj& str);
std::ostream& operator<<(std::ostream& out, const Object& obj);

#endif