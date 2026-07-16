#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include "ast.hpp"
#include "token.hpp"
using namespace std;

/*

    Implementation notes:
            Function Definition is implemented as syntactic sugar for a
            creating a variable definition with an assignment expression binding 
            the function body as a lambda expression to the supplied name. 
            In this way functions are unified so 
            
                fn dub(let x) { return x+x; } 
            
            will generate the _same_ instruction sequence as 
            
                let dub := &(let x) -> x+x;
            
            
*/

class Parser {
    private:    
        vector<Token> tokens;
        int tpos;
        void init(vector<Token>& tk);
        void advance();
        bool done();
        bool expect(TKSymbol symbol);
        void match(TKSymbol symbol);
        Token& current();
        TKSymbol lookahead() ;
        astnode* argsList() ;
        astnode* paramList() ;
        astnode* parseFunctionCallAndSubscripts(astnode* n);
        astnode* primary();
        astnode* unary();
        astnode* listOp();
        astnode* factor();
        astnode* term();
        astnode* relopExpr();
        astnode* compExpr();
        astnode* logicalExpr();
        astnode* assignExpr();
        astnode* expression();
        astnode* functionBody(astnode* n);
        astnode* parseIfStmt();
        astnode* parseWhileStmt();
        astnode* parseForeach();
        astnode* parseVarDec();
        astnode* parseSequence();
        astnode* parseBlock();
        astnode* parseFuncDef();
        astnode* parseClassDef();
        astnode* parsePrintStmt();
        astnode* parseImportStmt();
        astnode* parseReturn();
        astnode* statement();
        astnode* stmt_list();
        bool noisey;
        bool in_list_consxr;
    public:
        Parser(bool debug = false);
        astnode* parse(vector<Token> tokens) ;
}; 

#endif