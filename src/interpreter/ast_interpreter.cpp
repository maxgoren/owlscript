#include <cmath>
#include "ast_interpreter.hpp"
#include "../ast_builder.hpp"
using namespace std;

void saveFile(Object& m) {
    ofstream ofile(m.objval->fileObj->fname->str);
    if (ofile.is_open()) {
        ofile<<toString(m)<<flush;
        ofile.close();
    }
}

ASTInterpreter::ASTInterpreter(bool loud) {
    traceEval = loud;
    recDepth = 0;
    gc = GC(loud);
}

Context& ASTInterpreter::getContext() {
    return cxt;
}

//This is the entry point for evaluating owlscript programs.
Object ASTInterpreter::execAST(astnode* node) {
    bailout = false;
    recDepth = 0;
    Object m = exec(node);
    gc.run(cxt);
    return m;
}

Object ASTInterpreter::exec(astnode* node) {
    Object m;
    if (node != nullptr) {
        switch (node->type) {
            case STMT_NODE: 
                m = execStatement(node);
                break;
            case EXPR_NODE:
                m = evalExpression(node);
                break;
            default:
                break;
        }
        if (node->next != nullptr && !bailout)
            m = exec(node->next);
    }
    return m;
}

Object ASTInterpreter::execStatement(astnode* node) {
    enter("[statement]");
    Object m;
    switch (node->stmtType) {
        case PRINT_STMT: m = performPrintStatement(node); break;
        case WHILE_STMT: m = performWhileStatement(node); break;
        case IF_STMT: m = performIfStatement(node); break;
        case BLOCK_STMT: m = performBlockStatement(node); break;
        case DEF_STMT: m = performDefStatement(node); break;
        case FOR_STMT: m = performForStatement(node); break;
        case FOREACH_STMT: m = performForEachStatement(node); break;
        case EXPR_STMT: m = evalExpression(node->child[0]); break;
        case LET_STMT: m = performLetStatement(node); break;
        case RETURN_STMT: {
             m = evalExpression(node->child[0]);
            bailout = true;
        } break;
        case STRUCT_STMT: m = performStructDefStatement(node); break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::evalExpression(astnode* node) {
    if (node == nullptr) 
        return makeIntObject(0);
    enter("[expression]");
    Object m;
    switch (node->exprType) {
        case CONST_EXPR: m = getConstValue(node);   break;
        case ID_EXPR: m = getObjectByID(node->attributes.strval, node->attributes.depth); break;
        case BINARYOP_EXPR:
        case RELOP_EXPR: m = eval(node); break;
        case UNARYOP_EXPR: m = evalUnaryOp(node); break;
        case RANGE_EXPR: m = evalRangeExpression(node); break;
        case REG_EXPR: m = evalRegularExpr(node); break;
        case BLESS_EXPR: m = evalBlessExpression(node); break;
        case ASSIGN_EXPR: m = evalAssignmentExpression(node); break;
        case FUNC_EXPR: m = performFunctionCall(node); break;
        case LAMBDA_EXPR: m = performCreateLambda(node); break;
        case LIST_EXPR:
        case SUBSCRIPT_EXPR: m = evalListExpression(node); break;
        case LISTCOMP_EXPR: m = performListComprehension(node); break;
        case FILE_EXPR:  m = performFileOpenExpression(node->child[0]); break;
        case META_EXPR: m = performMetaExpression(node); break;
        case TERNARY_EXPR: m = evalTernaryExpression(node); break;
        default:
            break;
    }
    leave();
    return m;
}

Object ASTInterpreter::evalListExpression(astnode* node) {
    enter("List expression");
    Object m;
    if (node->exprType == SUBSCRIPT_EXPR) {
        m = evalSubscriptExpression(node);
    } else {
        switch (node->attributes.symbol) {
            case TK_LSQUARE:  m = performCreateUnNamedList(node); break;
            case TK_SHIFT:
            case TK_APPEND: m = execAppendList(node); break;
            case TK_PUSH: m = execPushList(node); break;
            case TK_POP: m = execPopList(node); break;
            case TK_UNSHIFT: m = execPopBackList(node); break;
            case TK_LENGTH: m = execLength(node); break;
            case TK_SORT: m = execSortList(node); break;
            case TK_FIRST: m = execFirst(node);  break;
            case TK_REST: m = execRest(node); break;
            case TK_MAP: m = execMap(node); break;
            case TK_FILTER: m = execFilter(node); break;
            case TK_REDUCE: m = execReduce(node); break;
            case TK_EMPTY: m = execIsEmptyList(node); break;
            default:
                break;
        }
    }
    leave();
    return m;
}

Object ASTInterpreter::performMetaExpression(astnode* node) {
    Object m;
    switch (node->attributes.symbol) {
        case TK_EVAL: m = evalMetaExpression(node); break;
        case TK_TYPEOF: m = makeStringObject(getTypeName(exec(node->child[0]))); break;
        default: 
            break;
    };
    return m;
}

Object ASTInterpreter::performCreateLambda(astnode* node) {
    enter("[create lambda]");
    LambdaObj* lm = makeLambdaObj(node->child[0], node->child[1]);
    if (!cxt.scoped.empty()) {
        for (auto frv : cxt.scoped.top()) {
            lm->freeVars = makeVarList(frv.first, frv.second, lm->freeVars);
        }
    }
    Object m = makeLambdaObject(lm);
    gc.add(m.objval);
    leave();
    return m;
}

Object ASTInterpreter::performFunctionCall(astnode* node) {
    enter("[function call]");
    string id = getNameAndScopeFromNode(node).name;
    int scope = getNameAndScopeFromNode(node).scope;
    Object lmbd = getObjectByID(id, scope);
    if (lmbd.type != AS_LAMBDA) {
        leave();
        cout<<"Error: No function '"<<id<<"' coult be found."<<endl;
        return makeNilObject();
    }
    Object m = evalFunctionExpr(getLambda(lmbd), node->child[1]);
    updateContext(id, lmbd, scope);
    leave();
    return m;
}

void ASTInterpreter::say(string s) {
    if (!traceEval)
        return;
    for (int i = 0; i < recDepth; i++)
        cout<<" ";
    cout<<s<<endl;
}

void ASTInterpreter::enter(string s) {
    ++recDepth;
    if (traceEval)
        say(s);
}

void ASTInterpreter::leave() {
    --recDepth;
}