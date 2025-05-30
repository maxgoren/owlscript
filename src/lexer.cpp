#include "lexer.hpp"

Lexer::Lexer() {
    reserved["if"] = Token(TK_IF, "if");
    reserved["in"] = Token(TK_IN, "in");
    reserved["let"] = Token(TK_LET, "let");
    reserved["var"] = Token(TK_LET, "var");
    reserved["ref"] = Token(TK_REF, "ref");
    reserved["def"] = Token(TK_FUNC, "def");
    reserved["map"] = Token(TK_MAP, "map");
    reserved["nil"] = Token(TK_NIL, "nil");
    reserved["else"] = Token(TK_ELSE, "else");
    reserved["func"] = Token(TK_FUNC, "func");
    reserved["push"] = Token(TK_PUSH, "push");
    reserved["size"] = Token(TK_SIZE, "size");
    reserved["sort"] = Token(TK_SORT, "sort");
    reserved["rest"] = Token(TK_REST, "rest");
    reserved["true"] = Token(TK_TRUE, "true");
    reserved["while"] = Token(TK_WHILE, "while");
    reserved["empty"] = Token(TK_EMPTY, "empty");
    reserved["first"] = Token(TK_FIRST, "first");
    reserved["print"] = Token(TK_PRINT, "print");
    reserved["false"] = Token(TK_FALSE, "false");
    reserved["bless"] = Token(TK_BLESS, "bless");
    reserved["break"] = Token(TK_BREAK, "break");
    reserved["return"] = Token(TK_RETURN, "return");
    reserved["struct"] = Token(TK_STRUCT, "struct");
    reserved["append"] = Token(TK_APPEND, "append"); 
    reserved["filter"] = Token(TK_FILTER, "filter");
    reserved["reduce"] = Token(TK_REDUCE, "reduce"); 
    reserved["typeOf"] = Token(TK_TYPEOF, "typeOf");
    reserved["matchre"] = Token(TK_MATCHRE, "matchre");
    reserved["println"] = Token(TK_PRINTLN, "println");
    reserved["foreach"] = Token(TK_FOREACH, "foreach");
    reserved["continue"] = Token(TK_CONTINUE, "continue");
}

TokenStream Lexer::lex(StringBuffer sb) {
    TokenStream ts;
    while (!sb.done()) {
        skipWhiteSpace(sb);
        skipComments(sb);
        if (isdigit(sb.get())) {
            ts.append(extractNumber(sb));
        } else if (isalpha(sb.get()) || sb.get() == '_') {
            ts.append(extractId(sb));
        } else if (sb.get() == '"') {
            ts.append(extractString(sb));
        } else {
            ts.append(checkSpecials(sb));
            sb.advance();
        }
    }
    ts.append(Token(TK_EOI, "<fin.>"));
    return ts;
}

