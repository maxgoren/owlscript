#include "parser.hpp"

TOKENS Parser::lookahead() {
    return current.tokenVal;
}

void Parser::nexttoken() {
    if (lexPos+1 == lexemes.size()) {
        current = Lexeme(EOFTOKEN, "<fin>", lexPos);
    } else {
        current = lexemes[++lexPos];
    }
}

void Parser::prevtoken() {
    current = lexemes[--lexPos];
}

bool Parser::match(TOKENS token) {
    if (token == current.tokenVal) {
        nexttoken();
        return true;
    }
    if (current.tokenVal != EOFTOKEN) {
        cout<<"mismatched token on line "<<current.lineNumber<<": "<<tokenString[current.tokenVal]<<", near: "<<current.stringVal<<", expected: "<<tokenString[token]<<endl;
    }
    return false;
}

Parser::Parser(bool trace) {
    loud = trace;
    recDepth = 0;
}

ASTNode* Parser::parse(vector<Lexeme>& tokens) {
    lexemes = tokens;
    current = tokens[0];
    if (tokens[0].tokenVal == EOFTOKEN)
        return nullptr;
    lexPos = 0;
    if (loud) {
        for (auto m : tokens) {
            cout<<m.lineNumber<<": "<<tokenString[m.tokenVal]<<", "<<m.stringVal<<endl;
        }
    }
    return program();
}

ASTNode* Parser::program() {
    return statementList();
}

ASTNode* Parser::statementList() {
    enter("statement list");
    ASTNode* node = statement();
    ASTNode* m = node;
    while (lookahead() != RCURLY && lookahead() != ELSE && lookahead() != EOFTOKEN) {
        ASTNode* t = statement();
        if (m == nullptr) {
            node = m = t;
            node->next = m;
        } else {
            m->next = t;
            m = t;
        }
    }
    leave();
    return node;
}

ASTNode* Parser::paramList() {
    return argsList();
}

ASTNode* Parser::argsList() {
    enter("args list");
    ASTNode* node = nullptr;
    if (lookahead() == RPAREN) {
        leave("no arguments.");
        return node;
    } 
    ASTNode d;
    ASTNode* c = &d;
    int argCount = 0;
    do {
        c->left = simpleExpr();
        c = c->left;
        argCount++;
        if (lookahead() == COMA)
            match(COMA);
    } while(lookahead() != RPAREN);
    if (lookahead() == COMA)
        match(COMA);
    node = d.left;
    leave(to_string(argCount) + " arguments.");
    return node;
}

ASTNode* Parser::ifStatement() {
    enter("if statement");
    ASTNode* node = makeStmtNode(IF_STMT, lookahead(), current.stringVal);
    match(IF);
    match(LPAREN);
    node->left = simpleExpr();
    match(RPAREN);
    say("Matched If test.");
    match(LCURLY);
    node->mid = statementList();
    if (lookahead() == RCURLY)
        match(RCURLY);
    say("Matched true statement");
    if (lookahead() == ELSE) {
        match(ELSE);
        match(LCURLY);
        say("Matching else....");
        node->right = statementList();
    }
    if (lookahead() == RCURLY)
        match(RCURLY);
    leave();
    return node;
}

ASTNode* Parser::loopStatement() {
    enter("loop");
    ASTNode* node = makeStmtNode(LOOP_STMT, lookahead(), current.stringVal);
    match (LOOP);
    match(LPAREN);
    node->left = simpleExpr();
    match(RPAREN);
    match(LCURLY);
    node->right = program();
    match(RCURLY);
    leave();
    return node;
}

ASTNode* Parser::listStatement() {
    enter("list statement");
    ASTNode* node = nullptr;
    if (lookahead() == PUSH) {
        node = makeStmtNode(PUSH_STMT, lookahead(), current.stringVal);
        match(PUSH);
        match(LPAREN);
        node->left = simpleExpr();
        match(COMA);
        node->right = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("push");
        return node;
    }
    if (lookahead() == APPEND) {
        node = makeStmtNode(APPEND_STMT, lookahead(), current.stringVal);
        match(APPEND);
        match(LPAREN);
        node->left = simpleExpr();
        match(COMA);
        node->right = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("append");
        return node;
    }
    if (lookahead() == POP) {
        ASTNode* node = makeStmtNode(POP_STMT, lookahead(), current.stringVal);
        match(POP);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("pop");
        return node;
    }
    leave();
    return nullptr;
}

