#ifndef inspectablestack_hpp
#define inspectablestack_hpp

template <class Item>
class InspectableStack {
    private:
        Item* st;
        int n;
        int maxn;
        void grow() {
            Item* old = st;
            st = new Item[2*maxn];
            for (int i = 0; i < n; i++) {
                st[i] = old[i];
            }
            delete [] old;
            maxn *= 2;
        }
        void init(int max) {
            n = 0;
            maxn = max;
            st = new Item[maxn];
        }
    public:
        InspectableStack() {
            init(255);
        }
        InspectableStack(const InspectableStack& cs) {
            init(cs.maxn);
            for (int i = 0; i < cs.n; i++) {
                st[i] = cs.st[i];
            }
            n = cs.n;
        }
        ~InspectableStack() {
            delete [] st;
        }
        int size() const { return n; }
        bool empty() const { return n == 0; }
        void push(Item info) {
            if (n+1 == maxn) grow();
            st[n++] = info;
        }
        Item& top() {
            return st[n-1];
        }
        Item& get(int i) {
            return st[i];
        }
        Item pop() {
            return st[--n];
        }
        void clear() {
            n = 0;
        }
        InspectableStack& operator=(const InspectableStack& cs) {
            if (this != &cs) {
                init(cs.maxn);
                for (int i = 0; i < cs.n; i++) {
                    st[i] = cs.st[i];
                }
                n = cs.n;
            }
            return *this;
        }
 };

#endif