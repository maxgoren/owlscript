#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include "token.hpp"
#include "lexer_matrix.h"
#include "../util/buffer.hpp"
using namespace std;

class Lexer {
    private:
        bool compressed;
        CharBuffer* buffer;
        bool noisey;
        bool in_comment;
        bool shouldSkip(char ch);
        Token makeLexToken(TKSymbol symbol, char* text, int length);
        Token nextToken();
        int get_next(int state, char p);
    public:
        Lexer(bool debug, bool comp);
        vector<Token> lex(CharBuffer* buffer);
};

Lexer::Lexer(bool dbg = false, bool compd = true) { noisey = dbg; compressed = compd; }

Token Lexer::makeLexToken(TKSymbol symbol, char* text, int length) {
    return Token(symbol, string(text, length));
}

int find(int curr, char p) {
    int num_entries = 2*mgc_lexer_matrix[curr][0];
    int l = 1, r = 1 + num_entries;
    if (r%2 == 0) r--;
    while (l <= r) {
        int m = (l+r)/2;
        if (m % 2 == 0) m--;
        if (p < mgc_lexer_matrix[curr][m]) {
            r = m - 2;
        } else if (p > mgc_lexer_matrix[curr][m]) {
            l = m + 2;
        } else {
            return mgc_lexer_matrix[curr][m+1];
        }
    }
    return 0;
}


int Lexer::get_next(int state, char p) {
    if (compressed) {
        if (mgc_lexer_matrix[state] != NULL) {
            return find(state, p);   
        }
        return 0;
    }
    return mgc_lexer_matrix[state][p];
}

Token Lexer::nextToken() {
    int state = 1;
    int last_match = 0;
    int match_len = 0;
    int len = 0;
    bool in_quote = false;
    int start = buffer->markStart();
    for (char p = buffer->get(); !buffer->done(); buffer->advance(), len++) {
        state = get_next(state, buffer->get());
        if (state > 0 && mgc_lex_accept[state] > -1) {
            last_match = state;
            match_len = len;
        }

        if (buffer->get() == '"') {
            if (!in_quote) in_quote = true;
            else {
                in_quote = false;
                buffer->advance();
                break;
            }
        }
        if (state < 1) {
            break;
        }
    }
    if (last_match == 0) {
        return {TK_EOI, "error"};
    }
    return Token((TKSymbol)mgc_lex_accept[last_match], buffer->sliceFromStart(match_len), buffer->lineNo());
}

bool Lexer::shouldSkip(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); 
}

vector<Token> Lexer::lex(CharBuffer* buff) {
    buffer = buff;
    in_comment = false;
    vector<Token> tokens;
    for (; !buffer->done();) { 
        while (shouldSkip(buffer->get())) buffer->advance();
        Token next;
        next = nextToken();
        if (next.getSymbol() != TK_EOI) {
            tokens.push_back(next);
            //cout<<"Recognized: {'"<<tokens.back().getString()<<"'}"<<endl;
        } else {
            if (!in_comment)
                cout<<buffer->get()<<"?"<<endl;
            buffer->advance();
            if (!in_comment)
                cout<<buffer->get()<<"?"<<endl;
        }
    }
    tokens.push_back(Token(TK_EOI, "<fin>"));
    //for (auto m : tokens) {
    //    cout<<"["<<tokenStr[m.getSymbol()]<<"] ["<<m.getString()<<"]"<<endl;
    //}
    return tokens;
}

#endif