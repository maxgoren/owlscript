#include "lexer.hpp"

Lexer::Lexer(bool dbg, bool compd) { noisey = dbg; compressed = compd; }

Token Lexer::makeLexToken(TKSymbol symbol, char* text, int length) {
    return Token(symbol, string(text, length));
}

int Lexer::find(int curr, char p) {
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
    string actual;
    string match;
     while (!buffer->done()) {
        if (buffer->get() != '"')
            actual.push_back(buffer->get());
        state = get_next(state, buffer->get());
        if (state > 0 && mgc_lex_accept[state] > -1) {
            last_match = state;
            match_len = len;
            match = actual;
        }
        if (buffer->get() == '"') {
            if (!in_quote) {
                in_quote = true;
            } else {
                in_quote = false;
                buffer->advance();
                break;
            }
        }
        if (state < 1) {
            break;
        }
        buffer->advance();
        len++;
    }
    if (last_match == 0) {
        return {TK_EOI, "error"};
    }
    return Token((TKSymbol)mgc_lex_accept[last_match], match, buffer->lineNo());
}

bool Lexer::shouldSkip(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); 
}

vector<Token> Lexer::lex(CharBuffer* buff) {
    buffer = buff;
    in_comment = false;
    vector<Token> tokens;
    while (!buffer->done()) { 
        while (!buffer->done()) {
            if (shouldSkip(buffer->get())) {
                buffer->advance();
            } else {
                break;
            }
        }
        Token next;
        next = nextToken();
        if (next.getSymbol() != TK_EOI) {
            tokens.push_back(next);
            //cout<<"Recognized: {'"<<tokenStr[next.getSymbol()]<<","<<tokens.back().getString()<<"'}"<<endl;
        } else {
            if (!in_comment)
                cout<<buffer->get()<<"?"<<endl;
            buffer->advance();
            if (!in_comment)
                cout<<buffer->get()<<"?"<<endl;
        }
    }
    tokens.push_back(Token(TK_EOI, "<fin>"));
    return tokens;
}