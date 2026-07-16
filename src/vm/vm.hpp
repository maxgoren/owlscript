#ifndef vm_hpp
#define vm_hpp
#include "regex/subset_match.hpp"
#include "gc.hpp"
using namespace std;

static const int BLOCK_CPIDX = -420;
static const int MAX_OP_STACK = 1337;

class VM {
    private:
        static const int GLOBAL_SCOPE = -1;
        static const int LOCAL_SCOPE = 0;
        friend class GarbageCollector;
        bool running = false;
        int verbLev;
        Instruction haltSentinel;
        vector<Instruction> codePage;
        int ip;
        int sp;
        ConstPool constPool;
        GarbageCollector collector;
        ActivationRecord* callstk;
        ActivationRecord* globals;
        StackItem opstk[MAX_OP_STACK];
        ActivationRecord* walkChain(int d);
        StackItem& top(int depth = 0);
        ActivationRecord* mostRecentAR(int func_id);
        void closeOver(Instruction& inst);
        void openBlock(Instruction& inst);
        void closeBlock();
        void callProcedure(Instruction& inst) ;
        void retProcedure() ;
        void instantiate(Instruction& inst);
        void storeGlobal() ;
        void loadGlobal(Instruction& inst);
        void loadLocal(Instruction& inst);
        void loadUpval(Instruction& inst);
        void storeLocal(Instruction& inst);
        void storeUpval(Instruction& inst) ;
        void makeList(Instruction& inst);
        void loadIndexed(Instruction& inst);
        void storeIndexed(Instruction& inst);
        void loadField(Instruction& inst);
        void storeField(Instruction& inst);
        void loadConst(Instruction& inst);
        void loadAddress(Instruction& inst);
        void randNumber(Instruction& inst);
        void branchOnFalse(Instruction& inst);
        void uncondBranch(Instruction& inst);
        void appendList();
        void pushList();
        void popList();
        void listLength();
        void makeRange();
        void duplicateTop();
        void haltvm();
        void printTopOfStack();
        void unaryOperation(Instruction& inst);
        void binaryOperation(Instruction& inst);
        void relationOperation(Instruction& inst);
        void arithmeticOperation(Instruction& inst);
         
        void execute(Instruction& inst);
        Instruction& fetch();
        void printInstruction(Instruction& inst);
        void printOperandStack() ;
        void printCallStack();
        void init(vector<Instruction>& cp, int verbosity);
    public:
        VM();
        ~VM();
        void setConstPool(ConstPool& cp);
        void run(vector<Instruction>& cp, int verbosity);
};



#endif