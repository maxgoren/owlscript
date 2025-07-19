#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "lex_spec.h"

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
TKToken* nextToken(DFA* dfa, const char* input, re_ast** ast_node_table) {
    int curr = 1;
    int last_accept = -1;
    int last_token_id = -1;
    int i = 0;
    for (; input[i] != '\0'; i++) {
        int next = -1;
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", dfa->states[curr]->label, input[i]);
#endif
        Transition* it = findTransition(dfa->dtrans[curr], input[i]);
        if (it != NULL) {
            if (input[i] == it->ch || ast_node_table[curr]->token.symbol == RE_PERIOD) {
                if (it->ch == '"') {
                    i++;
                    while (input[i] != '\0' && input[i] != '"')
                        i++;
                    if (input[i] != '"') {
                        printf("Unterminated string!\n");
                    } else {
                        i++;
                    }
                    return makeTKToken(36,i);
                } else {
                    next = it->to;
                }
            }
        } else if (ast_node_table[curr]->token.symbol == RE_PERIOD) {
            it = findTransition(dfa->dtrans[curr], '.');
            if (it != NULL)
                next = it->to;
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
TKTokenListNode* lex_input(DFA* dfa, char* input, re_ast** ast_node_table) {
    char* p = input;
    TKTokenListNode dummy;
    TKTokenListNode* thead = &dummy; 
    in_string = false;
    int line_number = 0;
    while (*p != '\0') {
        // skip whitespace
        if (!in_string)
            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
                if (*p == '\n') line_number++;
                p++;
            } 

        TKToken* t = nextToken(dfa, p, ast_node_table);
        if (t->rule_id == -1) {
            printf("Lex error near: %s\n", p);
            break;
        }
        t->lineno = line_number;
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
    tkt->lineno = 0;
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