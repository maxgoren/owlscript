#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

char* symbolStr[] = {
    "TK_ID", "TK_NUM", "TK_REALNUM", "TK_STR", "TK_TRUE", "TK_FALSE", "TK_NIL", "TK_LP", "TK_RP", "TK_LC", "TK_RC", "TK_LB", "TK_RB",
    "TK_ADD", "TK_MUL", "TK_SUB", "TK_DIV", "TK_MOD", "TK_POW", "TK_SQRT", "TK_POST_INC", "TK_POST_DEC", "TK_PRE_INC", "TK_PRE_DEC",
    "TK_LT", "TK_GT", "TK_LTE", "TK_GTE", "TK_EQU", "TK_NEQ", "TK_NOT", "TK_AND", "TK_OR", 
    "TK_BIT_AND", "TK_BIT_OR", "TK_BIT_XOR", "TK_BLESS", "TK_BREAK", "TK_CONTINUE",
    "TK_PERIOD", "TK_COMA", "TK_SEMI", "TK_COLON", "TK_AMPER", "TK_QM", "TK_REF","TK_LAMBDA", "TK_RANGE",
    "TK_ASSIGN", "TK_ASSIGN_SUM", "TK_ASSIGN_DIFF", "TK_ASSIGN_PROD", "TK_ASSIGN_DIV", 
    "TK_QUOTE", "TK_FUNC", "TK_PRODUCES", "TK_STRUCT", "TK_NEW", "TK_FREE", "TK_IN",
    "TK_LET", "TK_VAR", "TK_PRINT", "TK_PRINTLN", "TK_WHILE", "TK_FOREACH", "TK_RETURN", "TK_IF", "TK_ELSE",
    "TK_PUSH", "TK_APPEND", "TK_EMPTY", "TK_SIZE", "TK_FIRST", "TK_REST", "TK_MAP", "TK_FILTER", 
    "TK_REDUCE", "TK_REVERSE", "TK_SORT", "TK_PIPE", "TK_MATCHRE", "TK_REMATCH", "TK_TYPEOF", "TK_FOPEN",
    "TK_ERR", "TK_EOI"
};


int nr = 73;

TokenRule rules[] = {
    {"(\\d+)\\.(\\d+)",TK_REALNUM},
    {"[0-9]+", TK_NUM},
    {"if", TK_IF},
    {"in", TK_IN},
    {"let", TK_LET},
    {"var", TK_LET},
    {"def", TK_FUNC},
    {"map", TK_MAP},
    {"nil", TK_NIL},
    {"else",TK_ELSE},
    {"push", TK_PUSH},
    {"size", TK_SIZE},
    {"sort", TK_SORT},
    {"rest", TK_REST},
    {"true", TK_TRUE},
    {"false", TK_FALSE},
    {"break", TK_BREAK},
    {"fopen", TK_FOPEN},
    {"first", TK_FIRST},
    {"while", TK_WHILE},
    {"print", TK_PRINT},
    {"empty", TK_EMPTY},
    {"bless", TK_BLESS},
    {"lambda", TK_LAMBDA},
    {"filter", TK_FILTER},
    {"reduce", TK_REDUCE},
    {"return", TK_RETURN},
    {"struct", TK_STRUCT},
    {"typeOf", TK_TYPEOF},
    {"append", TK_APPEND},
    {"matchre", TK_MATCHRE},
    {"println", TK_PRINTLN},
    {"foreach", TK_FOREACH},
    {"reverse", TK_REVERSE},
    {"+=", TK_ASSIGN_SUM},
    {"[A-Za-z][A-Za-z0-9_]*", TK_ID},
    {"\\\".*\\\"", TK_STR},
    {"\\(", TK_LP},
    {"\\)", TK_RP},
    {"\\[", TK_LB},
    {"\\]", TK_RB},
    {"{", TK_LC},
    {"}", TK_RC},
    {",", TK_COMA},
    {";", TK_SEMI},
    {"!",TK_NOT},
    {"&", TK_BIT_AND},
    {"\\|", TK_BIT_OR},
    {"&&", TK_AND},
    {"\\|\\|", TK_OR},
    {"\\+", TK_ADD},
    {"\\+\\+", TK_POST_INC},
    {"\\+=", TK_ASSIGN_SUM},
    {"-", TK_SUB},
    {"--", TK_POST_DEC},
    {"-=", TK_ASSIGN_DIFF},
    {"\\*", TK_MUL},
    {"\\*=", TK_ASSIGN_PROD},
    {"/", TK_DIV},
    {"%", TK_MOD},
    {"<", TK_LT},
    {">", TK_GT},
    {"=~", TK_REMATCH},
    {"==", TK_EQU},
    {"!=", TK_NEQ},
    {"<=", TK_LTE},
    {">=", TK_GTE},
    {":=", TK_ASSIGN},
    {"->", TK_PRODUCES},
    {"&\\(", TK_LAMBDA},
    {"\\.", TK_PERIOD},
    {"\\.\\.", TK_RANGE},
    {"<fin.>", TK_EOI}
};

