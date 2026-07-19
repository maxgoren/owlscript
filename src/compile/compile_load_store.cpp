#include "bcgen.hpp"


void ByteCodeGenerator::emitLoadAddress(SymbolTableEntry& item, astnode* n) {
    emit(Instruction(ldaddr, item.addr));
    if (noisey) cout << "LDADDR: " << n->token.getString()<<"scopelevel="<<n->token.scopeLevel() << " depth=" << item.depth<< endl;
}
void ByteCodeGenerator::emitLoad(astnode* n, bool needLvalue) {
    if (noisey) cout<<"Compiling ID expression: ";
    SymbolTableEntry item = symTable.lookup(n->token.getString());
    int depth = n->token.scopeLevel();
    if (needLvalue) {
        emitLoadAddress(item, n);
    } else {
        if (depth == GLOBAL_SCOPE) {
            emit(Instruction(ldglobal, item.addr));
            if (noisey) cout << "LDGLOBAL: " << n->token.getString()<<"scopelevel="<<n->token.scopeLevel() << " depth=" << item.depth<<" addr="<<item.addr<<endl;
        } else if (depth == 0) {
            emit(Instruction(ldlocal, item.addr));
            if (noisey) cout << "LDLOCAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<item.depth<<" addr="<<item.addr<< endl;
        } else {
            emit(Instruction(ldupval, item.addr, depth));
            if (noisey) cout<< "LDUPVAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<item.depth<<" addr="<<item.addr<< endl;
        }
    }
}
void ByteCodeGenerator::emitStore(astnode* n) {
    SymbolTableEntry item = symTable.lookup(n->left->token.getString());
    int depth = n->left->token.scopeLevel();
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
void ByteCodeGenerator::emitFieldAccess(astnode* n, bool isLvalue) {
    if (noisey) cout<<"Emitting Field access for "<<n->left->token.getString()<<"."<<n->right->token.getString()<<endl;
    emitLoad(n->left, false);
    if (n->right->expr == ID_EXPR) {
        int fieldname = symTable.getConstPool().insert(alloc.alloc(new string(n->right->token.getString())));
        emit(Instruction(isLvalue ? stfield:ldfield, fieldname));
    } else {
        auto t = n->right;
        while (t->expr == FIELD_EXPR) {
            int fieldname = symTable.getConstPool().insert(alloc.alloc(new string(t->left->token.getString())));
            emit(Instruction(ldfield, fieldname));
            if (t->right->expr == FIELD_EXPR) {
                t = t->right;
            } else {
                fieldname = symTable.getConstPool().insert(alloc.alloc(new string(t->right->token.getString())));
                emit(Instruction(isLvalue ? stfield:ldfield, fieldname));
                t = t->left;
            }
        }
    }
}

void ByteCodeGenerator::emitStoreFuncInEnvironment(astnode* n, bool isLambda) {
    string name = n->token.getString();
    SymbolTableEntry fn_info = symTable.lookup(name);
    emit(Instruction(mkclosure, fn_info.constPoolIndex, fn_info.depth));
    if (isLambda)
        return;
    if (fn_info.depth == GLOBAL_SCOPE) {
        emit(Instruction(ldaddr, fn_info.addr));
        emit(Instruction(stglobal, fn_info.addr));
    } else {
        emit(Instruction(ldaddr, fn_info.addr));
        emit(Instruction(stlocal, fn_info.addr));
    }
}