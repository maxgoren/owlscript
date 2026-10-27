#ifndef actions_hpp
#define actions_hpp
#include "ast.hpp"
#include <vector>
using namespace std;

astnode* mkBlock(vector<astnode*>& reducing);
astnode* mkBuiltin(vector<astnode*>& reducing);
astnode* mkCall(vector<astnode*>& reducing);
astnode* mkConst(vector<astnode*>& reducing);
astnode* mkDotted(vector<astnode*>& reducing);
astnode* mkElse(vector<astnode*>& reducing);
astnode* mkExprStmt(vector<astnode*>& reducing);
astnode* 	 mkFor(vector<astnode*>& reducing);
astnode* 	 mkFunc(vector<astnode*>& reducing);
astnode* 	 mkId(vector<astnode*>& reducing);
astnode* 	 mkIf(vector<astnode*>& reducing);
astnode* 	 mkImport(vector<astnode*>& reducing);
astnode* 	 mkInstance(vector<astnode*>& reducing);
astnode* 	 mkLambda(vector<astnode*>& reducing);
astnode* 	 mkLet(vector<astnode*>& reducing);
astnode* 	 mkList(vector<astnode*>& reducing);
astnode* 	 mkListCon(vector<astnode*>& reducing);
astnode* 	 mkListOp(vector<astnode*>& reducing);
astnode* 	 mkNum(vector<astnode*>& reducing);
astnode* 	 mkOf(vector<astnode*>& reducing); 
astnode* 	 mkPrint(vector<astnode*>& reducing);
astnode* 	 mkProg(vector<astnode*>& reducing);
astnode* 	 mkRandom(vector<astnode*>& reducing);
astnode* 	 mkRet(vector<astnode*>& reducing); 
astnode* 	 mkSetComp(vector<astnode*>& reducing);
astnode* 	 mkString(vector<astnode*>& reducing);
astnode* 	 mkStruct(vector<astnode*>& reducing);
astnode* 	 mkSubscript(vector<astnode*>& reducing);
astnode* 	 mkTern(vector<astnode*>& reducing);
astnode* 	 mkWhile(vector<astnode*>& reducing);
astnode* 	 mkbinop(vector<astnode*>& reducing);
astnode* 	 pass(vector<astnode*>& reducing);
astnode* 	 unary(vector<astnode*>& reducing);
#endif