#include "parser.hpp"

Parser::Parser(bool debug) {
    loud = debug;
    listExprs = { TK_LENGTH, TK_EMPTY, TK_REST, TK_FIRST, TK_SORT, TK_MAP, TK_PUSH, TK_POP, TK_APPEND, TK_LSQUARE };
}

astnode* Parser::parse(vector<Token> in) {
    init(in);
    return program();
}

Symbol Parser::currSym() {
    return current.symbol;
}

Symbol Parser::lookahead() {
    if (tpos+1 < tokens.size())
        return tokens[tpos+1].symbol;
    return TK_EOF;
}

bool Parser::match(Symbol s) {
    if (s == currSym()) {
        if (loud)
            cout<<"Matched: "<<symbolAsString[s]<<" - "<<current.strval<<endl;
        advance();
        return true;
    }
    cout<<"Unexpected Token: "<<symbolAsString[s]<<" - "<<current.strval<<endl;
    return false;
}

void Parser::advance() {
    if (tpos+1 < tokens.size()) {
        tpos++;
        current = tokens[tpos];
        return;
    }
    else current = Token(TK_EOF, "<eof>");
}

void Parser::init(vector<Token> in) {
    tokens = in;
    tpos = 0;
    current = tokens[tpos];
}

astnode* Parser::makeLetStatement() {
    astnode* m = makeStmtNode(LET_STMT, current);
    match(TK_LET);
    if (currSym() == TK_ID) {
        m->child[0] = simpleExpr();
    }
    return m;
}

astnode* Parser::makePrintStatement() {
    astnode* m = makeStmtNode(PRINT_STMT, current);
    match(TK_PRINT);
    m->child[0] = simpleExpr();
    return m;
}

astnode* Parser::makeExprStatement() {
    astnode* m = makeStmtNode(EXPR_STMT, current);
    m->child[0] = simpleExpr();
    return m;
}

astnode* Parser::makeWhileStatement() {
    astnode* m = makeStmtNode(WHILE_STMT, current);
    match(TK_WHILE);
    match(TK_LPAREN);
    m->child[0] = simpleExpr();
    match(TK_RPAREN);
    match(TK_LCURLY);
    m->child[1] = statementList();
    match(TK_RCURLY);
    return m;
}

astnode* Parser::makeForStatement() {
    astnode* m = makeStmtNode(FOR_STMT, current);
    match(TK_FOR);
    match(TK_LPAREN);
    m->child[0] = simpleExpr();
    match(TK_SEMI);
    m->child[0]->next = simpleExpr();
    match(TK_SEMI);
    m->child[0]->next->next = simpleExpr();
    match(TK_RPAREN);
    match(TK_LCURLY);
    m->child[1] = statementList();
    match(TK_RCURLY);
    return m;
}

astnode* Parser::makeIfStatement() {
    astnode* m = makeStmtNode(IF_STMT, current);
    match(TK_IF);
    match(TK_LPAREN);
    m->child[0] = simpleExpr();
    match(TK_RPAREN);
    match(TK_LCURLY);
    m->child[1] = statementList();
    match(TK_RCURLY);
    if (currSym() == TK_ELSE) {
        match(TK_ELSE);
        match(TK_LCURLY);
        m->child[2] = statementList();
        match(TK_RCURLY);
    }
    return m;
}

astnode* Parser::makeReturnStatement() {
    astnode* m = makeStmtNode(RETURN_STMT, current);
    match(TK_RETURN);
    m->child[0] = simpleExpr();
    return m;
}

astnode* Parser::paramList() {
    astnode* m = simpleExpr();
    astnode* c = m;
    if (currSym() == TK_COMMA) {
        do {
            match(TK_COMMA);
            if (currSym() == TK_ID || currSym() == TK_NUM || currSym() == TK_STRING || currSym() == TK_LPAREN) {
                c->next = simpleExpr();
                c = c->next;
            }
        }  while (currSym() != TK_RPAREN && currSym() == TK_COMMA);
        if (currSym() == TK_RPAREN) 
            match(TK_RPAREN);
    }
    return m;
}

astnode* Parser::argsList() {
    astnode* m = simpleExpr();
    astnode* c = m;
    while (currSym() == TK_COMMA) {
        match(TK_COMMA);
        c->next = simpleExpr();
        c = c->next;
    }
    return m;
}

astnode* Parser::makeDefStatement() {
    astnode* m = makeStmtNode(DEF_STMT, current);
    match(TK_DEF);
    if (currSym() == TK_ID) {
        m->attributes.strval = current.strval;
        match(TK_ID);
    }
    match(TK_LPAREN);
    m->child[1] = paramList();
    match(TK_RPAREN);
    match(TK_LCURLY);
    m->child[0] = statementList();
    match(TK_RCURLY);
    return m;
}

astnode* Parser::program() {
    return statementList();
}

astnode* Parser::statementList() {
    astnode* m = statement();
    astnode* c = m;
    while (currSym() != TK_COMMA && currSym() != TK_RCURLY && currSym() != TK_EOF) {
        if (currSym() == TK_SEMI)
            match(TK_SEMI);
        astnode* q = statement();
        if (q != nullptr) {
            if (m == nullptr) 
                m = c = q;
            else {
                c->next = q;
                c = c->next;
            }
        }
    }
    return m;
}

