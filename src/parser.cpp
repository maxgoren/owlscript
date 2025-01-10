#include "parser.hpp"

Parser::Parser(bool debug) {
    loud = false;
    listExprs = { TK_LENGTH, TK_EMPTY, TK_REST, TK_FIRST, TK_SORT, TK_MAP, TK_FILTER, TK_PUSH, TK_POP, TK_APPEND, TK_LSQUARE };
}

astnode* Parser::parse(TokenStream& in) {
    init(in);
    return program();
}

inline Symbol Parser::currSym() {
    return current.symbol;
}

bool Parser::match(Symbol s) {
    if (s == currSym()) {
        if (loud)
            cout<<"Matched: "<<symbolAsString[s]<<" - "<<current.strval<<endl;
        advance();
        return true;
    }
    cout<<"Unexpected Token on line "<<current.lineNumber<<": "<<symbolAsString[s]<<", was Expecting: "<<symbolAsString[current.symbol]<<endl;
    return false;
}

void Parser::advance() {
    ts.advance();
    if (!ts.done()) {
        current = ts.get();
        return;
    }
    else current = Token(TK_EOF, "<eof>");
}

void Parser::init(TokenStream& in) {
    ts = in;
    ts.start();
    current = ts.get();
}

astnode* Parser::makeLetStatement() {
    astnode* m = makeStmtNode(LET_STMT, current);
    if (currSym() == TK_LET || currSym() == TK_VAR)
        match(currSym());
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

astnode* Parser::makeBlockStatement() {
    astnode* m = makeStmtNode(BLOCK_STMT, current);
    m->child[0] = makeBlock();
    return m;
}

astnode* Parser::makeBlock() {
    match(TK_LCURLY);
    astnode* m = statementList();
    match(TK_RCURLY);
    return m;
}

astnode* Parser::makeWhileStatement() {
    astnode* m = makeStmtNode(WHILE_STMT, current);
    match(TK_WHILE);
    match(TK_LPAREN);
    m->child[0] = simpleExpr();
    match(TK_RPAREN);
    m->child[1] = makeBlock();    
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
    m->child[1] = makeBlock();
    return m;
}

astnode* Parser::makeIfStatement() {
    astnode* m = makeStmtNode(IF_STMT, current);
    match(TK_IF);
    match(TK_LPAREN);
    m->child[0] = simpleExpr();
    match(TK_RPAREN);
    m->child[1] = makeBlock();
    if (currSym() == TK_ELSE) {
        match(TK_ELSE);
        m->child[2] = makeBlock();
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
    match(TK_VAR);
    astnode* m = nullptr;
    if (currSym() == TK_REF) {
        m = makeStmtNode(REF_STMT, current);
        match(TK_REF);
        m->child[0] = simpleExpr();
    } else {
        m = simpleExpr();
    }
    astnode* c = m;
    if (currSym() == TK_COMMA) {
        do {
            match(TK_COMMA);
            match(TK_VAR);
            if (currSym() == TK_REF) {
                c->next = makeStmtNode(REF_STMT, current);
                match(TK_REF);
                c->next->child[0] = simpleExpr();
            } else {
                c->next = simpleExpr();
            }
            c = c->next;
        }  while (currSym() != TK_RPAREN && currSym() == TK_COMMA);
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
    if (currSym() == TK_DEF)
        match(TK_DEF);
    else if (currSym() == TK_VAR)
        match(TK_VAR);
    if (currSym() == TK_ID) {
        m->attributes.strval = current.strval;
        match(TK_ID);
    }
    match(TK_LPAREN);
    if (currSym() != TK_RPAREN)
        m->child[1] = paramList();
    match(TK_RPAREN);
    m->child[0] = makeBlock();
    return m;
}

astnode* Parser::makeStructStatement() {
    astnode* m = makeStmtNode(STRUCT_STMT, current);
    match(TK_STRUCT);
    m->attributes.strval = current.strval;
    match(TK_ID);
    m->child[0] = makeBlock();
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
        case TK_VAR:
            m = makeLetStatement();
            return m;
        case TK_ID:
            m = makeExprStatement();
            return m;
        case TK_LPAREN:
            m = makeExprStatement();
            return m;
        case TK_LCURLY:
            m = makeBlockStatement();
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
        case TK_STRUCT:
            m = makeStructStatement();
            return m;
        case TK_RETURN:
            m = makeReturnStatement();
            return m;
        case TK_LSQUARE:
        case TK_APPEND:
        case TK_PUSH:
        case TK_POP:
        case TK_LENGTH:
        case TK_MATCH:
        case TK_SORT:
            m = makeExprStatement();
            return m;
        default:
            break;
    }
    return m;
}


astnode* Parser::simpleExpr() {
    astnode* node = compExpr();
    while (isEqualityOp(currSym())) {
        astnode* m = makeExprNode(RELOP_EXPR, current);
        match(currSym());
        m->child[0] = node;
        node = m;
        node->child[1] = compExpr();
    }
    return node;
}

astnode* Parser::compExpr() {
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
    while (currSym() == TK_MUL || currSym() == TK_DIV || currSym() == TK_MOD) {
        astnode* m = makeExprNode(BINARYOP_EXPR, current);
        match(currSym());
        m->child[0] = node;
        node = m;
        node->child[1] = factor();
    }
    return node;
}

//right associative, so right recursive instead of loop.
astnode* Parser::factor() {
     if (currSym() == TK_SQRT) {
        astnode* m = makeExprNode(UNARYOP_EXPR, current);
        match(TK_SQRT);
        m->child[0] = factor();
        return m;
    }
    astnode* node = var();
    if (currSym() == TK_POW) {
        astnode* m = makeExprNode(BINARYOP_EXPR, current);
        match(TK_POW);
        m->child[0] = node;
        m->child[1] = factor();
        return m;
    }
    return node;
}

astnode* Parser::var() {
    astnode* node;
    if (currSym() == TK_SUB) {
        node = makeExprNode(UNARYOP_EXPR, current);
        match(TK_SUB);
        node->child[0] = var();
        return node;
    }
    node = range();
    return node;
}

astnode* Parser::range() {
    astnode* node = primary();
    if (currSym() == TK_ELIPSE) {
        astnode* t = makeExprNode(RANGE_EXPR, current);
        match(TK_ELIPSE);
        t->child[0] = node;
        node = t;
        t->child[1] = primary();
    }
    if (currSym() == TK_PIPE) {
        astnode* t = makeExprNode(LISTCOMP_EXPR, current);
        match(TK_PIPE);
        t->child[0] = node;
        node = t;
        node->child[1] = primary();
        if (currSym() == TK_PIPE) {
            match(TK_PIPE);
            node->child[2] = primary();
        }
    }
    
    return node;
}

astnode* Parser::primary() {
    if (currSym() == TK_ID) {
        return makeIDExpr();
    }
    if ((currSym() == TK_NIL) || (currSym() == TK_STRING) ||
        (currSym() == TK_NUM) || (currSym() == TK_REALNUM) || 
        (currSym() == TK_TRUE) || (currSym() == TK_FALSE)) {
        return makeConstExpr();
    }
    if (currSym() == TK_SUB) {
        astnode* m = makeExprNode(UNARYOP_EXPR, current);
        match(TK_SUB);
        m->child[0] = simpleExpr();
        return m;
    }
    if (currSym() == TK_LOGIC_NOT) {
        astnode* m = makeExprNode(UNARYOP_EXPR, current);
        match(TK_LOGIC_NOT);
        m->child[0] = simpleExpr();
        return m;
    }
    if (currSym() == TK_LPAREN) {
        match(TK_LPAREN);
        astnode* m = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (currSym() == TK_LAMBDA || currSym() == TK_AMPER) {
        return makeLambdaExpr();
    }
    if (currSym() == TK_MAKE) {
        astnode* m = makeExprNode(BLESS_EXPR, current);
        match(TK_MAKE);
        m->child[0] = simpleExpr();
        return m;
    }
    if (currSym() == TK_MATCH) {
        astnode* m = makeExprNode(REG_EXPR, current);
        match(TK_MATCH);
        match(TK_LPAREN);
        m->child[0] = simpleExpr();
        match(TK_COMMA);
        m->child[1] = simpleExpr();
        match(TK_RPAREN);
        return m;
    }
    if (listExprs.find(currSym()) != listExprs.end()) {
        return makeListExpr();
    }
    return nullptr;
}

astnode* Parser::makeIDExpr() {
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
        m->exprType = FUNC_EXPR;
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

astnode* Parser::makeConstExpr() {
    astnode* m;
    switch (currSym()) {
        case TK_NIL: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_NIL);
        }
        break;
        case TK_STRING: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_STRING);
        }
        break;
        case TK_NUM: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_NUM);
        }
        break;
        case TK_REALNUM: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_REALNUM);
        }
        break;
        case TK_TRUE: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_TRUE);
        }
        break;
        case TK_FALSE: {
            m = makeExprNode(CONST_EXPR, current);
            match(TK_FALSE);
        }
        break;
        default:
            break;
    }
    return m;
}

