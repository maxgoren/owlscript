#ifndef queue_hpp
#define queue_hpp

template <class T>
class queue {
    private:
        struct qnode {
            T info;
            qnode* next;
            qnode(T i) : info(i), next(nullptr) { }
        };
        using link = qnode*;
        link head, tail;
        int count;
        void init() {
            head = nullptr;
            tail = nullptr;
            count = 0;
        }
    public:
        queue() {
            init();
        }
        queue(const queue& oq) {
            init();
            for (link it = oq.head; it != nullptr; it = it->next) {
                push(it->info);
            }
        }
        ~queue() {
            clear();
        }
        bool empty() const {
            return head == nullptr;
        }
        int size() const {
            return count;
        }
        void push(T info) {
            link t = new qnode(info);
            if (empty()) {
                head = t;
            } else {
                tail->next = t;
            }
            tail = t;
            count++;
        }
        T pop() {
            T ret = head->info;
            link x = head;
            head = head->next;
            x->next = x;
            delete x;
            count--;
            return ret;
        }
        T& front() {
            return head->info;
        }
        void clear() {
            while (!empty()) 
                pop();
        }
        queue& operator=(const queue& oq) {
            if (this != &oq) {
                init();
                for (link it = oq.head; it != nullptr; it = it->next) {
                    push(it->info);
                }
            }
            return *this;
        }
};

#endif