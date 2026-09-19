#ifndef gcobject_hpp
#define gcobject_hpp


enum CollectType {
    AR, FUNC, ITEM
};

struct GCObject {
    bool marked;
    CollectType kind;
    GCObject(CollectType t = ITEM) {
        kind = t;
        marked = false;
    }
};


#endif