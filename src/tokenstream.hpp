#ifndef tokenstream_hpp
#define tokenstream_hpp
#include <iostream>
#include <vector>
#include "token.hpp"
using namespace std;

class TokenStream {
    private:
        TKTokenListNode* head;
        TKTokenListNode* tail;
        TKTokenListNode* it;    
    public:
        TokenStream(TKTokenListNode* tkns) {
            init(tkns);
        }
        TokenStream() {
            head = tail = it = NULL;
        }
        void init(TKTokenListNode* tkns) {
            head = tkns;
            it = head;
        }
        void append(Token token) {
            
        }
        void start() {
            it = head;
        }
        bool done() {
            return it == NULL;
        }
        Token get() {
            return it == NULL ? Token(TK_EOI, "<fin.>"):Token(rules[it->token->rule_id].token, it->token->text);
        }
        void advance() {
            if (it != NULL)
                it = it->next;
        }
        void clear() {
            head = tail = it = NULL;
        }
};

#endif