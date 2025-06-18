#ifndef oop_ast_hpp
#define oop_ast_hpp
#include "token.hpp"

class Visitor;

class ASTNode {
    private:
        Token token;
    public:
        ASTNode(Token tok) : token(tok) { }
        Token getToken() {
            return token;
        }
        virtual void accept(Visitor* visitor) = 0;
};

//Base Expr Class
class ExpressionNode : public ASTNode {
    public:
        ExpressionNode(Token tok) : ASTNode(tok) { }
};

//Base Stmt Class
class StatementNode : public ASTNode {
    public:
        StatementNode(Token tok) : ASTNode(tok) { }
};

class AssignExpr;

class Visitor {
    public:
        virtual void visit(AssignExpr* ae) = 0;
};

class AssignExpr : public ExpressionNode {
    public:
        AssignExpr(Token tk) : ExpressionNode(tk) { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BinOpExpr : public ExpressionNode {

};


#endif