ASTNode* Parser::printStatement() {
    enter("print");
    ASTNode* node = makeStmtNode(PRINT_STMT, lookahead(), current.stringVal);
    match(PRINT);
    node->left = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::defStatement() {
    enter("procedure definition");
    ASTNode* node = makeStmtNode(DEF_STMT, lookahead(), current.stringVal);
    match(DEF);
    node->data.stringVal = current.stringVal;
    match(ID);
    match(LPAREN);
    node->left = paramList();
    match(RPAREN);
    match(LCURLY);
    node->right = program();
    match(RCURLY);
    leave();
    return node;
}

ASTNode* Parser::idStatement() {
    return var();
}


ASTNode* Parser::returnStatement() {
    enter("return statement");
    ASTNode*  node = makeStmtNode(RETURN_STMT, lookahead(), current.stringVal);
    match(RETURN);
    node->left = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::readStatement() {
    enter("read statement");
    ASTNode* node = makeStmtNode(READ_STMT, lookahead(), current.stringVal);
    match(READ);
    node->left = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::exprStatement() {
    enter("expr statement");
    ASTNode* node = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::letStatement() {
    enter("let statement");
    ASTNode* node = makeStmtNode(LET_STMT, lookahead(), current.stringVal);
    match(LET);
    node->left = simpleExpr();
    if (lookahead() == SEMI) 
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::statement() {
    switch (lookahead()) {
        case LET:
            return letStatement();
        case PRINT: 
            return printStatement();
        case READ:
            return readStatement();
        case IF: 
            return ifStatement();
        case LOOP:
            return loopStatement();
        case DEF: 
            return defStatement();
        case ID: 
            say("statement: id");
            return idStatement();
        case SEMI: match(SEMI);
                   break;
        case NUMBER:
        case LPAREN: 
            say("statement: lparen");
            return exprStatement();
        case PUSH:
        case APPEND:
        case POP: 
            return listStatement();
        case RETURN: 
            return returnStatement();
    default:
        ASTNode* node = simpleExpr();
        if (node == nullptr) {
            cout<<"Unknown Token on Line: "<< current.lineNumber<<": "<<current.stringVal<<endl;
            nexttoken();
        } else {
            return node;
        }

    }
    return nullptr;
}

ASTNode* Parser::simpleExpr() {
    enter("simple expr");
    ASTNode* node = expression();
    if (lookahead() == EQUAL || lookahead() == LESS || lookahead() == NOTEQUAL || 
        lookahead() == GREATER || lookahead() == LTE || lookahead() == GTE) {
        ASTNode* t = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        t->left = node;
        match(lookahead());
        node = t;
        node->right = expression();
    }
    leave();
    return node;
}

ASTNode* Parser::expression() {
    enter("expression");
    ASTNode* node = term();
    while (lookahead() == PLUS || lookahead() == MINUS) {
        ASTNode* expNode = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        expNode->left = node;
        node = expNode;
        match(lookahead());
        node->right = term();
    }
    leave();
    return node;
}

ASTNode* Parser::term() {
    enter("term");
    ASTNode* node = factor();
    while (lookahead() == MULTIPLY || lookahead() == DIVIDE || lookahead() == MOD) {
        ASTNode* expNode = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        expNode->left = node;
        node = expNode;
        match(lookahead());
        node->right = factor();
    }
    leave();
    return node;
}

ASTNode* Parser::factor() {
    enter("factor");
    ASTNode* node = primary();
    while (lookahead() == POW || lookahead() == SQRT /*|| lookahead() == MINUS*/) {
        ASTNode* expNode = makeExprNode(/*lookahead() == MINUS ? UOP_EXPR:*/OP_EXPR, lookahead(), current.stringVal);
        if (node != nullptr)
            expNode->left = node;
        node = expNode;
        auto matched = lookahead();
        match(lookahead());
        if (matched != SQRT && matched != MINUS)
            node->right = primary();
        else { 
            node->left = primary();
        }
    }
    leave();
    return node;
}

ASTNode* Parser::primary() {
    ASTNode* node;
    enter("primary");
    if (lookahead() == NUMBER) {
        node = makeExprNode(CONST_EXPR, lookahead(), current.stringVal);
        match(NUMBER);
        leave("number");
        return node;
    }
    if (lookahead() == NIL) {
        node = makeExprNode(CONST_EXPR, lookahead(), current.stringVal);
        match(NIL);
        leave("nil");
        return node;
    }
    if (lookahead() == TRUE) {
        node = makeExprNode(CONST_EXPR, lookahead(), current.stringVal);
        match(TRUE);
        leave("true");
        return node;
    }
    if (lookahead() == FALSE) {
        node = makeExprNode(CONST_EXPR, lookahead(), current.stringVal);
        match(FALSE);
        leave("false");
        return node;
    }
    if (lookahead() == QUOTE) {
        match(QUOTE);
        node = makeExprNode(STRINGLIT_EXPR, lookahead(), current.stringVal);
        match(STRING);
        match(QUOTE);
        leave("string");
        return node;
    }
    if (lookahead() == ID) {
        return var();
    }
    if (lookahead() == LPAREN) {
        match(LPAREN);
        node = simpleExpr();
        match(RPAREN);
    }
    if (lookahead() == LAMBDA) {
        leave("lambda");
        return lambdaExpr();
    }
    if (lookahead() == LSQ || lookahead() == LENGTH || lookahead() == SORT ||
        lookahead() == POP || lookahead() == MAP || lookahead() == FIRST || lookahead() == REST)
        return listExpr();
    leave();
    return node;
}

ASTNode* Parser::var() {
    enter("var");
    ASTNode* node = nullptr;
    if (lookahead() == ID) {
        node = makeExprNode(ID_EXPR, lookahead(), current.stringVal);
        say("id: " + node->data.stringVal);
        match(ID);
        if (lookahead() == LSQ) {
            say("list access.");
            match(LSQ);
            node->left = simpleExpr();
            match(RSQ);
            leave();
        }
        if (lookahead() == ASSIGN) {
            ASTNode* t = makeStmtNode(ASSIGN_STMT, lookahead(), current.stringVal);
            t->left = node;
            node = t;
            match(ASSIGN);
            node->right = simpleExpr();
            if (lookahead() == SEMI)
                match(SEMI);
            leave();
            return node;
        }
        if (lookahead() == LPAREN) {
            match(LPAREN);
            say("procedure call");
            node->type.expr = FUNC_EXPR;
            node->left = argsList();
            match(RPAREN);
            if (lookahead() == SEMI)
                match(SEMI);
            leave();
            return node;
        }
        return node;
    }
    return node;
}

ASTNode* Parser::listExpr() {
    if (lookahead() == LSQ) {
        ASTNode* node = makeExprNode(LIST_EXPR, lookahead(), current.stringVal);
        match(LSQ);
        if (lookahead() == RSQ) {
            match(RSQ);
            return node;
        } else {
            ASTNode d;
            ASTNode* c = &d;
            do {
                c->left = expression();
                c = c->left;
                if (lookahead() == COMA)
                    match(COMA);
            } while(lookahead() != RSQ);
            match(RSQ);
            node->left = d.left;
            return node;
        }
    }
    if (lookahead() == MAP) {
        ASTNode* node = makeExprNode(MAP_EXPR, lookahead(), current.stringVal);
        match(MAP);
        match(LPAREN);
        node->left = simpleExpr();
        match(COMA);
        node->right = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    if (lookahead() == LENGTH) {
        ASTNode* node = makeExprNode(LISTLEN_EXPR, lookahead(), current.stringVal);
        match(LENGTH);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    if (lookahead() == FIRST) {
        ASTNode* node = makeExprNode(CAR_EXPR, lookahead(), current.stringVal);
        match(FIRST);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    if (lookahead() == REST) {
        ASTNode* node = makeExprNode(CDR_EXPR, lookahead(), current.stringVal);
        match(REST);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    if (lookahead() == SORT) {
        ASTNode* node = makeExprNode(SORT_EXPR, lookahead(), current.stringVal);
        match(SORT);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    return nullptr;
}

ASTNode* Parser::lambdaExpr() {
    enter("lambda");
    ASTNode* node = makeExprNode(LAMBDA_EXPR, lookahead(), current.stringVal);
    match(LAMBDA);
    match(LPAREN);
    if (lookahead() != RPAREN)
        node->left = simpleExpr();
    match(RPAREN);
    match(LCURLY);
    node->right = statementList();
    if (lookahead() == RCURLY)
        match(RCURLY);
    leave();
    return node;
}

void Parser::enter(string s) {
    recDepth++;
    say(s);
}

void Parser::say(string s) {
    if (loud) {
        for (int i = 0; i < recDepth; i++)
            cout<<"  ";
        cout<<"("<<recDepth<<") "<<s<<endl;
    }
}

void Parser::leave(string s) {
    say(s);
    recDepth--;
}

void Parser::leave() {
    --recDepth;
}