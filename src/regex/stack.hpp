#ifndef stack_hpp
#define stack_hpp

template <class T>
class Stack {
    private:
        struct snode {
            T info;
            snode* next;
            snode(T i, snode* n) : info(i), next(n) { }
        };
        using link = snode*;
        link head;
        int count;
        void init() {
            head = nullptr;
            count = 0;
        }
    public:
        Stack() {
            init();
        }
        Stack(const Stack& st) {
            init();
            snode d(T(), nullptr); link c = &d;
            for (link it = st.head; it != nullptr; it = it->next) {
                c->next = new snode(it->info, nullptr);
                c = c->next;
            }
            head = d.next;
            count = st.count;
        }
        ~Stack() {
            clear();
        }
        bool empty() const {
            return head == nullptr;
        }
        int size() const {
            return count;
        }
        void push(T info) {
            head = new snode(info, head);
            count++;
        }
        T& top() {
            return head->info;
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
        void clear() {
            while (!empty()) pop();
        }
        Stack& operator=(const Stack& st) {
            if (this != &st) {
                init();
                snode d(T(), nullptr); link c = &d;
                for (link it = st.head; it != nullptr; it = it->next) {
                    c->next = new snode(it->info, nullptr);
                    c = c->next;
                }
                head = d.next;
                count = st.count;
            }
            return *this;
        }
};

#endif