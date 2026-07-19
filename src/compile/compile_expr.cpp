#include "bcgen.hpp"

void ByteCodeGenerator::emitBinaryOperator(astnode* n) {
    if (n->token.getSymbol() == TK_ASSIGN) {
        genCode(n->right, false);
        if (n->left->token.getSymbol() == TK_LB) {
            emitListAccess(n->left, true);
        } else if (n->left->token.getSymbol() == TK_PERIOD) {
            emitFieldAccess(n->left, true);
        } else {
            emitLoad(n->left, true);
            if (noisey) cout<<"Step three: emit appropriate store instructioin"<<endl;
            emitStore(n);
        }
    } else if (n->token.getSymbol() == TK_ASSIGN_SUM || n->token.getSymbol() == TK_ASSIGN_DIFF) {
        genCode(n->left, false);
        genCode(n->right, false);
        emit(Instruction(binop, n->token.getSymbol() == TK_ASSIGN_DIFF ? VM_SUB:VM_ADD));
        genCode(n->left, true);
        emitStore(n);
    } else {
        if (noisey) cout<<"Compiling BinOp: "<<n->token.getString()<<endl;
        genCode(n->left,  false);
        genCode(n->right, false);
        switch (n->token.getSymbol()) {
            case TK_ADD:  emit(Instruction(binop, VM_ADD)); break;
            case TK_SUB:  emit(Instruction(binop, VM_SUB)); break;
            case TK_MUL:  emit(Instruction(binop, VM_MUL)); break;
            case TK_DIV:  emit(Instruction(binop, VM_DIV)); break;
            case TK_MOD:  emit(Instruction(binop, VM_MOD)); break;
            case TK_LT:   emit(Instruction(binop, VM_LT)); break;
            case TK_GT:   emit(Instruction(binop, VM_GT)); break;
            case TK_LTE:  emit(Instruction(binop, VM_LTE)); break;
            case TK_GTE:  emit(Instruction(binop, VM_GTE)); break;
            case TK_EQU:  emit(Instruction(binop, VM_EQU)); break;
            case TK_NEQ:  emit(Instruction(binop, VM_NEQ)); break;
            case TK_LOGIC_AND:  emit(Instruction(binop, VM_LOGIC_AND)); break;
            case TK_LOGIC_OR:   emit(Instruction(binop, VM_LOGIC_OR)); break;
            case TK_MATCHRE:    emit(Instruction(binop, VM_REGEX)); break;
        }
    }
}
void ByteCodeGenerator::emitUnaryOperator(astnode* n) {
    switch (n->token.getSymbol()) {
        case TK_INCREMENT: { 
            genCode(n->left, false);
            emit(Instruction(incr)); 
            genCode(n->left, true);
            emitStore(n);
        } break;
        case TK_DECREMENT: { 
            genCode(n->left, false);
            emit(Instruction(decr)); 
            genCode(n->left, true);
            emitStore(n);
        } break;
        case TK_FLOOR: {
            genCode(n->left, false);
            emit(Instruction(floorval));
        } break;
        default:
            genCode(n->left, false);
            emit(Instruction(unop, VM_NEG));
    }
}

void ByteCodeGenerator::emitTernaryExpr(astnode* n) {
    astnode* lrc = n->right;
    genCode(n->left, false);
    int L1 = skipEmit(0);
    skipEmit(1);
    genCode(lrc->left, false);
    int L2 = skipEmit(0);
    skipEmit(1);
    genCode(lrc->right, false);
    int L3 = skipEmit(0);
    skipTo(L1);
    emit(Instruction(brf, L2+1));
    skipTo(L2);
    emit(Instruction(jump, L3));
    restore();
}

void ByteCodeGenerator::emitRangeExpr(astnode* n) {
    genExpression(n->left, false);
    genExpression(n->right, false);
    emit(Instruction(mkrange));
}

void ByteCodeGenerator::emitConstant(astnode* n) {
    if (noisey) cout<<"Compiling Constant: "<<n->token.getString()<<endl;
    switch (n->token.getSymbol()) {
        case TK_NUM:    {
            int idx = symTable.getConstPool().insert(StackItem(stod(n->token.getString())));
            emit(Instruction(ldconst, StackItem(stod(n->token.getString()))));  
        } break;
        case TK_STRING: {
            int idx = symTable.getConstPool().insert(StackItem(n->token.getString()));
            emit(Instruction(ldconst, idx));  
        } break;
        case TK_RANDOM: {
            double val = n->left == nullptr ? RAND_MAX:stod(n->left->token.getString());
            emit(Instruction(ldrand, val));  
        } break;
        case TK_TRUE:   emit(Instruction(ldconst, true)); break;
        case TK_FALSE:  emit(Instruction(ldconst, false)); break;
        case TK_NIL:    emit(Instruction(ldconst));
        default: break;
    } 
}

