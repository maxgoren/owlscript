#ifndef bag_hpp
#define bag_hpp
#include <iostream>
using namespace std;

template <class T>
struct bagnode {
    T info;
    bagnode* next;
    bagnode(T i = 0, bagnode* n = nullptr) : info(i), next(n) {

    }
};

template <class T>
class BagIterator {
    private:
        using link = bagnode<T>*;
        link curr;
    public:
        BagIterator(link it) : curr(it) { }
        T& operator*() {
            return curr->info;
        }
        BagIterator operator++() {
            curr = curr->next;
            return *this;
        }
        BagIterator operator++(int) {
            BagIterator tmp = *this;
            ++tmp;
            return *this;
        }
        bool operator==(const BagIterator& b) const {
            return curr == b.curr;
        }
        bool operator!=(const BagIterator& b) const {
            return !(*this == b);
        }
};


template <class T>
class Bag {
    private:
        using link = bagnode<T>*;
        using iterator = BagIterator<T>;
        link head;
        link tail;
        int count;
        void init() {
            head = nullptr;
            tail = nullptr;
            count = 0;
        }
    public:
        Bag() {
            init();
        }
        Bag(const Bag& ob) {
            init();
            for (link it = ob.head; it != nullptr; it = it->next)
                add(it->info);
        }
        ~Bag() {
            while (!empty()) {
                link x = head;
                head = head->next;
                x->next = nullptr;
                delete x;
                count--;
            }
        }
        int size() const {
            return count;
        }
        bool empty() const {
            return head == nullptr;
        }
        void add(T info) {
            link x = new bagnode<T>(info, nullptr);
            if (empty()) {
                head = x;
            } else {
                tail->next = x;
            }
            tail = x;
            count++;
        }
        void clear() {
            while (head != nullptr) {
                link x = head;
                head = head->next;
                delete x;
            }
            init();
        }
        iterator begin() {
            return iterator(head);
        }
        iterator end() {
            return iterator(nullptr);
        }
        Bag& operator=(const Bag& ob) {
            if (this != &ob) {
                init();
                for (link it = ob.head; it != nullptr; it = it->next)
                    add(it->info);
            }
            return *this;
        }
};

 #endif