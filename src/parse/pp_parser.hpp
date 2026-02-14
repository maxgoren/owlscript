#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include "ast.hpp"
#include "token.hpp"
using namespace std;

/*

    Implementation notes:
            Function Definition is implemented as syntactic sugar for a
            creating a variable definition with an assignment expression binding 
            the function body as a lambda expression to the supplied name. 
            In this way functions are unified so 
            
                fn dub(let x) { return x+x; } 
            
            will generate the _same_ instruction sequence as 
            
                let dub := &(let x) -> x+x;
            
            
*/

class Parser {
    private:    
        vector<Token> tokens;
        int tpos;
        void init(vector<Token>& tk);
        void advance();
        bool done();
        bool expect(TKSymbol symbol);
        void match(TKSymbol symbol);
        Token& current();
        TKSymbol lookahead() ;
        astnode* argsList() ;
        astnode* paramList() ;
        astnode* parseFunctionCallAndSubscripts(astnode* n);
        astnode* primary();
        astnode* unary();
        astnode* listOp();
        astnode* factor();
        astnode* term();
        astnode* relopExpr();
        astnode* compExpr();
        astnode* logicalExpr();
        astnode* assignExpr();
        astnode* expression();
        astnode* functionBody(astnode* n);
        astnode* parseIfStmt();
        astnode* parseWhileStmt();
        astnode* parseForeach();
        astnode* parseVarDec();
        astnode* parseSequence();
        astnode* parseBlock();
        astnode* parseFuncDef();
        astnode* parseClassDef();
        astnode* parsePrintStmt();
        astnode* parseImportStmt();
        astnode* parseReturn();
        astnode* statement();
        astnode* stmt_list();

        astnode* parseUnaryPrefix(int prec);
        astnode* parseListConstructor(int prec);
        astnode* parseConstExpr(int prec);
        astnode* parseExpr(int prec);
        astnode* parseFirst(int prec);
        astnode* parseRest(astnode* lhs);
        int precedence(TKSymbol sym);
        int associates(TKSymbol sym);
        bool noisey;
        bool in_list_consxr;
    public:
        Parser(bool debug);
        astnode* parse(vector<Token> tokens) ;
}; 

Parser::Parser(bool debug = false) {
    noisey = debug;
    in_list_consxr = false;
}
astnode* Parser::parse(vector<Token> tokens) {
    init(tokens);
    astnode* p = stmt_list();
    if (noisey)
        preorder(p, 1);
    return p;
}

