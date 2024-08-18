#include "lex.hpp"

Lexer::Lexer(bool debug) {
    loud = debug;
    reserved["println"] = TK_PRINT;
    reserved["print"] = TK_PRINT;
    reserved["while"] = TK_WHILE;
    reserved["for"] = TK_FOR;
    reserved["if"] = TK_IF;
    reserved["else"] = TK_ELSE;
    reserved["let"] = TK_LET;
    reserved["def"] = TK_DEF;
    reserved["return"] = TK_RETURN;
    reserved["true"] = TK_TRUE;
    reserved["false"] = TK_FALSE;
    reserved["lambda"] = TK_LAMBDA;
    reserved["append"] = TK_APPEND;
    reserved["push"] = TK_PUSH;
    reserved["pop"] = TK_POP;
    reserved["length"] = TK_LENGTH;
    reserved["sort"] = TK_SORT;
    reserved["first"] = TK_FIRST;
    reserved["rest"] = TK_REST;
    reserved["map"] = TK_MAP;
    reserved["empty"] = TK_EMPTY;
    state = DONE;
}

void Lexer::init(string str) {
    sb.init(str);
}

void Lexer::init(vector<string> lines) {
    sb.init(lines);
}

vector<Token> Lexer::lex() {
    vector<Token> result;
    state = START;
    Token next;
    while (state != DONE && state != ERROR) {
        if (loud)
            cout<<"State: "<<dfastate[state]<<endl;
        if (shouldSkip(sb.get()))
            skipWhiteSpace();

        if (state == START || state == SCANNING) {
            if (loud)
                cout<<"Scanning from "<<sb.get()<<endl;

            if (isdigit(sb.get())) {
                state = IN_NUM;
                next = extractNumber();
            } else if (sb.get() == '"') {
                state = IN_STRING;
                sb.advance();
                next = extractString();
            } else if (isalpha(sb.get())) {
                state = IN_ID;
                next = extractIdentifier();
            } else {
                state = IN_SPECIAL;
                next = checkSpecials();
                sb.advance(); 
            }
            if (loud)
                cout<<"State: "<<dfastate[state]<<endl;
            if (state != ERROR) { 
                if (state != IN_COMMENT) {
                    if (loud)
                        cout<<"Extracted: "<<next.strval<<endl;
                    result.push_back(next);
                    state = SCANNING;
                }
                if (next.symbol == TK_CLOSE_COMMENT) {
                    state = SCANNING;
                }
            } else {
                cout<<"An Error Occured during lexing."<<endl;
                result.clear();
                state = DONE;
            }
        } else {
            if (loud)
                cout<<"State: "<<dfastate[state]<<endl;
        }
        if (sb.done()) {
            if (loud) 
                cout<<"Input Stream Exhausted."<<endl;
            state = DONE;
        }
    }
    result.push_back(Token(TK_EOF, "<eof>"));
    return result;
}

Token Lexer::extractNumber() {
    string num;
    bool is_real = false;
    while (!sb.done() && (isdigit(sb.get()) || sb.get() == '.')) {
        num.push_back(sb.get());
        if (sb.get() == '.')
            is_real = true;
        sb.advance();
    }
    return Token(is_real ? TK_REALNUM:TK_NUM, num);
}

Token Lexer::extractIdentifier() {
    string id;
    while (!sb.done() && (isalpha(sb.get()) || isdigit(sb.get()))) {
        id.push_back(sb.get());
        sb.advance();
    }
    if (reserved.find(id) != reserved.end()) {
        return Token(reserved[id], id);
    }
    return Token(TK_ID, id);
}

Token Lexer::extractString() {
    string str;
    while (!sb.done()) {
        if (sb.get() == '"')
            break; 
        str.push_back(sb.get());
        sb.advance();
    }
    if (!sb.done() && sb.get() != '"') {
        state = ERROR;
        cout<<"Error: unterminated string"<<endl;
        return Token(TK_ERROR, "unterminated string");
    } else sb.advance();
    return Token(TK_STRING, str);
}

Token Lexer::checkSpecials() {
    if (sb.get() == ':') {
        if (sb.advance() == '=')
            return Token(TK_ASSIGN, ":=");
        else sb.reverse();
        return Token(TK_COLON, ":");
    }
    if (sb.get() == '<') {
        if (sb.advance() == '=')
            return Token(TK_LTE, "<=");
        else sb.reverse();
        return Token(TK_LT, "<");
    }
    if (sb.get() == '>') {
        if (sb.advance() == '=')
            return Token(TK_GTE, ">=");
        else sb.reverse();
        return Token(TK_GT, ">");
    }
    if (sb.get() == '=') {
        if (sb.advance() == '=')
            return Token(TK_EQU, "==");
        else sb.reverse();
    }
    if (sb.get() == '!') {
        if (sb.advance() == '=') 
            return Token(TK_NOTEQU, "!=");
        else sb.reverse();
        return Token(TK_BANG, "!");
    }
    if (sb.get() == '/') {
        if (sb.advance() == '*') {
            state = IN_COMMENT;
            return Token(TK_OPEN_COMMENT, "/*");
        }
        sb.reverse();
        return Token(TK_DIV, "/");
    }
    if (sb.get() == '*') {
        if (sb.advance() == '/') {
            return Token(TK_CLOSE_COMMENT);
        }
        sb.reverse();
        return Token(TK_MUL, "*");
    }
    if (sb.get() == '-') {
        if (sb.advance() == '>') {
            return TK_PRODUCE;
        }
        sb.reverse();
        return Token(TK_SUB, "-");
    }
    if (sb.get() == '+') return Token(TK_ADD, "+");
    if (sb.get() == '(') return Token(TK_LPAREN, "(");
    if (sb.get() == ')') return Token(TK_RPAREN, ")");
    if (sb.get() == '{') return Token(TK_LCURLY, "{");
    if (sb.get() == '}') return Token(TK_RCURLY, "}");
    if (sb.get() == '[') return Token(TK_LSQUARE, "[");
    if (sb.get() == ']') return Token(TK_RSQUARE, "]");
    if (sb.get() == '.') return Token(TK_PERIOD, ".");
    if (sb.get() == ',') return Token(TK_COMMA, ",");
    if (sb.get() == ';') return Token(TK_SEMI, ";");
    if (sb.get() == '&') return Token(TK_AMPER, "&");
    state = ERROR;
    return Token(TK_ERROR, "<error>");
}

bool Lexer::shouldSkip(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

void Lexer::skipWhiteSpace() {
    while (shouldSkip(sb.get())) 
        sb.advance();
    if (sb.done())
         state = DONE;
    else state = SCANNING;
}