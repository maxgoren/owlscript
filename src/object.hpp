#ifndef object_hpp
#define object_hpp
#include <iostream>
#include <fstream>
#include "ast.hpp"
using namespace std;

enum ObjectType {
    AS_INT, AS_REAL, 
    AS_BOOL, AS_STRING, 
    AS_LIST, AS_LAMBDA, 
    AS_STRUCT, AS_KVPAIR,
    AS_FILE, AS_THUNK, AS_NIL
};

enum ObjType {
    OT_STR, OT_LIST, OT_LAMBDA, OT_STRUCT, OT_KVPAIR, OT_FILE, OT_THUNK
};

struct StringObj {
    char* str;
    int length;
};

struct BidingList;

struct LambdaObj {
    BidingList* freeVars;
    astnode* params;
    astnode* body;
};

struct ListNode;

struct ListObj {
    ListNode* head;
    ListNode* tail;
    int length;
};


struct FileObj {
    StringObj* fname;
    ListObj* lines;
};

struct KVPair;
struct StructObj;
struct Thunk;

struct ObjBase {
    ObjType type;
    union {
        ListObj* listObj;
        StringObj* stringObj;
        LambdaObj* lambdaObj;
        StructObj* structObj;
        FileObj* fileObj;
        KVPair* kvpairObj;
        Thunk*  thunkObj;
    };
    bool mark;
    int refCount;
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
            case AS_STRING: { objval = ob.objval; } break;
            case AS_LIST: { objval = ob.objval; } break;
            case AS_LAMBDA: { objval = ob.objval; } break;
            case AS_STRUCT: { objval = ob.objval; } break;
            case AS_KVPAIR: { objval = ob.objval; } break;
            case AS_THUNK: { objval = ob.objval; } break;
            case AS_FILE: { 
                objval = ob.objval; 
                objval->fileObj = ob.objval->fileObj; 
                objval->fileObj->lines = ob.objval->fileObj->lines; 
            } break;
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
            case AS_STRING: {  objval = ob.objval; } break;
            case AS_LIST: { objval = ob.objval; } break;
            case AS_LAMBDA: { objval = ob.objval; } break;
            case AS_STRUCT: { objval = ob.objval; } break;
            case AS_KVPAIR: { objval = ob.objval; } break;
            case AS_THUNK: { objval = ob.objval; } break;
            case AS_FILE: { objval = ob.objval; objval->fileObj = ob.objval->fileObj; objval->fileObj->lines = ob.objval->fileObj->lines; } break;
            case AS_NIL: intval = 0; break;
            default: 
                break;
        }
        return *this;
    }
};

struct BidingList {
    string key;
    Object value;
    BidingList* next;
};

struct ListNode {
    Object info;
    ListNode* next;
    ListNode(Object m) : info(m), next(nullptr) { }
    ListNode() { next = nullptr; }
};

struct StructObj {
    string name;
    unordered_map<string, Object> bindings;
    bool blessed;
};

struct KVPair {
    Object key;
    Object value;
};

struct Thunk {
    Object memoResult;
    bool beenEvald;
    astnode* code;
};


StructObj* makeStructObj();
LambdaObj* makeLambdaObj(astnode* body, astnode* params);
StringObj* makeStringObj(string str);
ListObj*   makeListObj();
FileObj*   makeFileObj(StringObj* filename);
KVPair*    makeKVPairObj(Object key, Object value);
ObjBase*   makeObjBase(ObjType ot);
ListNode*  makeListNode(Object& m);
BidingList*   makeVarList(string key, Object val, BidingList* list);
Thunk*     makeThunkObj(astnode* code);

bool isRealAnInteger(double);
Object makeIntObject(int intVal);
Object makeRealObject(double val);
Object makeBoolObject(bool val);
Object makeStringObject(string val);
Object makeLambdaObject(LambdaObj* lambda);
Object makeListObject(ListObj* list);
Object makeStructObject(StructObj* structobj);
Object makeFileObject(FileObj* fileObj);
Object makeKVPairObject(KVPair* kvp);
Object makeNilObject();
Object makeThunk(Thunk* thunk);
Object makeReferenceObject(Object* obj);
ListObj* getList(Object m);
LambdaObj* getLambda(Object m);
StructObj* getStruct(Object m);
StringObj* getString(Object m);
FileObj* getFile(Object m);
double getAsReal(Object m);
KVPair* getKVPair(Object m);

bool comparesAsOrdinal(Object m);
string     toString(Object obj);
ObjectType typeOf(Object obj);
string      getTypeName(Object obj);
bool   listEmpty(ListObj* list);
int    listLength(ListObj* list);
ListObj*   appendToList(ListObj* list, Object m);
ListObj*   updateListItem(ListObj* list, int index, Object m);
Object getListItem(ListObj* list, int index);
void   pushToList(ListObj* list, Object m);
Object popList(ListObj* list);
Object popBackList(ListObj* list);
bool   compareUnknownTypes(Object a, Object b);
ListObj* reverseList(ListObj* list);

void destroyObject(ObjBase*);
void destroyList(ListObj*);
void destroyStruct(StructObj*);
void destroyString(StringObj*);
void destroyLambda(LambdaObj*);

std::ostream& operator<<(std::ostream& out, const StringObj& str);
std::ostream& operator<<(std::ostream& out, const Object& obj);
bool operator==(const Object& a, const Object b);
bool operator!=(const Object& a, const Object b);

#endif