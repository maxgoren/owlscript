#ifndef tokens_h
#define tokens_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum RESymbol {
    RE_CHAR, RE_PERIOD, RE_LPAREN, RE_RPAREN, 
    RE_CCL, RE_STAR, RE_PLUS, RE_QUESTION, 
    RE_CONCAT, RE_OR, RE_EPSILON, RE_NONE
};

extern char* RESymbolStr[];

typedef struct Token_ {
    enum RESymbol symbol;
    char ch;
    char *ccl;
    struct Token_* next;
} REToken;

REToken* makeToken(enum RESymbol sym, char ch);

bool is_digit(char c);

bool is_char(char c);

bool is_special(char c);

REToken* tokenize(char* str);

int tokensLength(REToken* list);

char* toString(REToken* tokens);

void printTokenStream(REToken* tokens);

void freeTokenStream(REToken* tokens);
#ifdef __cplusplus
}
#endif
#endif