#include "lexer.hpp"

Lexeme Lexer::extractStringLiteral() {
    string strLit;
    while (sb.getChar() != sb.EOFMark()) {
        char rc = sb.getChar();
        if (rc == '\\') {
            if (sb.nextChar() == 'n')
                strLit.push_back('\n');
            else sb.unGet();
        } else {
            strLit.push_back(rc);
        }
        sb.nextChar();
        if (sb.getChar() == '"')
            break;
    }
    return Lexeme(STRING, strLit, sb.lineNumber());
}

Lexeme Lexer::extractWord() {
    string word;
    while (sb.getChar() != sb.EOFMark() && isalnum(sb.getChar())) {
        word.push_back(sb.getChar());
        sb.nextChar();
    }
    sb.unGet();
    if (reservedWords.find(word) != reservedWords.end()) {
        return Lexeme(reservedWords[word], word, sb.lineNumber());
    }
    return Lexeme(ID, word, sb.lineNumber());
}

Lexeme Lexer::extractNumber() {
    string word;
    while (sb.getChar() != sb.EOFMark() && (isdigit(sb.getChar()) || sb.getChar() == '.')) {
        word.push_back(sb.getChar());
        sb.nextChar();
    }
    sb.unGet();
    return Lexeme(NUMBER, word, sb.lineNumber());
}

Lexeme Lexer::checkSpecials() {
    if (sb.getChar() == ' ' || sb.getChar() == '\t' || sb.getChar() == '\r' || sb.getChar() == '\n') 
        return Lexeme(WHITESPACE, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '(') return Lexeme(LPAREN, sb.asString(), sb.lineNumber());
    if (sb.getChar() == ')') return Lexeme(RPAREN, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '{') return Lexeme(LCURLY, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '}') return Lexeme(RCURLY, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '[') return Lexeme(LSQ, sb.asString(), sb.lineNumber());
    if (sb.getChar() == ']') return Lexeme(RSQ, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '+') return Lexeme(PLUS, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '-') return Lexeme(MINUS, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '/') return Lexeme(DIVIDE, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '*') return Lexeme(MULTIPLY, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '%') return Lexeme(MOD, sb.asString(), sb.lineNumber());
    if (sb.getChar() == ';') return Lexeme(SEMI, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '.') return Lexeme(PERIOD, sb.asString(), sb.lineNumber());
    if (sb.getChar() == ',') return Lexeme(COMA, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '"') return Lexeme(QUOTE, sb.asString(), sb.lineNumber());
    if (sb.getChar() == '<') {
        sb.nextChar();
        if (sb.getChar() == '=') 
            return Lexeme(LTE, "<=", sb.lineNumber());
        sb.unGet();
        return Lexeme(LESS, sb.asString(), sb.lineNumber()); }
    if (sb.getChar() == '>') {
        sb.nextChar();
        if (sb.getChar() == '=') 
            return Lexeme(GTE, "==", sb.lineNumber());
        sb.unGet();
        return Lexeme(GREATER, sb.asString(), sb.lineNumber());
    }
    if (sb.getChar() == '!') {
        sb.nextChar();
        if (sb.getChar() == '=') 
            return Lexeme(NOTEQUAL, "==", sb.lineNumber());
        sb.unGet();
         return Lexeme(NOT, "!", sb.lineNumber());
    }
    if (sb.getChar() == '=') {
        sb.nextChar();
        if (sb.getChar() == '=') 
            return Lexeme(EQUAL, "==", sb.lineNumber());
        sb.unGet();
         return Lexeme(ERROR, "<error>", sb.lineNumber());
    }
    if (sb.getChar() == ':') {
        sb.nextChar();
        if (sb.getChar() == '=') {
            return Lexeme(ASSIGN, ":=", sb.lineNumber());
        }
        sb.unGet();
        return Lexeme(COLON, sb.asString(), sb.lineNumber());
    }
    cout<<"No idea what a "<<sb.getChar()<<" is"<<endl;
    return Lexeme(ERROR, "<error>", sb.lineNumber());
}

Lexer::Lexer() {
    initReserved();
}

void Lexer::initReserved() {
    reservedWords["print"]  = PRINT;
    reservedWords["println"]= PRINT;
    reservedWords["read"] = READ;
    reservedWords["if"] = IF;
    reservedWords["else"] = ELSE;
    reservedWords["loop"] = LOOP;
    reservedWords["def"] = DEF;
    reservedWords["return"] = RETURN;
    reservedWords["push"] = PUSH;
    reservedWords["pop"] = POP;
    reservedWords["length"] = LENGTH;
    reservedWords["append"] = APPEND;
    reservedWords["sort"] = SORT;
    reservedWords["map"] = MAP;
    reservedWords["first"] = FIRST;
    reservedWords["rest"] = REST;
    reservedWords["lambda"] = LAMBDA;
    reservedWords["nil"] = NIL;
    reservedWords["true"] = TRUE;
    reservedWords["false"] = FALSE;
}

vector<Lexeme>& Lexer::lexString(string str) {
    sb.load(str);
    return start();
}

vector<Lexeme>& Lexer::lex(string filename) {
    sb.loadFile(filename);
    return start();
}

vector<Lexeme>& Lexer::start() {
    Lexeme next;
    lexemes.clear();
    while (sb.getChar() != sb.EOFMark()) {
        if (isalpha(sb.getChar())) {
            next = extractWord();
        } else if (isdigit(sb.getChar())) {
            next = extractNumber();
        } else if (sb.getChar() == '#') {
            int ln = sb.lineNumber();
            while (sb.lineNumber() == ln) sb.nextChar();
            continue;
        } else {
            next = checkSpecials();
        }
        if (next.tokenVal != WHITESPACE && next.tokenVal != ERROR)
            lexemes.push_back(next);
        if (sb.getChar() == '"') {
            sb.nextChar();
            next = extractStringLiteral();
            lexemes.push_back(next);
            if (sb.getChar() == '"') {
                next = checkSpecials();
                lexemes.push_back(next);
            }
        }
        if (sb.getChar() == sb.EOFMark())
            break;
        sb.nextChar();
    }
    lexemes.push_back(Lexeme(EOFTOKEN, "<EOF>", sb.lineNumber()));
    return lexemes;
}