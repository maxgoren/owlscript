#ifndef compiler_hpp
#define compiler_hpp
#include <iostream>
#include <vector>
#include "../parse/lexer.hpp"
#include "../parse/parser.hpp"
#include "bcgen.hpp"
using namespace std;

class Compiler {
    private:
        Lexer lexer;
        Parser parser;
        ByteCodeGenerator codeGen;
    public:
        Compiler(int verbosity = 0) {
            if (verbosity > 0) {
                lexer = Lexer(true);
                parser = Parser(true);
                codeGen = ByteCodeGenerator(true);
            }
        }
        ConstPool& getConstPool() {
            return codeGen.getConstPool();
        }
        vector<Instruction> compile(CharBuffer* buff) {
            return codeGen.compile(parser.parse(lexer.lex(buff)));
        }
        vector<Instruction> operator()(CharBuffer* buff) {
            return compile(buff);
        }
};

#endif