void ByteCodeGenerator::emitListOperation(astnode* listExpr) {
    auto listname = listExpr->left;
    auto operation = listExpr->right;
    if (operation == nullptr)
        return;
    switch (operation->token.getSymbol()) {
        case TK_APPEND: {
            genExpression(listname, false);
            genExpression(operation->left, false);
            emit(Instruction(list_append));
        } break;
        case TK_PUSH: {
            genExpression(listname, false);
            genExpression(operation->left, false);
            emit(Instruction(list_push));
        } break;
        case TK_POP: {
            genExpression(listname, false);
            emit(Instruction(list_pop));
        } break;
        case TK_SIZE: {
            genExpression(listname, false);
            emit(Instruction(list_len));  
        } break;
        case TK_EMPTY: {
            genExpression(listname, false);
            emit(Instruction(list_len));  
            emit(Instruction(ldconst, symTable.getConstPool().insert(0.0)));
            emit(Instruction(binop, VM_EQU));
        } break;
    }
}

void ByteCodeGenerator::emitComprehension(astnode* n) {
    emit(Instruction(mklist));
    genExpression(n->left, false);
    //set i = 0;
    int IDX = symTable.lookup("__scitr").addr;
    emit(Instruction(ldconst, StackItem(0.0)));
    emit(Instruction(ldaddr, IDX));
    emit(Instruction(stlocal));
    int L1 = skipEmit(0);
    emit(Instruction(ldlocal, IDX));  // index of current position
    emit(Instruction(ldidx));         // get data at that index
    genExpression(n->right, false);
    emit(Instruction(call, -1, 1));
    emit(Instruction(ldlocal, IDX));
    emit(Instruction(stidx));   
    /*
    emit(Instruction(ldlocal, IDX));
    emit(Instruction(incr));
    emit(Instruction(ldaddr, IDX));
    emit(Instruction(stlocal));
    emit(Instruction(ldlocal, IDX));
    emit(Instruction(list_len));
    emit(Instruction(binop, VM_LT));
    emit(Instruction(brf, L1));
    */
}

void ByteCodeGenerator::emitLambda(astnode* n) {
    int numArgs = 0;
    for (astnode* x = n; x != nullptr; x = x->next)
        numArgs++;
    int L1 = skipEmit(0);
    skipEmit(1);
    string name = n->token.getString();
    emit(Instruction(defun, name, numArgs, 0));
    symTable.openFunctionScope(name, L1+1);
    genCode(n->right, false);
    emit(Instruction(retfun));
    int cpos = skipEmit(0);
    symTable.closeScope();
    skipTo(L1);
    emit(Instruction(jump, cpos));
    restore();
    emitStoreFuncInEnvironment(n, true);
}
void ByteCodeGenerator::emitBlessExpr(astnode* n) {
    string name = n->left->token.getString();
    int cpIdx = symTable.lookupClass(name) == nullptr ? -1:symTable.lookupClass(name)->cpIdx;
    if (cpIdx == -1) {
        return;
    }
    ClassObject* klass = symTable.lookupClass(name);

    auto it = klass->scope->iter();
    for (auto x = n->right; x != nullptr && !it.done(); x = x->next) {
        genExpression(x, false);
        it.next();
    }

    emit(Instruction(mkstruct, cpIdx, klass->scope->size()));

    it = klass->scope->iter();
    vector<int> idxs;
    for (auto x = n->right; x != nullptr && !it.done(); x = x->next) {
        if (it.get().constPoolIndex == -1)
            it.get().constPoolIndex = symTable.getConstPool().insert(it.get().name);
        idxs.push_back(it.get().constPoolIndex);
        it.next();
    }
    while (!idxs.empty()) {
        emit(Instruction(stfield, idxs.back()));
        idxs.pop_back();
    }
}
void ByteCodeGenerator::emitFunctionCall(astnode* n) {
    if (noisey) cout<<"Compiling Function Call."<<endl;
    SymbolTableEntry fn_info = symTable.lookup(n->left->token.getString());
    int argsCount = 0;
    for (auto x = n->right; x != nullptr; x = x->next)
        argsCount++;
    genCode(n->right, false);
    genExpression(n->left, false);
    emit(Instruction(call, fn_info.constPoolIndex, argsCount, n->left->token.scopeLevel()));
}
void ByteCodeGenerator::emitListConstructor(astnode* n) {
    emit(Instruction(mklist));
    if (n->left != nullptr) {
        if (n->left->expr == SETCOMP_EXPR) {
            genExpression(n->left, false);
        } else if (n->left->expr == RANGE_EXPR) {
            genExpression(n->left, false);
        } else {
            for (astnode* it = n->left; it != nullptr; it = it->next) {
                genExpression(it, false);
                emit(Instruction(list_append));
            }
        }
    }
}