astnode* Parser::statement() {
    astnode* m = nullptr;
    switch (currSym()) {
        case TK_PRINT: 
            m = makePrintStatement();
            return m;
        case TK_LET: 
            m = makeLetStatement();
            return m;
        case TK_ID:
            m = makeExprStatement();
            return m;
        case TK_LPAREN:
            m = makeExprStatement();
            return m;
        case TK_NUM:
            m = makeExprStatement();
            return m;
        case TK_REALNUM:
            m = makeExprStatement();
            return m;
        case TK_AMPER:
            m = makeExprStatement();
            return m;
        case TK_IF: 
            m = makeIfStatement();
            return m;
        case TK_WHILE: 
            m = makeWhileStatement();
            return m;
        case TK_FOR:
            m = makeForStatement();
            return m;
        case TK_DEF: 
            m = makeDefStatement();
            return m;
        case TK_RETURN:
            m = makeReturnStatement();
            return m;
        case TK_LSQUARE:
        case TK_APPEND:
        case TK_PUSH:
        case TK_POP:
        case TK_LENGTH:
        case TK_SORT:
            m = makeExprStatement();
            return m;
        default:
            break;
    }
    return m;
}

astnode* Parser::simpleExpr() {
    astnode* node = expr();
    while (isRelOp(currSym())) {
        astnode* m = makeExprNode(RELOP_EXPR, current);
        match(currSym());
        m->child[0] = node;
        node = m;
        node->child[1] = expr();
    }
    return node;
}

astnode* Parser::expr() {
    astnode* node = term();
    while (currSym() == TK_ADD || currSym() == TK_SUB) {
        astnode* m = makeExprNode(BINARYOP_EXPR, current);
        match(currSym());
        m->child[0] = node;
        node = m;
        node->child[1] = term();
    }
    return node;
}

astnode* Parser::term() {
    astnode* node = factor();
    while (currSym() == TK_MUL || currSym() == TK_DIV) {
        astnode* m = makeExprNode(BINARYOP_EXPR, current);
        match(currSym());
        m->child[0] = node;
        node = m;
        node->child[1] = factor();
    }
    return node;
}

astnode* Parser::factor() {
    if (currSym() == TK_SUB) {
        astnode* m = makeExprNode(UNARYOP_EXPR, current);
        match(TK_SUB);
        m->child[0] = simpleExpr();
        return m;
    }
    if (currSym() == TK_BANG) {
        astnode* m = makeExprNode(UNARYOP_EXPR, current);
        match(TK_BANG);
        m->child[0] = simpleExpr();
        return m;
    }
    if (currSym() == TK_ID) {
        astnode* m = makeExprNode(ID_EXPR, current);
        match(TK_ID);
        if (currSym() == TK_LSQUARE) {
            astnode* t = makeExprNode(SUBSCRIPT_EXPR, current);
            match(TK_LSQUARE);
            t->child[0] = m;
            m = t;
            m->child[1] = simpleExpr();
            match(TK_RSQUARE);
        }
        if (currSym() == TK_ASSIGN) {
            astnode* t = makeExprNode(ASSIGN_EXPR, current);
            match(TK_ASSIGN);
            t->child[0] = m;
            m = t;
            m->child[1] = simpleExpr();
        } else if (currSym() == TK_LPAREN) {
            astnode* t = makeExprNode(FUNC_EXPR, current);
            t->attributes = m->attributes;
            t->child[0] = m;
            m = t;
            match(TK_LPAREN);
            if (currSym() == TK_RPAREN) {
                match(TK_RPAREN);
                return m;
            } else {
                m->child[1] = argsList();
                match(TK_RPAREN);
            }
        }
        return m; 
    }
    if (currSym() == TK_STRING) {
        astnode* m = makeExprNode(CONST_EXPR, current);
        match(TK_STRING);
        return m;
    }
    if (currSym() == TK_NUM) {
        astnode* m = makeExprNode(CONST_EXPR, current);
        match(TK_NUM);
        return m;
    }
    if (currSym() == TK_REALNUM) {
        astnode* m = makeExprNode(CONST_EXPR, current);
        match(TK_REALNUM);
        return m;
    }
    if (currSym() == TK_LPAREN) {
        match(TK_LPAREN);
        astnode* m = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_LAMBDA || currSym() == TK_AMPER) {
        astnode* m = makeExprNode(LAMBDA_EXPR, current);
        match(currSym());
        if (currSym() == TK_ID) {
            m->attributes.strval = current.strval;
            match(TK_ID);
        }
        match(TK_LPAREN);
        m->child[1] = paramList();
        match(TK_RPAREN);
        if (currSym() == TK_LCURLY) {
            match(TK_LCURLY);
            m->child[0] = statementList();
            match(TK_RCURLY);
        } else if (currSym() == TK_PRODUCE) {
            match(TK_PRODUCE);
            m->child[0] = statement();
        }
        return m;
    }
    if (listExprs.find(currSym()) != listExprs.end()) {
        return makeListExpr();
    }
    
    return nullptr;
}

astnode* Parser::makeListExpr() {
    astnode* m;
    if (currSym() == TK_LSQUARE) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_LSQUARE);
        m->child[0] = argsList();
        match(TK_RSQUARE);
        return m;
    }
    if (currSym() == TK_APPEND) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_APPEND);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_COMMA);
        m->child[1] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_PUSH) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_PUSH);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_COMMA);
        m->child[1] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_POP) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_POP);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_LENGTH) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_LENGTH);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_EMPTY) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_EMPTY);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_SORT) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_SORT);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_FIRST) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_FIRST);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_REST) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_REST);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_MAP) {
        m = makeExprNode(LIST_EXPR, current);
        match(TK_MAP);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_COMMA);
        m->child[1] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    return m;
}