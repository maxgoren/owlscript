#include "parser.hpp"

astnode* Parser::statement() {
    astnode* n = nullptr;
    switch (lookahead()) {
        case TK_PRINT:
        case TK_PRINTLN:  n = parsePrintStmt(); break;
        case TK_CLASS:    n = parseClassDef();break;
        case TK_IF:       n = parseIfStmt(); break;
        case TK_FOR:      n = parseForeach(); break;
        case TK_WHILE:    n = parseWhileStmt();break;
        case TK_FN:       n = parseFuncDef(); break;
        case TK_LET:      n = parseVarDec(); break;
        case TK_LCURLY:   n = parseBlock(); break;
        case TK_RETURN:   n = parseReturn(); break;
        case TK_IMPORT:   n = parseImportStmt(); break;
        default:
            n = new astnode(EXPR_STMT, current());
            n->left = expression();
    }
    if (expect(TK_SEMI))
        match(TK_SEMI);
    return n;
}

astnode* Parser::stmt_list() {
    astnode* x = statement();
    astnode* m = x;
    while (!expect(TK_EOI) && !expect(TK_RCURLY)) {
        astnode* q = statement();
        if (m == nullptr) {
            m = x = q;
        } else {
            m->next = q;
            m = q;
        }
    }
    return x;
}

astnode* Parser::argsList() {
    astnode d, *t = &d;
    while (t != nullptr && !expect(TK_RPAREN) && !expect(TK_RB)) {
        if (expect(TK_COMMA))
            match(TK_COMMA);
        t->next = expression();
        t = t->next;
    }
    return d.next;
}
astnode* Parser::paramList() {
    astnode d, *t = &d;
    while (t != nullptr && !expect(TK_RPAREN)) {
        if (expect(TK_COMMA))
            match(TK_COMMA);
        t->next = statement();
        t = t->next;
    }
    return d.next;
}

astnode* Parser::functionBody(astnode* n) {
    n->left = paramList();
    match(TK_RPAREN);
    if (expect(TK_LCURLY)) {
        match(TK_LCURLY);
        n->right = stmt_list();
        match(TK_RCURLY);
    } else if (expect(TK_PRODUCE)) {
        match(TK_PRODUCE);
        n->right = expression();
    }
    return n;
}
astnode* Parser::parseIfStmt() {
    astnode* n = new astnode(IF_STMT, current());
    match(TK_IF);
    match(TK_LPAREN);
    n->left = expression();
    match(TK_RPAREN);
    match(TK_LCURLY);
    n->right = stmt_list();
    match(TK_RCURLY);
    if (expect(TK_ELSE)) {
        astnode* e = new astnode(ELSE_STMT, current());
        match(TK_ELSE);
        if (expect(TK_IF)) {
            e->right = parseIfStmt();
        } else {
            match(TK_LCURLY);
            e->right = stmt_list();
        }
        e->left = n->right;
        n->right = e;
        match(TK_RCURLY);
    }
    return n;
}

astnode* Parser::parseWhileStmt() {
    astnode* n = new astnode(WHILE_STMT, current());
    match(TK_WHILE);
    match(TK_LPAREN);
    n->left = expression();
    match(TK_RPAREN);
    match(TK_LCURLY);
    n->right = stmt_list();
    match(TK_RCURLY);
    return n;
}
astnode* Parser::parseVarDec() {
    astnode* n = new astnode(LET_STMT, current());
    match(TK_LET);
    astnode* t = new astnode(ID_EXPR, current());
    match(TK_ID);
    if (expect(TK_COLON)) {
        match(TK_COLON);
        n->right = new astnode(ID_EXPR, current());
        match (TK_ID);
    }
    if (expect(TK_ASSIGN)) {
        astnode* r = new astnode(BIN_EXPR, current());
        match(TK_ASSIGN);
        r->left = t;
        r->right = expression();
        t = r;
    }
    n->left = t;
    return n;
}
astnode* Parser::parseSequence() {
    match(TK_LCURLY);
    astnode* t = stmt_list();
    match(TK_RCURLY);
    return t;
}
astnode* Parser::parseBlock() {
    astnode* n = new astnode(BLOCK_STMT, current());
    n->left = parseSequence();
    return n;
}
astnode* Parser::parseFuncDef() {
    astnode* n = new astnode(LET_STMT, current());
    astnode* m = new astnode(BIN_EXPR, current());
    m->token.setString(":=");
    m->token.setSymbol(TK_ASSIGN);
    match(TK_FN);
    m->left = new astnode(ID_EXPR, current());
    match(TK_ID);
    m->right = new astnode(LAMBDA_EXPR, current());
    m->right->token.setString(m->left->token.getString());
    match(TK_LPAREN);
    m->right = functionBody(m->right);
    n->left = m;
    return n;
}
astnode* Parser::parseClassDef() {
    astnode* n = new astnode(DEF_CLASS_STMT, current());
    match(TK_CLASS);
    n->left = expression();
    n->right = parseSequence();
    return n;
}
astnode* Parser::parsePrintStmt() {
    astnode* n = new astnode(PRINT_STMT, current());
    match(lookahead());
    n->left = expression();
    return n;
}
astnode* Parser::parseReturn() {
    astnode* n = new astnode(RETURN_STMT, current());
    match(TK_RETURN);
    n->left = expression();
    return n;
}

astnode* Parser::parseImportStmt() {
    astnode* n = new astnode(IMPORT_STMT, current());
    match(TK_IMPORT);
    n->left = expression();
    //match(TK_FROM);
    //n->right = expression();
    return n;
}

astnode* Parser::parseForeach() {
    astnode* n = new astnode(FOREACH_STMT, current());
    match(TK_FOR);
    match(TK_LPAREN);
    n->left = expression();
    match(TK_RPAREN);    
    match(TK_LCURLY);
    n->right = stmt_list();
    match(TK_RCURLY);
    return n;
}