void Parser::init(vector<Token>& tk) {
    tokens = tk;
    tpos = 0;
}
void Parser::advance() {
    tpos++;
}
bool Parser::done() {
    return lookahead() == TK_EOI;
}
bool Parser::expect(TKSymbol symbol) {
    return symbol == lookahead();
}
void Parser::match(TKSymbol symbol) {
    if (expect(symbol)) {
        advance();
        return;
    }
    cout<<"Mismatched token: "<<current().getString()<<endl;
}
Token& Parser::current() {
    return tokens[tpos];
}
TKSymbol Parser::lookahead() {
    return tokens[tpos].getSymbol();
}
astnode* Parser::argsList() {
    astnode d, *t = &d;
    while (!expect(TK_RPAREN) && !expect(TK_RB)) {
        if (expect(TK_COMMA))
            match(TK_COMMA);
        t->next = expression();
        t = t->next;
    }
    return d.next;
}
astnode* Parser::paramList() {
    astnode d, *t = &d;
    while (!expect(TK_RPAREN)) {
        if (expect(TK_COMMA))
            match(TK_COMMA);
        t->next = statement();
        t = t->next;
    }
    return d.next;
}
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
    if (expect(TK_LPAREN)) {
        match(TK_LPAREN);
        n = expression();
        match(TK_RPAREN);
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
astnode* Parser::listOp() {
    astnode* n = unary();
    if (expect(TK_RANGE)) {
        astnode* t = new astnode(RANGE_EXPR, current());
        match(TK_RANGE);
        t->left = n;
        t->right = unary();
        n = t;
    }
    if (expect(TK_LOGIC_OR) && in_list_consxr) {
        astnode* t = new astnode(SETCOMP_EXPR, current());
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

bool isBinOp(TKSymbol symbol) {
    switch (symbol) {
        case TK_EQU:  case TK_LT:  case TK_GT:
        case TK_NEQ: case TK_LTE: case TK_GTE:
        case TK_MATCHRE: case TK_AND: case TK_OR:
        case TK_ASSIGN: case TK_ASSIGN_SUM:
        case TK_ASSIGN_DIFF:  case TK_MOD:
        case TK_ADD: case TK_SUB: case TK_MUL:
        case TK_DIV:
            return true;
        default:
            break;
    }
    return false;
}

int Parser::precedence(TKSymbol sym) {
    switch (sym) {
        case TK_LAMBDA: return 10;
        case TK_ASSIGN: return 20;
        case TK_ASSIGN_SUM: return 21;
        case TK_ASSIGN_DIFF: return 21;
        case TK_AND: return 24;
        case TK_OR: return 24;
        case TK_LT: return 25;
        case TK_GT: return 25;
        case TK_EQU: return 25;
        case TK_NEQ: return 25;
        case TK_LTE: return 25;
        case TK_GTE: return 25;
        case TK_MATCHRE: return 25;
        case TK_SUB: return 50;
        case TK_ADD: return 50;
        case TK_MUL: return 60;
        case TK_DIV: return 60;
        case TK_MOD: return 60;
        case TK_RANDOM:
        case TK_NEW:
        case TK_INCREMENT:
        case TK_DECREMENT:
        case TK_PERIOD:
        case TK_LB:
        case TK_TRUE:
        case TK_FALSE:
        case TK_PUSH:
        case TK_APPEND:
        case TK_FIRST:
        case TK_REST:
        case TK_GET:
        case TK_POP:
        case TK_EMPTY:
        case TK_SIZE:
        case TK_LPAREN: return 100;
        default:
            break;
    }
    return 10;
}

astnode* Parser::parseConstExpr(int prec) {
    astnode* n = nullptr;
    if (expect(TK_NUM)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_NUM);
    } else if (expect(TK_STRING)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_STRING);
    } else if (expect(TK_TRUE)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_TRUE);
    } else if (expect(TK_FALSE)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_FALSE);
    } else if (expect(TK_NIL)) {
        n = new astnode(CONST_EXPR, current());
        match(TK_NIL);
    }
    return n;
}

astnode* Parser::parseUnaryPrefix(int prec) {
    astnode* n = nullptr;
    if (expect(TK_SUB) || expect(TK_NOT)) {
        n = new astnode(UOP_EXPR, current());
        match(lookahead());
        n->left = parseExpr(prec);
    }
    return n;
}

astnode* Parser::parseListConstructor(int prec) {
    astnode* n = nullptr;        
    if (expect(TK_LB)) {
        n = new astnode(LISTCON_EXPR, current());
        match(TK_LB);
        in_list_consxr = true;
        n->left = argsList();
        in_list_consxr = false;
        match(TK_RB);
    }
    return n;
}

astnode* Parser::parseFirst(int prec) {
    astnode* node = nullptr;
    switch (lookahead()) {
        case TK_NUM: 
        case TK_STRING:
        case TK_NIL:
        case TK_TRUE: 
        case TK_FALSE:  return parseConstExpr(prec);
        case TK_SUB:    return parseUnaryPrefix(prec);
        case TK_LB:     return parseListConstructor(prec);
        case TK_NEW: {
            node = new astnode(BLESS_EXPR, current());
            match(TK_NEW);
            node->left = new astnode(ID_EXPR, current());
            match(TK_ID);
            match(TK_LPAREN);
            node->right = argsList();
            match(TK_RPAREN);
        } break;
        case TK_LAMBDA: {
            node = new astnode(LAMBDA_EXPR, current());
            match(TK_LAMBDA);
            node = functionBody(node);
        } break;
        case TK_ID:     {
            node = new astnode(ID_EXPR, current());
            match(TK_ID);
        } break;
        case TK_RANDOM: {
            node = new astnode(CONST_EXPR, current());
            match(TK_RANDOM);
            match(TK_LPAREN);
            if (!expect(TK_RPAREN)) {
                node->left = parseExpr(prec);
            }
            match(TK_RPAREN);
        } break;
        default:
            break;
    }
    return node;
}

astnode* Parser::parseRest(astnode* lhs) {

}


astnode* Parser::parseExpr(int prec) {
    astnode* lhs = parseFirst(prec);
    while (prec < precedence(lookahead())) {
        lhs = parseRest(lhs);
    }
    return lhs;
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
    n->right = stmt_list();
    return n;
}

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

#endif