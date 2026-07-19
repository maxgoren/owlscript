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
        void emitComprehension(astnode* n);
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
        ByteCodeGenerator(bool debug = false);
        ConstPool& getConstPool() ;
        vector<Instruction> compile(astnode* n, CompilerState& cs);
};

#endif