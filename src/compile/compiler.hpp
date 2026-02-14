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
        bool isImportStatement(astnode* ast) {
            return isStmtType(ast, IMPORT_STMT);
        }
        bool isIDExpr(astnode* ast) {
            return isExprType(ast, ID_EXPR);
        }
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
                    while (isImportStatement(ast)) {
                        if (isIDExpr(ast->left)) {
                            string name = ast->left->token.getString();
                            name += ".owl";
                            FileStringBuffer* fsb = new FileStringBuffer();
                            if (!fsb->readFile(name)) {
                                state = DONE;
                                return {halt};
                            }
                            compile(fsb);
                        }
                        ast = ast->next;
                    }
                    if (ast != nullptr) {
                        state = CODE_GEN;
                        vector<Instruction> pg = codeGen.compile(ast, state);
                        state = DONE;
                        return pg;
                    } else {
                        state = DONE;
                        return {halt};
                    }
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