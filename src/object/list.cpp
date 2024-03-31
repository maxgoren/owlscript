#include "object.hpp"

ListHeader* makeListHeader() {
    ListHeader*  lh = new ListHeader;
    lh->head = nullptr;
    lh->tail = nullptr;
    lh->size = 0;
    return lh;
}

ListNode* makeListNode(Object* obj) {
    ListNode* ln = new ListNode;
    ln->data = obj;
    ln->next = nullptr;
    return ln;
}

void push_front_list(ListHeader* lh, Object* obj) {
    if (lh == nullptr) return;
    ListNode* ln = makeListNode(obj);
    ln->next = lh->head;
    lh->head = ln;
    if (lh->tail == nullptr) lh->tail = ln;
    lh->size++;
}

void push_back_list(ListHeader* lh, Object* obj) {
    if (lh == nullptr) return;
    ListNode* ln = makeListNode(obj);
    if (lh->size == 0) {
        lh->head = ln;
        lh->tail = ln;
    } else {
        lh->tail->next = ln;
        lh->tail = ln;
    }
    lh->size++;
}

Object* pop_front_list(ListHeader* lh) {
    if (lh == nullptr || lh->head == nullptr) return nullptr;
    ListNode* t = lh->head;
    lh->head = lh->head->next;
    lh->size--;
    Object* ret = t->data;
    delete t;
    return ret;
}


ListNode* copyList(ListNode* a) {
    ListNode dummy; ListNode *t = &dummy;
    ListNode* it = a;
    while (it != nullptr) {
        ListNode* nn = new ListNode;
        nn->data = it->data;
        nn->next = nullptr;
        t->next = nn;
        t = t->next;
        it = it->next;
    }    
    return dummy.next;
}

ListNode* mergeList(ListNode* a, ListNode* b) {
    ListNode dummy; ListNode *c = &dummy;
    while (a != nullptr && b != nullptr) {
        //thats just ugly.
        if (stof(toString(b->data)) > stof(toString(a->data))) {
            c->next = a; a = a->next; c = c->next;
        } else {
            c->next = b; b = b->next; c = c->next;
        }
    }
    c->next = (a == nullptr) ? b:a;
    return dummy.next;
}

ListNode* mergeSortList(ListNode* h) {
    if (h == nullptr || h->next == nullptr)
        return h;
    ListNode* fast = h->next, *slow = h;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* front = h;
    ListNode* back = slow->next;
    slow->next = nullptr;
    return mergeList(mergeSortList(front), mergeSortList(back));
}