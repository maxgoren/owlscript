#ifndef compiler_hpp
#define compiler_hpp
#include <iostream>
#include <vector>
#include "../parse/lexer.hpp"
#include "../parse/parser.hpp"
#include "bcgen.hpp"
#include "compiler_state.hpp"
using namespace std;

typedef vector<Instruction> ByteCode;


class Compiler {
    private:
        Lexer lexer;
        Parser parser;
        ByteCodeGenerator codeGen;
        CompilerState state;
    public:
        Compiler(int verbosity = 0) {
            if (verbosity > 0) {
                lexer = Lexer(true);
                parser = Parser(true);
                codeGen = ByteCodeGenerator(true);
            }
            state = READY;
        }
        ConstPool& getConstPool() {
            return codeGen.getConstPool();
        }
        vector<Instruction> compile(CharBuffer* buff) {
            state = LEX;
            vector<Token> tokens = lexer.lex(buff);
            if (tokens.size() > 1) {
                state = PARSE;
                astnode* ast = parser.parse(tokens);
                if (ast) {
                    state = CODE_GEN;
                    vector<Instruction> pg = codeGen.compile(ast, state);
                    state = DONE;
                    return pg;
                }
            }
            state = ERROR;
            return {halt};
        }
        vector<Instruction> operator()(CharBuffer* buff) {
            return compile(buff);
        }
};

#endif