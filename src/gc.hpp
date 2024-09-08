#ifndef gc_hpp
#define gc_hpp
#include "object.hpp"
#include "context.hpp"
#include <set>
using namespace std;

class GC {
    private:
        bool loud;
        set<ObjBase*> allocated_objects;
        bool is_gc_object(const Object& m) {
            switch (m.type) {
                case AS_STRING:
                case AS_STRUCT:
                case AS_LAMBDA:
                case AS_LIST:
                        return true;
                default:
                    break;
            }
            return false;
        }
        void markList(Object& m) {
            for (auto it = getList(m)->head; it != nullptr; it = it->next) {
                if (is_gc_object(it->info)) {
                    it->info.objval->mark = true;
                }
            }
        }
        void checkObject(Object& m) {
            if (is_gc_object(m)) {
                m.objval->mark = true;
                switch (m.objval->type) {
                    case OT_LIST:
                        markList(m);
                    default:
                        break;
                }
            }
        }
        void mark(Context& cxt) {
            for (auto & m : allocated_objects) {
                m->mark = false;
            }
            for (auto & m : cxt.globals) {
                checkObject(m.second);
            }
            for (int i = 0; i < cxt.scoped.size(); i++) {
                for (auto& m : cxt.scoped.get(i)) {
                    checkObject(m.second);
                }
            }
        }
        void sweep(Context& cxt) {
            vector<ObjBase*> unreachable;
            for (auto & m : allocated_objects) {
                if (m->mark == false) {
                    if (loud) {
                        cout<<m<<" has become unreachable."<<endl;
                    }
                    unreachable.push_back(m);  
                } else {
                    m->mark = false;
                }
            }
            for (auto & m : unreachable) {
                if (loud) {
                    cout<<"Reclaiming: "<<m<<endl;
                }
                allocated_objects.erase(m);
                destroyObject(m);
            }
        }
    public:
        GC(bool debug = true) {
            loud = debug;
        }
        void add(ObjBase* object) {
            allocated_objects.insert(object);
        }
        void run(Context& cxt) {
            if (loud) {
                cout<<"[GC Running.]"<<endl;
            }
            mark(cxt);
            sweep(cxt);
            if (loud) {
                cout<<"[GC Complete.]"<<endl;
            }
        }
};

#endif