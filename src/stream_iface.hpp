#ifndef stream_iface_hpp
#define stream_iface_hpp

template <typename T>
class Stream {
    public:
        virtual bool done() = 0;
        virtual T& advance() = 0;
        virtual T& get() = 0;
};

#endif