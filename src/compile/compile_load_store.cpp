#include "bcgen.hpp"

int runtimeDepth(int currDepth, int itemDepth) {
    if (itemDepth == -1 || currDepth - itemDepth < 0)
        return -1;
    return (currDepth - itemDepth);
}

void ByteCodeGenerator::emitLoadAddress(SymbolTableEntry& item, astnode* n) {
    emit(Instruction(ldaddr, item.addr));
    if (noisey) cout << "LDADDR: " << n->token.getString()<<", scopelevel="<<n->token.scopeLevel() << ", depth="<<(symTable.depth() - item.depth)<<" (raw): " << item.depth<< endl;
}

void ByteCodeGenerator::emitLoad(astnode* n, bool needLvalue) {
    if (noisey) cout<<"Compiling ID expression: ";
    SymbolTableEntry item = symTable.findReady(n->token.getString());
    int depth = runtimeDepth(symTable.depth(), item.depth);
    if (needLvalue) {
        emitLoadAddress(item, n);
    } else {
        if (depth == GLOBAL_SCOPE) {
            emit(Instruction(ldglobal, item.addr));
            if (noisey) cout << "LDGLOBAL: " << n->token.getString()<<", scopelevel="<<n->token.scopeLevel() << " depth=" << item.depth<<" addr="<<item.addr<<endl;
        } else if (depth == 0) {
            emit(Instruction(ldlocal, item.addr));
            if (noisey) cout << "LDLOCAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<(symTable.depth() - item.depth)<<" (raw): "<<item.depth<<"addr="<<item.addr<< endl;
        } else {
            emit(Instruction(ldupval, item.addr, depth));
            if (noisey) cout<< "LDUPVAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<(symTable.depth() - item.depth)<<" (raw): "<<item.depth<<" addr="<<item.addr<< endl;
        }
    }
}

void ByteCodeGenerator::emitStore(astnode* n) {
    SymbolTableEntry item = symTable.findReady(n->left->token.getString());
    int depth = runtimeDepth(symTable.depth(), item.depth);
    if (depth == GLOBAL_SCOPE) {
        emit(Instruction(stglobal, item.addr));
        if (noisey) cout << "STGLOBAL: " << n->left->token.getString()<<", scopelevel= "<<n->left->token.scopeLevel() << " depth= " <<item.depth << endl;
    } else if (depth == 0) {
        emit(Instruction(stlocal, item.addr));
        if (noisey) cout << "STLOCAL: " << n->left->token.getString()<<", scopelevel= "<<n->left->token.scopeLevel() << " depth= " << item.depth<< endl;
    } else {
        emit(Instruction(stupval, depth));
        if (noisey) cout << "STUPVAL: " << n->left->token.getString()<<", scopelevel= "<<n->left->token.scopeLevel() << " depth= " << item.depth<< endl;
    }

}

void ByteCodeGenerator::emitListAccess(astnode* n, bool isLvalue) {
    genExpression(n->left, false);
    genExpression(n->right, false);
    emit(Instruction(isLvalue ? stidx:ldidx));
}


void ByteCodeGenerator::emitMethodCall(astnode* n) {
    int fieldname = symTable.getConstPool()->insert(alloc.alloc(n->right->left->token.getString()));
    if (n->right->right != nullptr) {
        genCode(n->right->right, false);
    }
    int numArgs = 1;
    for (auto x = n->right->right; x != nullptr; x = x->next) numArgs++;
    emit(Instruction(invoke, fieldname, numArgs));
}

void ByteCodeGenerator::emitListMethod(astnode* n) {
    auto listname_node = n->right->left;
    auto operation = n->right->right;
    if (operation == nullptr)
        return;
    int listname = symTable.getConstPool()->insert(alloc.alloc(listname_node->token.getString()));
    emit(Instruction(ldfield, listname));
    switch (operation->token.getSymbol()) {
        case TK_APPEND: {
            genExpression(operation->left, false);
            emit(Instruction(list_append));
        } break;
        case TK_PUSH: {
            genExpression(operation->left, false);
            emit(Instruction(list_push));
        } break;
        case TK_POP: {
            emit(Instruction(list_pop));
        } break;
        case TK_SIZE: {
            emit(Instruction(list_len));  
        } break;
        case TK_EMPTY: {
            emit(Instruction(list_len));  
            emit(Instruction(ldconst, symTable.getConstPool()->insert(0.0)));
            emit(Instruction(binop, VM_EQU));
        } break;
    }
}

void ByteCodeGenerator::emitFieldAccess(astnode* n, bool isLvalue) {
    if (noisey) {
        cout<<"Emitting Field access on "<<n->left->token.getString()<<".";
        if (n->right->expr == FUNC_EXPR) {
            cout<<n->right->left->token.getString()<<endl;
        } else {
            cout<<n->right->token.getString()<<endl;
        }
    }
    emitLoad(n->left, false);
    if (noisey) {
        cout<<"And now for the Field Part."<<endl;
    }
    if (n->right->expr == ID_EXPR) {   
        if (noisey) {
            cout<<"Regular Field Access."<<endl;
        }
        int fieldname = symTable.getConstPool()->insert(alloc.alloc(n->right->token.getString()));
        emit(Instruction(isLvalue ? stfield:ldfield, fieldname));
    } else if (n->right->expr == FUNC_EXPR) {
        if (noisey) {
            cout<<"Field is method call"<<endl;
        }
        emitMethodCall(n);
    } else if (n->right->expr == LIST_EXPR) {
        if (noisey) {
            cout<<"Field is List expression "<<endl;
        }
        emitListMethod(n);
    } else if (n->right->expr == FIELD_EXPR) {
        auto t = n->right;
        while (t->expr == FIELD_EXPR) {
            int fieldname = symTable.getConstPool()->insert(alloc.alloc(t->left->token.getString()));
            emit(Instruction(ldfield, fieldname));
            if (t->right->expr == FIELD_EXPR) {
                t = t->right;
            } else {
                fieldname = symTable.getConstPool()->insert(alloc.alloc(t->right->token.getString()));
                emit(Instruction(isLvalue ? stfield:ldfield, fieldname));
                t = t->left;
            }
        }
    }
}

void ByteCodeGenerator::emitStoreFuncInEnvironment(astnode* n, bool isLambda) {
    string name = n->token.getString();
    SymbolTableEntry fn_info = symTable.lookup(name);
    bool inClass = symTable.currentScopeIsClass();
    emit(Instruction(mkclosure, fn_info.constPoolIndex, fn_info.depth));
    if (isLambda && !inClass)
        return;
    if (inClass) {
        emit(Instruction(ldaddr, fn_info.addr));
        emit(Instruction(stfield, fn_info.addr));
    } else {
        int depth = runtimeDepth(symTable.depth(), fn_info.depth);
        if (fn_info.depth == GLOBAL_SCOPE) {
            emit(Instruction(ldaddr, fn_info.addr));
            emit(Instruction(stglobal, fn_info.addr));
        } else {
            emit(Instruction(ldaddr, fn_info.addr));
            emit(Instruction(stlocal, fn_info.addr));
        }
    }
}