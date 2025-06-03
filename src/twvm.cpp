#include "twvm.hpp"

TWVM::TWVM(bool debug) {
    loud = debug;
    bailout = false;
    depth = 0;
}

void TWVM::exec(astnode* node) {
    if (node != nullptr) {
        if (node->nk == STMT_NODE) {
            evalStmt(node);
        } else {
            evalExpr(node);
        }
        if (!bailout)
            exec(node->next);
    }
}

void TWVM::evalStmt(astnode* node) {
    enter("exec: " + stmtNodeToString(node) + " (" + node->token.strval + ")\n");
    switch (node->type.stmt) {
        case LET_STMT:      letStatement(node); break;
        case IF_STMT:       ifStatement(node); break;
        case WHILE_STMT:    whileStatement(node); break;
        case FOREACH_STMT:  foreachStatement(node); break;
        case PRINT_STMT:    printStatement(node); break;
        case EXPR_STMT:     expressionStatement(node); break;
        case FUNC_DEF_STMT: defineFunction(node); break;
        case RETURN_STMT:   returnStatement(node); break;
        case BLOCK_STMT:    blockStatement(node); break;
        case STRUCT_DEF_STMT: defineStruct(node); break;
        case BREAK_STMT:    breakStatement(node); break;
        default:
            break;
    }
    leave("");
}

void TWVM::evalExpr(astnode* node) {
    enter("eval " + exprNodeToString(node) + " (" + node->token.strval + ")\n");
    if (node != nullptr) {
        switch (node->type.expr) {
            case UNOP_EXPR:     unaryOperation(node); break;
            case RELOP_EXPR:    binaryOperation(node); break;
            case BINOP_EXPR:    binaryOperation(node); break;
            case TERNARY_EXPR:  ternaryConditional(node); break;
            case LOGIC_EXPR:    booleanOperation(node); break;
            case BITWISE_EXPR:  binaryOperation(node); break;
            case ASSIGN_EXPR:   assignExpr(node); break;
            case CONST_EXPR:    constExpr(node); break;
            case ID_EXPR:       idExpr(node); break;
            case FUNC_EXPR:     functionCall(node); break;
            case LIST_EXPR:     listExpression(node); break;
            case SUBSCRIPT_EXPR: subscriptExpression(node); break;
            case REG_EXPR:      regularExpression(node); break;
            case REF_EXPR:      referenceExpression(node); break;
            case LAMBDA_EXPR:   lambdaExpression(node); break;
            case RANGE_EXPR:    rangeExpression(node); break;
            case ZF_EXPR:       listComprehension(node); break;
            case BLESS_EXPR:    blessExpression(node); break;
            default:
                break;
        }
    }
    leave("");
}

void TWVM::listExpression(astnode* node) {
    switch (node->token.symbol) {
        case TK_LB:     makeAnonymousList(node); break;
        case TK_SIZE:   getListSize(node); break;
        case TK_EMPTY:  getListEmpty(node); break;
        case TK_APPEND: doAppendList(node); break;
        case TK_PUSH:   doPushList(node); break;
        case TK_FIRST:  getFirstListElement(node); break;
        case TK_REST:   getRestOfList(node); break;
        case TK_MAP:    doMap(node);break;
        case TK_FILTER: doFilter(node); break;
        case TK_REDUCE: doReduce(node); break;
        case TK_SORT:   doSort(node); break;
        default:
            break;
    }
}

Context& TWVM::context() {
    return cxt;
}

void TWVM::push(Object info) {
    cxt.getOperandStack().push(info);
}

Object TWVM::pop() {
    if (cxt.getOperandStack().empty()) {
        cout<<"Error: Stack Underflow."<<endl;
        return cxt.nil();
    }
    return cxt.getOperandStack().pop();
}

Object& TWVM::peek(int spaces) {
    return cxt.getOperandStack().get(cxt.getOperandStack().size()-1-spaces);
}

void TWVM::enter(string s) {
    depth++;
    say(s);
}

void TWVM::leave(string s) {
    say(s);
    depth--;
}

void TWVM::say(string s) {
    if (loud && !s.empty()) {
        for (int i = 0; i < depth; i++) cout<<" ";
        cout<<s;
    }
}