void tag_final_pos_with_token_id(re_ast* ast, int rulenum) {
    if (ast != NULL) {
        if (isLeaf(ast) && ast->token.ch == '#')
            ast->tk_token_id = rulenum;
        tag_final_pos_with_token_id(ast->left, rulenum);
        tag_final_pos_with_token_id(ast->right, rulenum);
    }
}

CombinedRE* combine(int numrules) {
    re_ast* root = re2ast(augmentRE(rules[0].pattern));
    tag_final_pos_with_token_id(root, 0);
    char* re = malloc(sizeof(char)*1024);
    re[0] = '(';
    int p = 1;
    char* pat = augmentRE(rules[0].pattern);
    for (int k = 0; k < strlen(pat); k++)
        re[p++] = pat[k];
    for (int i = 1; i < numrules; i++) {
        re[p++] = '|';
        re_ast* tmp = re2ast(augmentRE(rules[i].pattern));
        tag_final_pos_with_token_id(tmp, i);
        re_ast* alt = makeNode(1, *makeToken(RE_OR, '|'));
        alt->left = root;
        alt->right = tmp;
        root = alt;
        pat = augmentRE(rules[i].pattern);
        for (int k = 0; k < strlen(pat); k++)
            re[p++] = pat[k];
    }
     re[p++] = ')';
    re[p++] = '\0';
    CombinedRE* cre = malloc(sizeof(CombinedRE));
    cre->pattern = re; //augmentRE(re);
    cre->patlen = strlen(cre->pattern);
    cre->ast = root;
    return cre;
}

CombinedRE* init_lex_dfa(int num_rules) {
    return combine(num_rules);
}

//Classic Maximal-much: Prefer longest match, and equal length matches are chosen by priority
//with priority being order in the rule list.
TKToken* nextToken(DFA* dfa, const char* input) {
    int curr = 1;
    int last_accept = -1;
    int last_token_id = -1;
    int i = 0;
    for (; input[i] != '\0'; i++) {
        int next = -1;
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", dfa->states[curr]->label, input[i]);
#endif
        for (Transition* it = dfa->dtrans[curr]; it != NULL; it = it->next) {
            if (input[i] == it->ch || ast_node_table[curr]->token.symbol == RE_PERIOD) {
                next = it->to;
                if (it->ch == '"') {
                    i++;
                    while (input[i] != '\0' && input[i] != '"')
                        i++;
                    return makeTKToken(36,i+1);
                }
                break;
            }
        }
        if (next == -1) {
            break;
        }
        curr = next;
        if (dfa->states[curr]->is_accepting) { 
            if (last_accept < i+1) {
                last_accept = i+1;
                last_token_id = dfa->states[curr]->token_id;
            } else {
                if (dfa->states[curr]->token_id < dfa->states[last_accept]->token_id) {
                    last_accept = i+1;
                    last_token_id = dfa->states[curr]->token_id;
                }
            }
        }
    }
    TKToken* t;
    if (last_accept == -1) {
        t = makeTKToken(-1, 0);
    } else {
        t = makeTKToken(last_token_id, last_accept);
    }
    return t;
}

bool in_string;
TKTokenListNode* lex_input(DFA* dfa, char* input) {
    char* p = input;
    TKTokenListNode dummy;
    TKTokenListNode* thead = &dummy; 
    in_string = false;
    while (*p != '\0') {
        // skip whitespace
        if (!in_string)
            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;

        TKToken* t = nextToken(dfa, p);
        if (t->rule_id == -1) {
            printf("Lex error near: %s\n", p);
            break;
        }
        thead->next = makeTokenListNode(t);
        thead = thead->next;
        int i, j;
        if (rules[t->rule_id].token == TK_STR) {
            for (i = 0, j = 1; j < t->length-1;) {
                    t->text[i++] = p[j++];
            }
            t->text[i++] = '\0';
        } else {
            for (i = 0, j = 0; i < t->length;) {
                    t->text[i++] = p[j++];
            }
            t->text[i++] = '\0';
        }
        p += t->length;
    }
    thead->next = makeTokenListNode(makeTKToken(nr-1, 5));
    thead->next->token->text = "EOI";
#ifdef DEBUG
    for (TKTokenListNode* it = dummy.next; it != NULL; it = it->next) {
        printf("<%d, %s, %s> \n", rules[it->token->rule_id].token, rules[it->token->rule_id].pattern, it->token->text);
    }
#endif
    return dummy.next;
}

TKToken* makeTKToken(int rid, int len) {
    TKToken* tkt = malloc(sizeof(TKToken));
    tkt->rule_id = rid;
    tkt->length = len;
    if (rid != -1 && len > 0) {
        tkt->text = malloc(sizeof(char)*(len+1));
    }
    return tkt;
}

TKTokenListNode* makeTokenListNode(TKToken* token) {
    TKTokenListNode* tn = malloc(sizeof(TKTokenListNode));
    tn->token = token;
    tn->next = NULL;
    return tn;
}