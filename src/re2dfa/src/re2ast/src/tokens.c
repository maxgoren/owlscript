#include "tokens.h"

char* RESymbolStr[] = {
    "CHAR", "PERIOD", "LPAREN", "RPAREN", "CHAR CLASS",
    "STAR", "PLUS", "QUESTION", 
    "CONCAT", "OR", "EPSILON", "NONE"
};

REToken* makeToken(enum RESymbol sym, char ch) {
    REToken* nt = malloc(sizeof(REToken));
    nt->ch = ch;
    if (sym == RE_CCL) {
        nt->ccl = malloc(sizeof(char)*128);
        nt->ccl[0] = '\0';
    } else {
        nt->ccl = NULL;
    }
    nt->symbol = sym;
    nt->next = NULL;
    return nt;
}

bool is_digit(char c) {
    return (int)c > 47 && (int)c < 58;
}

bool is_char(char c) {
    return ((int)c > 64 && (int)c < 91) || 
           ((int)c > 96 && (int)c < 123) || 
           (c == '#' || c == '.');
}

bool is_special(char c) {
    return (((int)c > 32 && c < 48) || 
            ((int)c > 57 && (int)c < 65)) || 
            (((int)c > 90 && (int)c < 97) || 
            ((int)c > 122 && (int)c < 127));
}


REToken* tokenize(char* str) {
    REToken header; REToken* t = &header;
    int sp = 0;
    for (int i = 0; str[i] != '\0'; i++) { 
        if (str[i] == '\\') {
            char c = str[++i];
            if (c == 'd') {
                t->next = makeToken(RE_CCL, '[');
                t = t->next;
                t->ccl = strdup("0123456789");
            } else  {
                t->next = makeToken(RE_CHAR, c);
                t = t->next;
            }
        } else if (str[i] == '.') {
            t->next = makeToken(RE_PERIOD, str[i]);
            t = t->next;
        } else if (is_digit(str[i]) || is_char(str[i])) {
            t->next = makeToken(RE_CHAR, str[i]);
            t = t->next;
        } else {
            switch (str[i]) {
                case '@': { 
                    t->next = makeToken(RE_CONCAT, str[i]);
                    t = t->next;  
                } break;
                case '|': { 
                    t->next = makeToken(RE_OR, str[i]);
                    t = t->next;
                 } break;
                case '+': { 
                    t->next = makeToken(RE_PLUS, str[i]);
                    t = t->next; 
                } break;
                case '*': { 
                    if (str[i-1] != '\\') {
                        t->next = makeToken(RE_STAR, str[i]);
                        t = t->next;
                    } else {
                        t->next = makeToken(RE_CHAR, '*');
                        t = t->next;
                    }
                } break;
                case '?': { 
                    t->next = makeToken(RE_QUESTION, str[i]);
                    t = t->next; 
                } break;
                case '(': { 
                    t->next = makeToken(RE_LPAREN, str[i]);
                    t = t->next; 
                } break;
                case ')': { 
                    t->next = makeToken(RE_RPAREN, str[i]);
                    t = t->next; 
                } break;
                case '[': {
                    t->next = makeToken(RE_CCL, '[');
                    t = t->next;
                    char* ccl = t->ccl;
                    i++;
                    if (str[i] == '-') {
                        *ccl++ = str[i++];
                    }
                    while (str[i] != ']') {
                        char start = str[i++];
                        if (str[i-1] != '[' && str[i] == '-' && str[i+1] != ']') {
                            i++;
                            char end = str[i++];
                            for (char c = start; c <= end; c++) {
                                *ccl++ = c;
                            }
                        } else {
                            *ccl++ = start;
                        }
                    }
                    break;
                }
                default:
                    if (is_special(str[i])) {
                        t->next = makeToken(RE_CHAR, str[i]);
                        t = t->next;
                    }
                    break;
            }
        }
    }
    return header.next;
}

int tokensLength(REToken* list) {
    int len = 0;
    for (REToken* it = list; it != NULL; it = it->next, len++);
    return len;
}

char* toString(REToken* tokens) {
    int len = 1024;
    char* asStr = malloc(sizeof(char)*len);
    int n = 0;
    for (REToken* it = tokens; it != NULL && n < len; it = it->next) {
        if (it->symbol == RE_CCL) {
            for (char *sp = it->ccl; *sp && n < len; sp++) {
                asStr[n++] = *sp;
            }
        } else {
            asStr[n++] = it->ch;
        }
    }
    asStr[n++] = '\0';
    return asStr;
}

void printTokenStream(REToken* tokens) {
    for (REToken* it = tokens; it != NULL; it = it->next) {
        printf("<%d, %c>\n", it->symbol, it->ch);
    }
}

void freeTokenStream(REToken* tokens) {
    REToken* head = tokens;
    while (head != NULL) {
        REToken* x = head;
        head = head->next;
        x->next = NULL;
        free(x);
    }
}