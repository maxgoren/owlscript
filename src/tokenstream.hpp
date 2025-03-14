#ifndef tokenstream_hpp
#define tokenstream_hpp
#include "stream_iface.hpp"
#include "token.hpp"
#include <vector>
using namespace std;

class TokenStream : public Stream<Token> {
    private:
        vector<Token> tokens;
        int tpos;
    public:
        TokenStream(vector<Token> tkns) {
            init(tkns);
        }
        TokenStream() {

        }
        void init(vector<Token> tkns) {
            tokens = tkns;
            tpos = 0;
        }
        void start() {
            tpos = 0;
        }
        bool done() {
            return tpos == tokens.size();
        }
        Token& get() {
            return tokens[tpos];
        }
        Token& advance() {
            tpos++;
            return get();
        }
        void rewind() {
            if (tpos-1 >= 0)
                tpos--;
        }
};

#endif