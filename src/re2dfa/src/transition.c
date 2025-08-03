#include "transition.h"

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
    return balanceAVL(trans);
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

// performs an iterative in-order traversal over the
// tree storing the transitions for a given state.
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

int height(Transition* node) {
    return node == NULL ? -1:node->height;
}

int max(int a, int b) {
    return (a > b) ? a:b;
}

Transition* makeTransition(int to, char ch) {
    Transition* nt = malloc(sizeof(Transition));
    nt->to = to;
    nt->ch = ch;
    nt->wc_period = false;
    nt->height = -1;
    nt->left = NULL;
    nt->right = NULL;
    return nt;
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

// Uses rotations to ensure that the tree 
// rooted at 'trans' has children whos height 
// differs by at most one (AVL Tree)
Transition* balanceAVL(Transition* trans) {
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