Token Lexer::checkSpecials(StringBuffer& sb) {
    switch (sb.get()) {
        case '%': return Token(TK_MOD, "%");
        case '/': return Token(TK_DIV, "/");
        case '?': return Token(TK_QM, "?");
        case '(': return Token(TK_LP, "(");
        case ')': return Token(TK_RP, ")");
        case '{': return Token(TK_LC, "{");
        case '}': return Token(TK_RC, "}");
        case '[': return Token(TK_LB, "[");
        case ']': return Token(TK_RB, "]");
        case ',': return Token(TK_COMA, ",");
        case ';': return Token(TK_SEMI, ";");
        default: break;
    }
    if (sb.get() == '+') {
        sb.advance();
        if (sb.get() == '+') {
            return Token(TK_POST_INC,"++");
        } else if (sb.get() == '=') {
            return Token(TK_ASSIGN_SUM, "+=");
        }
        sb.rewind();
        return Token(TK_ADD, "+");
    }
    if (sb.get() == '&') {
        sb.advance();
        if (sb.get() == '(') {
            return Token(TK_LAMBDA, "&(");
        } else if (sb.get() == '&') {
            return Token(TK_AND, "&&");
        }
        sb.rewind();
        return Token(TK_BIT_AND, "&");
    }
    if (sb.get() == '|') {
        sb.advance();
        if (sb.get() == '|') {
            return Token(TK_OR, "||");
        } else if (sb.get() == '&') {
            return Token(TK_PIPE, "|");
        }
        sb.rewind();
        return Token(TK_BIT_OR, "|");
    }
    if (sb.get() == '^') {
        sb.advance();
        if (sb.get() == '^') {
            return Token(TK_POW, "^^");
        }
        sb.rewind();
        return Token(TK_BIT_XOR, "^");
    }
    if (sb.get() == '*') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_ASSIGN_PROD, "*=");
        }
        sb.rewind();
        return Token(TK_MUL, "*");
    }
    if (sb.get() == '-') {
        sb.advance(); 
        if (sb.get() == '>') {
            return Token(TK_PRODUCES, "->");
        } else if (sb.get() == '-') {
            return Token(TK_POST_DEC, "--");
        } else if (sb.get() == '=') {
            return Token(TK_ASSIGN_DIFF, "-=");
        }
        sb.rewind();
        return Token(TK_SUB, "-");
    }
    if (sb.get() == '.') {
        sb.advance();
        if (sb.get() == '.') {
            return Token(TK_RANGE, "..");
        }
        sb.rewind();
        return Token(TK_PERIOD, ".");
    }
    if (sb.get() == '<') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_LTE, "<=");
        }
        sb.rewind();
        return Token(TK_LT, "<");
    }
    if (sb.get() == '>') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_GTE, ">=");
        }
        sb.rewind();
        return Token(TK_GT, ">");
    }
    if (sb.get() == '=') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_EQU, "==");
        }
        sb.rewind();
        return Token(TK_ERR, "=");
    }
    if (sb.get() == '!') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_NEQ, "!=");
        }
        sb.rewind();
        return Token(TK_NOT, "!");
    }
    if (sb.get() == ':') {
        sb.advance();
        if (sb.get() == '=') {
            return Token(TK_ASSIGN, ":=");
        }
        sb.rewind();
        return Token(TK_COLON, ":");
    }
    return Token(TK_ERR, "err");
}

Token Lexer::extractString(StringBuffer& sb) {
    string str;
    sb.advance();
    while (!sb.done()) {
        if (sb.get() == '"') 
            break;
        if (sb.get() == '\\') {
            sb.advance();
            switch (sb.get()) {
                case 'n': str.push_back('\n'); break;
                case 'r': str.push_back('\r'); break;
                case 't': str.push_back('\t'); break;
                default:
                    str.push_back(sb.get());
                    break;
            }
        } else {
            str.push_back(sb.get());
        }
        sb.advance();
    }
    if (sb.get() == '"') {
        sb.advance();
    } else {
        cout<<"Error: unterminated string."<<endl;
    }
    return Token(TK_STR, str);
}

Token Lexer::checkReserved(string id) {
    if (reserved.find(id) != reserved.end())
        return reserved.at(id);
    return Token(TK_ID, id);
}

Token Lexer::extractNumber(StringBuffer& sb) {
    string num;
    while (!sb.done()) {
        if (isdigit(sb.get()) || sb.get() == '.') {
            num.push_back(sb.get());
            sb.advance();
        } else break;
    }
    return Token(TK_NUM, num);
}

Token Lexer::extractId(StringBuffer& sb) {
    string id;
    while (!sb.done()) {
        if (isalpha(sb.get()) || isdigit(sb.get()) || sb.get() == '_') {
            id.push_back(sb.get());
            sb.advance();
        } else break;
    }
    return checkReserved(id);
}

void Lexer::skipWhiteSpace(StringBuffer& sb) {
    while (!sb.done()) {
        if (sb.get() == ' ' || sb.get() == '\t' || sb.get() == '\r') {
            sb.advance();
        } else break;
    }
}

void Lexer::skipComments(StringBuffer& sb) {
    if (sb.get() == '{') {
        sb.advance();
        if (sb.get() == '*') {
            sb.advance();
            while (!sb.done()) {
                if (sb.get() == '*') {
                    sb.advance();
                    if (sb.get() == '}') {
                        sb.advance();
                        skipWhiteSpace(sb); 
                        return;
                    }
                }
                sb.advance();
            }
        }
        sb.rewind();
    }
}