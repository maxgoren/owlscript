#include "interpreter.hpp"

ActivationRecord::ActivationRecord() {

}

Interpreter::Interpreter() {
    recDepth = 0;
    stopProcedure = false;
    dontEval.insert({AS_LIST, AS_STRING});
    builtIns.insert({"rest", "first", "sort", "map", "length"});
}

bool Interpreter::scopeIsGlobal() {
    return callStack.empty();
}

//resolve names to addresses using innermost nesting rule.
int Interpreter::getAddress(string name) {
    int addr = 0; //address zero is never used, and is used as a control address for storing nil object.

    //if we are in a procedure call, check the procedures symbol table first.
    if (!callStack.empty() && callStack.top()->env.find(name) != callStack.top()->env.end())
        addr = callStack.top()->env[name];
    //If address is still zero, we havent found the variable yet.
    //Are we in a nested procedure? Check the outter procedures symbol table, but only one, this is not dynamic scoping.
    if (addr == 0 && callStack.size() > 1 && callStack.top()->staticLink->env.find(name) != callStack.top()->staticLink->env.end())
        addr = callStack.top()->staticLink->env[name];
    //If the address is still zero, check the global symbol table
    if (addr == 0 && st.find(name) != st.end())
        addr = st[name];
    
    return addr;
}

Object* Interpreter::getVariableValue(ASTNode* node) {
    int addr = 0;
    Object* result;
    string name = node->data.stringVal;
    addr = getAddress(name);
    if (addr > 0) {
        result = memStore.get(addr);
        say(name + " resolves to address: " + to_string(addr) + ", value: " + toString(result));
        if (result->type == AS_LIST && node->left != nullptr) {
            result = getListItem(node, result);
        }
        return result;
    }
    return makeNilObject();
}

Object* Interpreter::expression(ASTNode* node) {
    if (node == nullptr) {
        return makeNilObject();
    }
    Object* result, *tmp;
    int addr, arrIndex = 0;
    double val;
    if (node->kind != EXPRNODE) {
        cout<<"Error: found expression where expecting a statement."<<endl;
        return makeNilObject();
    }
    switch (node->type.expr) {
        case OP_EXPR:
            enter("[op expression]" + node->data.stringVal); leave();
            return eval(node);
        case UOP_EXPR:
            enter("[unary op expression]");
            val = stof(toString(expression(node->left)));
            return makeRealObject(val * -1);
        case ID_EXPR:
            enter("[id expression]");
            result = getVariableValue(node);
            if (result != nullptr) {
                leave();
                return result;
            }
            say("no variable named: " + node->data.stringVal);
            leave();
            return makeStringObject(new string("(err)"));
        case CONST_EXPR:
            enter("[const expression] " + node->data.stringVal); leave();
            if (node->data.stringVal == "true") return makeBoolObject(true);
            if (node->data.stringVal == "false") return makeBoolObject(false);
            if (node->data.stringVal == "nil") return makeNilObject();
            return makeRealObject(stof(node->data.stringVal.c_str()));
        case FUNC_EXPR:
            enter("[func_expr] " + node->data.stringVal); leave();
            return procedureCall(node);
        case LAMBDA_EXPR:
            enter("[lambda_expr]"); leave();
            return lambdaExpr(node);
        case STRINGLIT_EXPR:
            enter("[string literal expression]"); leave();
            return makeStringObject(&(node->data.stringVal));
        case CAR_EXPR: 
            enter("[car expr]"); leave();
            return carExpr(node);
        case CDR_EXPR:
            enter("[cdr expr]"); leave();
            return cdrExpr(node);
        case LIST_EXPR:
            enter("[list expression]"); leave();
            return listExpr(node);
        case LISTLEN_EXPR:
            enter("[listlen expr]"); leave();
            return listSize(node);
        case SORT_EXPR:
            enter("[sortlist_expr]"); leave();
            return sortList(node);
        case MAP_EXPR:
            enter("map_expr"); leave();
            return mapExpr(node);
        default:
            break;
    }
    leave();
    return makeRealObject(0.0f);
}

void Interpreter::statement(ASTNode* node) {
    enter("[statement]");
    switch (node->type.stmt) {
        case LET_STMT:
            letStmt(node);
            break;
        case PRINT_STMT:
            printStmt(node);
            break;
        case READ_STMT:
            readStmt(node);
            break;
        case ASSIGN_STMT:
            assignStmt(node);
            break;
        case PUSH_STMT:
            pushList(node);
            break;
        case APPEND_STMT:
            appendList(node);
            break;
        case POP_STMT:
            enter("[pop list expr]"); leave();
            popList(node);
            break;
        case DEF_STMT:
            defineFunction(node);
            break;
        case IF_STMT:
            ifStmt(node);
            break;
        case LOOP_STMT:
            loopStmt(node);
            break;
        case RETURN_STMT:
            returnStmt(node);
            break;
        default: 
            cout<<"Invalid Statement: "<<node->data.stringVal<<endl;
        break;
    }
    leave();
}

void Interpreter::run(ASTNode* node) {
    if (node == nullptr)
        return;
    switch(node->kind) {
        case STMTNODE:
            statement(node);
            break;
        case EXPRNODE:
            expression(node);
            break;
    }
    if (stopProcedure) {
        stopProcedure = false;
        return;
    } else {
        leave();
        run(node->next);
    }
}

void Interpreter::enter(string s) {
    recDepth = (recDepth > 60) ? 0:recDepth+1;
    step++;
    say(s);
}

void Interpreter::setLoud(bool l) {
    loud = l;
}

void Interpreter::say(string s) {
    if (loud) {
        for (int i = 0; i < recDepth; i++)
            cout<<"  ";
        cout<<"("<<step<<") "<<s<<endl;
    }
}

void Interpreter::leave(string s) {
    say(s);
    leave();
}

void Interpreter::leave() {
    --recDepth;
    step--;
    if (recDepth < 0) {
        resetRecDepth();
    }
    if (step < 0) step = 0;
}

void Interpreter::resetRecDepth() {
    recDepth = 0;
}

void Interpreter::memstats() {
    memStore.memstats();
}