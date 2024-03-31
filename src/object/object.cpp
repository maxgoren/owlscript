#include "object.hpp"

Object::Object() {
    type = AS_REAL;
    realVal = 0.0f;
}

Object::Object(const Object& obj) {
    switch (obj.type) {
        case AS_BOOL:   boolVal = obj.boolVal; break;
        case AS_INT:    intVal = obj.intVal; break;
        case AS_REAL:   realVal = obj.realVal; break;
        case AS_CHAR:   charVal = obj.charVal; break;
        case AS_STRING: stringVal = obj.stringVal; break;
        case AS_LIST:   list = obj.list; break;
        case AS_CLOSURE: closure = obj.closure; break;
        case AS_NIL:
        default:
            break;
    }
    type = obj.type;
}

bool Object::operator==(const Object& obj) const noexcept {
    if (type == obj.type) {
        switch (obj.type) {
            case AS_BOOL: return boolVal == obj.boolVal;
            case AS_INT:  return intVal == obj.intVal;
            case AS_REAL: return realVal == obj.realVal;
            case AS_CHAR: return charVal == obj.charVal;
            case AS_STRING: return stringVal == obj.stringVal;
            case AS_LIST: return  list == obj.list;
            case AS_CLOSURE: return closure == obj.closure;
            case AS_NIL:
        default:
            break;
        }
    }
    return false;
}

bool Object::operator!=(const Object& obj) const noexcept {
    return !(*this == obj);
}

Object& Object::operator=(const Object& obj) {
    if (obj != *this) {
        switch (obj.type) {
            case AS_BOOL:   boolVal = obj.boolVal;
            case AS_INT:    intVal = obj.intVal;
            case AS_REAL:   realVal = obj.realVal;
            case AS_CHAR:   charVal = obj.charVal; break;
            case AS_STRING: stringVal = obj.stringVal;
            case AS_LIST:   list = obj.list;
            case AS_CLOSURE: closure = obj.closure;
            case AS_NIL:
            default:
                break;
        }
        type = obj.type;
    }
    return *this;
}

Object* makeObject(StoreAs type) {
    Object* obj = GarbageCollector.allocNode();
    obj->type = type;
    obj->isnull = false;
    return obj;
}

Object* makeIntObject(int value) {
    Object* obj = makeObject(AS_INT);
    obj->intVal = value;
    return obj;
}

Object* makeRealObject(float value) {
    Object* obj = makeObject(AS_REAL);
    obj->realVal = value;
    return obj;
}

Object* makeCharObject(char value) {
    Object* obj = makeObject(AS_CHAR);
    obj->charVal = value;
    return obj;
}

Object* makeBoolObject(bool value) {
    Object* obj = makeObject(AS_BOOL);
    obj->boolVal = value;
    return obj;
}

Object* makeStringObject(string* value) {
    Object* obj = makeObject(AS_STRING);
    obj->stringVal = value;
    return obj;
}

Object* makeListObject(ListHeader* list) {
    Object* obj = makeObject(AS_LIST);
    obj->list = list;
    return obj;
}

Object* makeClosureObject(Lambda* closure) {
    Object* obj = makeObject(AS_CLOSURE);
    obj->closure = closure;
    return obj;
}

Object* makeNilObject() {
    Object* obj = makeObject(AS_NIL);
    return obj;
}

string toString(Object* obj) {
    if (obj == nullptr)
        return "[err]";
    string str;
    Object* data;
    switch (obj->type) {
        case AS_NIL:    str = "0"; break;
        case AS_CHAR:   str.push_back(obj->charVal); break;
        case AS_REAL:   str = to_string(obj->realVal); break;
        case AS_INT:    str = to_string(obj->intVal);  break;
        case AS_BOOL:   str = obj->boolVal ? "1":"0"; break;
        case AS_STRING: str = *obj->stringVal; break;
        case AS_CLOSURE: str = "(LAMBDA)"; break;
        case AS_LIST: 
            str = "[ ";
            if (obj->list != nullptr) {
                for (ListNode* m = obj->list->head; m != nullptr; m = m->next) {
                    data = m->data;
                    if (data == nullptr) {
                        str += "[err] ";
                    } else {
                        if (m->data->type == AS_STRING)
                            str += "\"" + toString(m->data) + "\"";
                        else {
                            str += toString(m->data);
                        }
                        str += (m->next == nullptr) ? " ":", ";
                    }
                }
            } else {
                str += "0";
            }
            str.push_back(']');
            break;
        default: 
            str = "0"; break;
    }
    return str;
}