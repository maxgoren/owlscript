#include "ast.hpp"

astnode* makeExprNode(ExprType type, Token data) {
    astnode* m = new astnode;
    m->type = EXPR_NODE;
    m->exprType = type;
    m->attributes = data;
    for (int i = 0; i < 3; i++)
        m->child[i] = nullptr;
    m->next = nullptr;
    return m;
}

astnode* makeStmtNode(StmtType type, Token data) {
    astnode* m = new astnode;
    m->type = STMT_NODE;
    m->stmtType = type;
    m->attributes = data;
    for (int i = 0; i < 3; i++)
        m->child[i] = nullptr;
    m->next = nullptr;
    return m;
}

void printToken(Token m) {
    cout<<"["<<symbolAsString[m.symbol]<<", "<<m.strval<<"]"<<endl;
}
int rd = 0;
void printNode(astnode* n) {
    for (int i = 0; i < rd; i++) cout<<"  ";
    if (n->type == EXPR_NODE) {
        switch (n->exprType) {
            case ID_EXPR: cout<<"[id expr]"; break;
            case CONST_EXPR: cout<<"[const expr]"; break;
            case BINARYOP_EXPR: cout<<"[binop expr]"; break;
            case UNARYOP_EXPR: cout<<"[uop exor]"; break;
            case RELOP_EXPR: cout<<"[relop expr]"; break;
            case ASSIGN_EXPR: cout<<"[assign expr]"; break;
            case FUNC_EXPR: cout<<"[func expr]"; break;
            case LAMBDA_EXPR: cout<<"[lambda expr]"; break;
            default:
                break;
        }
    } else if (n->type == STMT_NODE) {
        switch (n->stmtType) {
            case LET_STMT: cout<<"[let statement]"; break;
            case IF_STMT: cout<<"[if statement]"; break;
            case WHILE_STMT: cout<<"[while statement]"; break;
            case PRINT_STMT: cout<<"[print statement]"; break;
            case EXPR_STMT: cout<<"[expr statment]"; break;
            case DEF_STMT:  cout<<"[def statement]"; break;
            case RETURN_STMT: cout<<"[return statement]"; break;
            default:
                break;
        }
    }
    cout<<"[Scope: "<<(n->attributes.nestLevel == -1 ? "global":"level " + to_string(n->attributes.nestLevel))<<"]";
    printToken(n->attributes);
}

void nullFunc(astnode* x) {
    ;
}


void traverse(astnode* node, void (*pre)(astnode* x), void (*post)(astnode* x)) {
    rd++;
    if (node != nullptr) {
        pre(node);
        for (int i = 0; i < 3; i++)
            traverse(node->child[i], pre, post);
        post(node);
        traverse(node->next, pre, post);
    }
    rd--;
}