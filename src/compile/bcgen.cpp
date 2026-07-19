#include "bcgen.hpp"

ByteCodeGenerator::ByteCodeGenerator(bool debug) {
    code = vector<Instruction>(1024, Instruction(halt, 0));
    code.resize(1024);
    cpos = 0;
    highCI = 0;
    noisey = debug;
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

void ByteCodeGenerator::genStatement(astnode* n, bool needLvalue) {
    switch (n->stmt) {
        case DEF_CLASS_STMT: { emitClassDef(n);} break;
        case BLOCK_STMT:     { emitBlock(n);   } break;
        case IF_STMT:        { emitIfStmt(n);  } break;
        case LET_STMT:       { emitLet(n);     } break;
        case PRINT_STMT:     { emitPrint(n);   } break;
        case RETURN_STMT:    { emitReturn(n);  } break;
        case FOREACH_STMT:   { emitForeach(n); } break;
        case WHILE_STMT:     { emitWhile(n);   } break;
        case EXPR_STMT:      { genCode(n->left, false); } break;
        default: break;
    };
}

void ByteCodeGenerator::genExpression(astnode* n, bool needLvalue) {
    switch (n->expr) {
        case CONST_EXPR:     { emitConstant(n);         } break;
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
        case SETCOMP_EXPR:   { emitComprehension(n); } break;
        default:
            break;
    }
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

ConstPool& ByteCodeGenerator::getConstPool() {
    return symTable.getConstPool();
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