astnode* Parser::makeLambdaExpr() {
    astnode* m = makeExprNode(LAMBDA_EXPR, current);
    match(currSym());
    if (currSym() == TK_ID) {
        m->attributes.strval = current.strval;
        match(TK_ID);
    }
    match(TK_LPAREN);
    if (currSym() != TK_RPAREN)
        m->child[1] = argsList();
    match(TK_RPAREN);
    if (currSym() == TK_LCURLY) {
        m->child[0] = makeBlock();
    } else if (currSym() == TK_PRODUCE) {
        match(TK_PRODUCE);
        m->child[0] = statement();
    }
    return m;
}

astnode* Parser::makeListExpr() {
    astnode* m;
    switch (currSym()) {
        case TK_LSQUARE: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_LSQUARE);
            m->child[0] = argsList();
            match(TK_RSQUARE);
        }
        break;
        case TK_APPEND: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_APPEND);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_COMMA);
            m->child[1] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_PUSH: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_PUSH);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_COMMA);
            m->child[1] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_POP: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_POP);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_LENGTH: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_LENGTH);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_EMPTY: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_EMPTY);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_SORT: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_SORT);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            if (currSym() == TK_COMMA) {
                match(TK_COMMA);
                m->child[1] = simpleExpr();
            } else m->child[1] = nullptr;
            match(TK_RPAREN);
        }
        break;
        case TK_FIRST: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_FIRST);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_REST: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_REST);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_MAP: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_MAP);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_COMMA);
            m->child[1] = simpleExpr();
            match(TK_RPAREN);
        }
        break;
        case TK_FILTER: {
            m = makeExprNode(LIST_EXPR, current);
            match(TK_FILTER);
            match(TK_LPAREN);
            m->child[0] = simpleExpr();
            match(TK_COMMA);
            m->child[1] = simpleExpr();
            match(TK_RPAREN);
        }
        default:
            break;
    }
    return m;
}