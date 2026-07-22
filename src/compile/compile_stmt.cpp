#include "bcgen.hpp"

void ByteCodeGenerator::emitReturn(astnode* n) {
    genCode(n->left, false);
    emit(Instruction(retfun));
}
void ByteCodeGenerator::emitPrint(astnode* n) {
    if (noisey) cout<<"Compiling Print Statement: "<<endl;
    genExpression(n->left, false); 
    emit(Instruction(print));
    if (n->token.getSymbol() == TK_PRINTLN)
        emit(Instruction(newline));
}

void ByteCodeGenerator::emitBlock(astnode* n) {
    int L1 = skipEmit(0);
    emit(Instruction(entblk, L1+2));
    symTable.openFunctionScope(n->token.getString(), L1+2);
    genCode(n->left, false);
    symTable.closeScope();
    emit(Instruction(retblk));
}

void ByteCodeGenerator::emitClassDef(astnode* n) {
    if (noisey) cout<<"Compiling class Definition: "<<n->left->token.getString()<<endl;
    int L1 = skipEmit(0);
    skipEmit(1);
    string name = n->left->token.getString();
    ClassObject* ent = symTable.lookupClass(name);
    emit(Instruction(defstruct, ent->cpIdx, L1+1));
    int cpos = skipEmit(0);
    skipTo(L1);
    emit(Instruction(jump, cpos));
    restore();
}

void ByteCodeGenerator::emitLet(astnode* n) {
    switch (n->left->expr) {
        case BIN_EXPR:{
            emitBinaryOperator(n->left); 
        } break;
        case ID_EXPR: {
            genCode(n->right, false); 
            genCode(n->left, true);
        } break;
    }  
}

void ByteCodeGenerator::emitIterator(astnode* n, int IDX, int SEQ) {
    //sets current index to 0
    emit(Instruction(ldconst, StackItem(0.0)));
    emit(Instruction(ldaddr, IDX));
    emit(Instruction(stlocal));
    //evaluate list expression and assign  the result a temp name
    //this way if passed a list constructor the list only gets built once.
    //as we use this temporary name to refer to the list moving forward.
    genExpression(n->right, false);
    emit(Instruction(ldaddr, SEQ));
    emit(Instruction(stlocal));
}

void ByteCodeGenerator::emitForeach(astnode* n) {
    //foreach loops take place in there own block scope
    emit(Instruction(entblk));
    symTable.openFunctionScope(n->token.getString(), -1);
    int IDX = symTable.lookup("itr").addr;    
    int SEQ = symTable.lookup("clti").addr;

    // set up Iterator object
    astnode* itexpr = n->left;
    emitIterator(itexpr, IDX, SEQ);

    // loop test expr: index < list.length
    int P1 = skipEmit(0);
    emit(Instruction(ldlocal, IDX)); //current index into list
    emit(Instruction(ldlocal, SEQ)); //current list to iterate
    emit(Instruction(list_len));      // obtain its length
    emit(Instruction(binop, VM_LT));  //more to go?

    //start of loop body, at the beginning of each iteration
    //we push the value at the current index of the list being iterated on to the stack
    //it's value is then stored by the name supplied by user for iterator object
    int L1 = skipEmit(0); 
    skipEmit(1);
    emit(Instruction(ldlocal, SEQ)); //current list were iterating
    emit(Instruction(ldlocal, IDX));  // index of current position
    emit(Instruction(ldidx));         // get data at that index
    emit(Instruction(ldaddr, symTable.lookup(itexpr->left->token.getString()).addr)); //address of runtime iterator
    emit(Instruction(stlocal));       //store data to iterator name
    
    //whatever code user wants to perform 
    genStatement(n->right, false);

    //get us ready for next run through
    emit(Instruction(ldlocal, IDX)); //get value of current index
    emit(Instruction(incr));        //increment it by 1
    emit(Instruction(ldaddr, IDX));  //save new index
    emit(Instruction(stlocal));
    emit(Instruction(jump, P1));  //jump back up to test expression

    //backpatch test-expression failure branch
    int L2 = skipEmit(0);
    skipTo(L1);
    emit(Instruction(brf, L2));
    restore();

    //close scope we opened for loop
    emit(Instruction(retblk));
    symTable.closeScope();
}

void ByteCodeGenerator::emitWhile(astnode* n) {
    int P1 = skipEmit(0);
    genCode(n->left, false);
    int L1 = skipEmit(0);
    skipEmit(1);
    genCode(n->right, false);
    emit(Instruction(jump, P1));
    int L2 = skipEmit(0);
    skipTo(L1);
    emit(Instruction(brf, L2));
    restore();
}

void ByteCodeGenerator::emitIfStmt(astnode* n) {
    if (n->right->token.getSymbol() == TK_ELSE) {
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
    } else {
        genCode(n->left, false);
        int L1 = skipEmit(0);
        skipEmit(1);
        genCode(n->right, false);
        int L2 = skipEmit(0);
        skipTo(L1);
        emit(Instruction(brf, L2));
        restore();
    }
}

