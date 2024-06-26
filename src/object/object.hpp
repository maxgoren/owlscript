#ifndef object_hpp
#define object_hpp
#include <iostream>
#include <vector>
#include <cmath>
#include "../closure/closure.hpp"
using namespace std;
enum StoreAs {
    AS_INT, AS_REAL, AS_BOOL, AS_CHAR, AS_STRING, AS_LIST, AS_CLOSURE, AS_NIL
};

struct ListHeader;

struct Object {
    bool isnull;
    bool gcLive;
    StoreAs type;
    union {
        int intVal;
        double realVal;
        bool boolVal;
        char charVal;
        string* stringVal;
        ListHeader* list;
        Lambda* closure;
    };
    Object();
    Object(const Object& obj);
    Object& operator=(const Object& obj);
    bool operator==(const Object& obj) const noexcept;
    bool operator!=(const Object& obj) const noexcept;
};

class GC_Allocator {
    private:
        vector<Object*> created;
        vector<Object*> freed;
        void mark_node(Object* obj);
    public:
        GC_Allocator();
        Object* allocNode();
        void freeNode(Object* obj);
        void mark();
        void sweep();
};
inline GC_Allocator GarbageCollector;


struct ListNode {
    Object* data;
    ListNode* next;
};

struct ListHeader {
    int size;
    ListNode* head;
    ListNode* tail;
};

ListHeader* makeListHeader();
ListNode* makeListNode(Object* obj);
void push_front_list(ListHeader* lh, Object* obj);
void push_back_list(ListHeader* lh, Object* obj);
Object* pop_front_list(ListHeader* lh);
ListNode* copyList(ListNode* a);
ListNode* mergeList(ListNode* a, ListNode* b);
ListNode* mergeSortList(ListNode* h);
Object* makeObject(StoreAs type);
Object* makeIntObject(int value);
Object* makeRealObject(double value);
Object* makeCharObject(char object);
Object* makeBoolObject(bool value);
Object* makeStringObject(string* object);
Object* makeListObject(ListHeader* listObj);
Object* makeClosureObject(Lambda* closure);
Object* makeNilObject();
string toString(Object* object);
string getTypeOf(Object* object);

#endif