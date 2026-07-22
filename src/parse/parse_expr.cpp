#include "parser.hpp"

astnode* Parser::parseFunctionCallAndSubscripts(astnode* n) {
    while (expect(TK_LPAREN) || expect(TK_LB) || expect(TK_PERIOD)) {
        if (expect(TK_LPAREN)) {
            astnode* fc = new astnode(FUNC_EXPR, current());
            fc->left = n;
            match(TK_LPAREN);
            fc->right = argsList();
            match(TK_RPAREN);
            n = fc;
        } else if (expect(TK_LB)) {
            astnode* ss = new astnode(SUBSCRIPT_EXPR, current());
            match(TK_LB);
            ss->left = n;
            ss->right = expression();
            match(TK_RB);
            n = ss;
        } else if (expect(TK_PERIOD)) {
            astnode* ma = new astnode(FIELD_EXPR, current());
            match(TK_PERIOD);
            ma->left = n;
            ma->right = primary();
            if (ma->right->expr == LIST_EXPR)
            ma->expr = LIST_EXPR;
            n = ma;
        }
    }
    return n;
}
astnode* Parser::primary() {
    astnode* n = nullptr;
    if (expect(TK_NUM)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_NUM);
    } else if (expect(TK_STRING)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_STRING);
    } else if (expect(TK_ID)) {
        n = new astnode(ID_EXPR, current());
        match(TK_ID);
    } else if (expect(TK_TRUE)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_TRUE);
    } else if (expect(TK_FALSE)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_FALSE);
    } else if (expect(TK_NIL)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_NIL);
    } else if (expect(TK_LPAREN)) {
        match(TK_LPAREN);
        n = expression();
        match(TK_RPAREN);
    } else if (expect(TK_LAMBDA)) {
        n = new astnode(LAMBDA_EXPR, current());
        match(TK_LAMBDA);
        n = functionBody(n);
    } else if (expect(TK_LB)) {
        n = new astnode(LISTCON_EXPR, current());
        match(TK_LB);
        in_list_consxr = true;
        n->left = argsList();
        in_list_consxr = false;
        match(TK_RB);
    } else if (expect(TK_APPEND)) {
        n = new astnode(LIST_EXPR, current());
        match(TK_APPEND);
        match(TK_LPAREN);               
        n->left = expression();
        match(TK_RPAREN);
    } else if (expect(TK_PUSH)) {
        n = new astnode(LIST_EXPR, current());
        match(TK_PUSH);
        match(TK_LPAREN);
        n->left = expression();
        match(TK_RPAREN);
    } else if (expect(TK_POP)) {
        n = new astnode(LIST_EXPR, current());
        match(TK_POP);
        match(TK_LPAREN);
        match(TK_RPAREN);
    } else if (expect(TK_SIZE)) {
        n = new astnode(LIST_EXPR, current());
        match(TK_SIZE);
        match(TK_LPAREN);
        match(TK_RPAREN);
    } else if (expect(TK_EMPTY)) {
        n = new astnode(LIST_EXPR, current());
        match(TK_EMPTY);
        match(TK_LPAREN);
        match(TK_RPAREN);
    } else if (expect(TK_NEW)) {
        n = new astnode(BLESS_EXPR, current());
        match(TK_NEW);
        n->left = new astnode(ID_EXPR, current());
        match(TK_ID);
        match(TK_LPAREN);
        n->right = argsList();
        match(TK_RPAREN);
    } else if (expect(TK_RANDOM)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_RANDOM);
        match(TK_LPAREN);
        if (!expect(TK_RPAREN)) {
            n->left = primary();
        }
        match(TK_RPAREN);
    } else if (expect(TK_FLOOR)) {
        n = new astnode(UOP_EXPR, current());
        match(TK_FLOOR);
        match(TK_LPAREN);
        n->left = expression();
        match(TK_RPAREN);
    }
    if (n != nullptr && (n->expr == ID_EXPR || n->expr == LIST_EXPR || n->expr == LAMBDA_EXPR)) {
        n = parseFunctionCallAndSubscripts(n);
    }
    return n;
}
astnode* Parser::unary() {
    astnode* n = nullptr;
    if (expect(TK_SUB) || expect(TK_NOT)) {
        n = new astnode(UOP_EXPR, current());
        match(lookahead());
        n->left = unary();
    } else {
        n = primary();
    }
    if (expect(TK_INCREMENT) || expect(TK_DECREMENT)) {
        astnode* t = new astnode(UOP_EXPR, current());
        match(lookahead());
        t->left = n;
        n = t;
    }
    return n;
}

//for (i of [1 .. 3] as &(let i) -> i+i) { println i; }

astnode* Parser::listOp() {
    astnode* n = unary();
    if (expect(TK_RANGE)) {
        astnode* t = new astnode(RANGE_EXPR, current());
        match(TK_RANGE);
        t->left = n;
        t->right = unary();
        n = t;
    }
    if (expect(TK_OF)) {
        astnode* t = new astnode(ITERATOR_EXPR, current());
        match(TK_OF);
        t->left = n;
        t->right = unary();
        n = t;
    }
    if ((expect(TK_LOGIC_OR) || expect(TK_AS)) && in_list_consxr) {
        astnode* t = new astnode(SETCOMP_EXPR, current());
        match(lookahead());
        t->left = n;
        t->right = unary();
        n = t;
        if (expect(TK_LOGIC_OR) || expect(TK_IF)) {
            match(TK_IF);
            n->right->next = unary();
        }
    }
    return n;
}
astnode* Parser::factor() {
    astnode* n = listOp();
    while (expect(TK_MUL) || expect(TK_DIV) || expect(TK_MOD)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = listOp();
        n = q;
    }
    return n;
}
astnode* Parser::term() {
    astnode* n = factor();
    while (expect(TK_ADD) || expect(TK_SUB)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = factor();
        n = q;
    }
    return n;
}
astnode* Parser::relopExpr() {
    astnode* n = term();
    while (expect(TK_LT) || expect(TK_GT) || expect(TK_LTE) || expect(TK_GTE)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = term();
        n = q;
    }
    return n;
}
astnode* Parser::compExpr() {
    astnode* n = relopExpr();
    while (expect(TK_EQU) || expect(TK_NEQ) || expect(TK_MATCHRE)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = relopExpr();
        n = q;
    }
    return n;
}
astnode* Parser::logicalExpr() {
    astnode* n = compExpr();
    while (expect(TK_LOGIC_AND) || expect(TK_LOGIC_OR)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = compExpr();
        n = q;
    }
    return n;
}
astnode* Parser::assignExpr() {
    astnode* n = logicalExpr();
    while (expect(TK_ASSIGN) || expect(TK_ASSIGN_DIFF) || expect(TK_ASSIGN_SUM)) {
        astnode* q = new astnode(BIN_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = logicalExpr();
        n = q;
    }
    return n;
}
astnode* Parser::expression() {
    astnode* n = assignExpr();
    if (expect(TK_QM)) {
        astnode* q = new astnode(TERNARY_EXPR, current());
        match(lookahead());
        q->left = n;
        q->right = new astnode(ELSE_STMT, current());
        q->right->left = assignExpr();
        match(TK_COLON);
        q->right->right = assignExpr();
        n = q;
    }
    return n;
}