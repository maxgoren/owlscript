#include "transition.h"

int height(Transition* node) {
    return node == NULL ? -1:node->height;
}

int max(int a, int b) {
    return (a > b) ? a:b;
}

Transition* rotL(Transition* node) {
    Transition* x = node->right; node->right = x->left; x->left = node;
    node->height = 1 + max(height(node->left), height(node->right));
    x->height = 1 + max(height(x->left), height(x->right));
    return x;
}

Transition* rotR(Transition* node) {
    Transition* x = node->left; node->left = x->right; x->right = node;
    node->height = 1 + max(height(node->left), height(node->right));
    x->height = 1 + max(height(x->left), height(x->right));
    return x;
}

int balanceFactor(Transition* node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}


Transition* addTransition(Transition* trans, int to, char ch) {
    if (trans == NULL)
        return makeTransition(to, ch);
    if (trans->to == to && trans->ch == ch)
            return trans;
    if (ch < trans->ch) {
        trans->left = addTransition(trans->left, to, ch);
    } else {
        trans->right = addTransition(trans->right,  to, ch);
    }
    trans->height = 1 + max(height(trans->left), height(trans->right));
    if (balanceFactor(trans) < -1) {
        if (balanceFactor(trans->right) > 0)
            trans->right = rotR(trans->right);
        trans = rotL(trans);
    } else if (balanceFactor(trans) > 1) {
        if (balanceFactor(trans->left) < 0)
            trans->left = rotL(trans->left);
        trans = rotR(trans);
    }
    return trans;
}

Transition* findTransition(Transition* node, char ch) {
    if (node == NULL)
        return NULL;
    if (node->ch == ch) {
        return node;
    }
    if (ch < node->ch) return findTransition(node->left, ch);
    else return findTransition(node->right, ch);
    return node;
}

int printTransitions(Transition* root, int src) {
    Transition* st[255];
    int stsp = 0;
    int tc = 0;
    Transition* it = root;
    while (it != NULL) {
        st[++stsp] = it;
        it = it->left;
    } 
    while (stsp > 0) {
        it = st[stsp--];
        if (it != NULL) {
            printf("{%d -(%c)-> %d} ", src, it->ch, it->to);
            tc++;
            it = it->right;
            while (it != NULL) {
                st[++stsp] = it;
                it = it->left;
            }
        }
    }
    printf("\n");
    return tc;
}

void cleanTransTree(Transition* node) {
    if (node != NULL) {
        cleanTransTree(node->left);
        cleanTransTree(node->right);
        free(node);
    }
}