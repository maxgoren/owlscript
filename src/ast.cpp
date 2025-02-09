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

bool isExprNode(astnode* node) {
    return node != nullptr && node->type == EXPR_NODE;
}
bool isStmtNode(astnode* node) {
    return node != nullptr && node->type == STMT_NODE;
}

bool isExprType(astnode* node, ExprType type) {
    return isExprNode(node) && node->exprType == type;
}
bool isStmtType(astnode* node, StmtType type) {
    return isStmtNode(node) && node->stmtType == type;
}

Token& getAttributes(astnode* node) {
    return node->attributes;
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
            case REG_EXPR: cout<<"[regular expression]"; break;
            case REF_EXPR: cout<<"[reference expression]"; break;
            case SUBSCRIPT_EXPR: cout<<"[subscript expression]"; break;
            case LISTCOMP_EXPR: cout<<"[list comprehension expression]"; break;
            case RANGE_EXPR:    cout<<"[range expression]"; break;
            case LIST_EXPR:     cout<<"[list expression]"; break;
            case FILE_EXPR:     cout<<"[file expression]"; break;
            case META_EXPR:     cout<<"[meta expression]"; break;
            case OBJECT_DOT_EXPR: cout<<"[object dot expr]"; break;
            default:
                break;
        }
    } else if (n->type == STMT_NODE) {
        switch (n->stmtType) {
            case LET_STMT: cout<<"[let statement]"; break;
            case IF_STMT: cout<<"[if statement]"; break;
            case WHILE_STMT: cout<<"[while statement]"; break;
            case FOR_STMT: cout<<"[for statement]"; break;
            case FOREACH_STMT: cout<<"[foreach statement]"; break;
            case PRINT_STMT: cout<<"[print statement]"; break;
            case EXPR_STMT: cout<<"[expr statment]"; break;
            case DEF_STMT:  cout<<"[def statement]"; break;
            case RETURN_STMT: cout<<"[return statement]"; break;
            case REF_STMT: cout<<"[reference statement]"; break;
            case STRUCT_STMT: cout<<"[struct def statement]"; break;
            case BLOCK_STMT:  cout<<"[Open New Scope]"; break;
            default:
                break;
        }
    }
    cout<<"[Scope: "<<(n->attributes.depth == -1 ? "global":"level " + to_string(n->attributes.depth))<<"]";
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

void cleanup(astnode* x) {
    if (x != nullptr) {
        for (int i = 0; i < 3; i++)
            cleanup(x->child[i]);
        cleanup(x->next);
        delete x;
    }
}