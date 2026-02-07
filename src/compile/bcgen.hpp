#ifndef bytecodecompiler_hpp
#define bytecodecompiler_hpp
#include <iomanip>
#include <unordered_map>
#include "../parse/ast.hpp"
#include "../vm/instruction.hpp"
#include "../vm/constpool.hpp"
#include "scopingst.hpp"
#include "stresolver.hpp"
#include "compiler_state.hpp"
using namespace std;


class  ByteCodeGenerator {
    private:
        bool noisey;
        vector<Instruction> code;
        int cpos;
        int highCI;
        ScopingST symTable;
        STBuilder sr;
        ResolveLocals rl;
        int scopeLevel();
        void emit(Instruction inst);
        int skipEmit(int numSpaces) ;
        void skipTo(int loc) ;
        void restore() ;
        void emitBinaryOperator(astnode* n) ;
        void emitUnaryOperator(astnode* n);
        void emitLoad(astnode* n, bool needLvalue);
        void emitLoadAddress(SymbolTableEntry& item, astnode* n);
        void emitStore(astnode* n);
        void emitListAccess(astnode* n, bool isLvalue) ;
        void emitFieldAccess(astnode* n, bool isLvalue) ;
        void emitListOperation(astnode* listExpr) ;
        void emitReturn(astnode* n) ;
        void emitPrint(astnode* n) ;
        void emitConstant(astnode* n) ;
        void emitLambda(astnode* n) ;
        void emitBlessExpr(astnode* n) ;
        void emitFunctionCall(astnode* n) ;
        void emitListConstructor(astnode* n);
        void emitRangeExpr(astnode* n) ;
        void emitBlock(astnode* n) ;
        void emitClassDef(astnode* n) ;
        void emitStoreFuncInEnvironment(astnode* n, bool isLambda) ;
        void emitLet(astnode* n) ;
        void emitWhile(astnode* n) ;
        void emitForeach(astnode* n);
        void emitIterator(astnode* n, int idx, int seq);
        void emitIfStmt(astnode* n) ;
        void emitTernaryExpr(astnode* n);
        void genStatement(astnode* n, bool needLvalue) ;
        void genExpression(astnode* n, bool needLvalue) ;
        void genCode(astnode* n, bool needLvalue) ;
        void printOperand(StackItem& operand) ;
        
        void printByteCode() ;
        
        void printConstPool() ;
    public:
        ByteCodeGenerator(bool debug);
        ConstPool& getConstPool() ;
        vector<Instruction> compile(astnode* n, CompilerState& cs);
};

ByteCodeGenerator::ByteCodeGenerator(bool debug = false) {
    code = vector<Instruction>(1024, Instruction(halt, 0));
    code.resize(1024);
    cpos = 0;
    highCI = 0;
    noisey = debug;
}
ConstPool& ByteCodeGenerator::getConstPool() {
    return symTable.getConstPool();
}
vector<Instruction> ByteCodeGenerator::compile(astnode* n, CompilerState& cs) {
    cs = BUILD_ST;
    sr.buildSymbolTable(n, &symTable);
    cs = RESOLVE_NAMES;
    rl.resolveLocals(n, &symTable);
    cs = CODE_GEN;
    genCode(n, false);
    if (noisey) {
        printByteCode();
        printConstPool();
    }
    return code;
}

int ByteCodeGenerator::scopeLevel() {
    return symTable.depth();
}
void ByteCodeGenerator::emit(Instruction inst) {
    if (cpos+1 == code.max_size()) {
        code.push_back(inst);
        cpos = code.size()-1;
    } else {
        code[cpos++] = inst;
    }
    if (cpos > highCI)
        highCI = cpos;
}
int ByteCodeGenerator::skipEmit(int numSpaces) {
    cpos += numSpaces;
    return cpos;
}
void ByteCodeGenerator::skipTo(int loc) {
    cpos = loc;
}
void ByteCodeGenerator::restore() {
    cpos = highCI;
}
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
            if (noisey) cout << "LDGLOBAL: " << n->token.getString()<<"scopelevel="<<n->token.scopeLevel() << " depth=" << item.depth<< endl;
        } else if (depth == 0) {
            emit(Instruction(ldlocal, item.addr));
            if (noisey) cout << "LDLOCAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<item.depth<< endl;
        } else {
            emit(Instruction(ldupval, item.addr, depth));
            if (noisey) cout<< "LDUPVAL: " << n->token.getString()<<", scopelevel= "<<n->token.scopeLevel() << " depth= " <<item.depth<< endl;
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
    emit(Instruction(mkstruct, cpIdx, klass->scope->size()));
    int i = 0;
    auto it = klass->scope->iter();
    for (auto x = n->right; x != nullptr; x = x->next) {
        genExpression(x, false);
        if (it.get().constPoolIndex == -1)
            it.get().constPoolIndex = symTable.getConstPool().insert(it.get().name);
        emit(Instruction(stfield, symTable.getConstPool().get(it.get().constPoolIndex)));
        it.next();
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
        if (n->left->expr == RANGE_EXPR) {
            genExpression(n->left, false);
        } else {
            for (astnode* it = n->left; it != nullptr; it = it->next) {
                genExpression(it, false);
                emit(Instruction(list_append));
            }
        }
    }
}
void ByteCodeGenerator::emitRangeExpr(astnode* n) {
    genExpression(n->left, false);
    genExpression(n->right, false);
    emit(Instruction(mkrange));
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

    // loop test expr
    int P1 = skipEmit(0);
    emit(Instruction(ldlocal, IDX)); //current index into list
    emit(Instruction(ldlocal, SEQ)); //current list to iterate
    emit(Instruction(list_len));                              // obtain its length
    emit(Instruction(binop, VM_LT));                          //fmore to go?

    //start of loop body, at the beginning of each iteration
    //we push the value at the current index of the list being iterated on to the stack
    //it's value is then stored by the name supplied by user for iterator object
    int L1 = skipEmit(0); 
    skipEmit(1);
    emit(Instruction(ldlocal, SEQ)); //current list were iterating
    emit(Instruction(ldlocal, IDX));  // index of current position
    emit(Instruction(ldidx));                                 // get data at that index
    emit(Instruction(ldaddr, symTable.lookup(itexpr->left->token.getString()).addr)); //address of runtime iterator
    emit(Instruction(stlocal));                                                       //store data to iterator
    
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
void ByteCodeGenerator::genStatement(astnode* n, bool needLvalue) {
    switch (n->stmt) {
        case DEF_CLASS_STMT: { emitClassDef(n); } break;
        case BLOCK_STMT:  { emitBlock(n);   } break;
        case IF_STMT:     { emitIfStmt(n);   } break;
        case LET_STMT:    { emitLet(n);      } break;
        case PRINT_STMT:  { emitPrint(n);      } break;
        case RETURN_STMT: { emitReturn(n);     } break;
        case FOREACH_STMT: { emitForeach(n); } break;
        case WHILE_STMT:  { emitWhile(n);      } break;
        case EXPR_STMT:   { genCode(n->left, false); } break;
        default: break;
    };
}
void ByteCodeGenerator::genExpression(astnode* n, bool needLvalue) {
    switch (n->expr) {
        case CONST_EXPR:     { emitConstant(n);  } break;
        case ID_EXPR:        { emitLoad(n, needLvalue); } break;
        case BIN_EXPR:       { emitBinaryOperator(n); } break;
        case UOP_EXPR:       { emitUnaryOperator(n);  } break; 
        case LAMBDA_EXPR:    { emitLambda(n);         } break;
        case FUNC_EXPR:      { emitFunctionCall(n);   } break;
        case LISTCON_EXPR:   { emitListConstructor(n); } break;
        case SUBSCRIPT_EXPR: { emitListAccess(n, needLvalue); } break;
        case FIELD_EXPR:     { emitFieldAccess(n, needLvalue); } break;
        case LIST_EXPR:      { emitListOperation(n); } break;
        case BLESS_EXPR:     { emitBlessExpr(n); } break;
        case RANGE_EXPR:     { emitRangeExpr(n); } break;
        case TERNARY_EXPR:   { emitTernaryExpr(n); } break;
        default:
            break;
    }
}
void ByteCodeGenerator::genCode(astnode* n, bool needLvalue) {
    if (n != nullptr) {
        if (n->kind == STMTNODE) {
            genStatement(n, needLvalue);
        } else {
            genExpression(n, needLvalue);
        }
        genCode(n->next, false);
    }
}
void ByteCodeGenerator::printOperand(StackItem& operand) {
    switch (operand.type) {
        case INTEGER: cout<<to_string(operand.intval); break;
        default: cout<<"."; break;
    }
}
void ByteCodeGenerator::printByteCode() {
    cout<<"Compiled Bytecode: "<<endl;
    int addr = 0;
    for (auto m : code) {
        cout<<setw(2)<<addr<<": [0x"<<setw(2)<<m.op<<" "<<instrStr[m.op]<<" ";
        printOperand(m.operand[0]);
        cout<<" ";
        printOperand(m.operand[1]);
        if (m.op == call) cout<<" "<<m.operand[2].intval;
        cout<<" ]"<<endl;
        if (m.op == halt)
            break;
        addr++;
    }
    cout<<"----------------"<<endl;
    cout<<"Symbol table: ";
    symTable.print();
    cout<<"----------------"<<endl;
}
void ByteCodeGenerator::printConstPool() {
    cout<<"Constant Pool: "<<endl;
    for (int i = 0; i < symTable.getConstPool().size(); i++) {
        cout<<i<<": {"<<symTable.getConstPool().get(i).toString()<<"}"<<endl;
    }
    cout<<"-------------------"<<endl;
}

#endif