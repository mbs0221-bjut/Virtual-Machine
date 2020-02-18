#include "lexer.h"

#ifndef __INTER_H_
#define __INTER_H_

#include <list>
#include <stack>

class Visitor;

class ASTNode{
	friend class Visitor;
public:
	virtual void Codegen(FILE *fp) = 0;
};

//Óï¾ä
class Stmt : ASTNode{
public:
	int line;
	int begin, next;
	static int label;
	static int newlabel(){
		return label++;
	}
	virtual void Codegen(FILE *fp);
};

int Stmt::label = 0;

//Óï¾ä¿é
class Block : public Stmt{
	vector<Stmt*> block;
public:
	Block(vector<Stmt*> &block) : block(block) {  }
	virtual void Codegen(FILE *fp);
};

//±í´ïÊ½
class ExprAST : public Stmt{
public:
	Type *type;
	int label;
	static int count;
	ExprAST() { label = count++; }
	virtual void Codegen(FILE *fp);
};

int ExprAST::count = 0;

class BinaryExprAST : public ExprAST {
	int opt;
	ExprAST *pL, *pR;
public:
	BinaryExprAST(int opt, ExprAST *pL, ExprAST *pR) 
		 : opt(opt), pL(pL), pR(pR) { }
	virtual void Codegen(FILE *fp);
};

class UnaryExprAST : public ExprAST{
	int opt;
	ExprAST *E1;
public:
	UnaryExprAST(int opt, ExprAST *E1)
		: opt(opt), E1(E1){ }
	virtual void Codegen(FILE *fp);
};

class VariableExprAST : public ExprAST {
	string name;
public:
	VariableExprAST(const string &name) : name(name){ }
	void Codegen(FILE *fp);
};

class AssignExprAST : public VariableExprAST {
	ExprAST *expr;
public:
	AssignExprAST(const string &name, ExprAST *expr)
		: VariableExprAST(name), expr(expr) {  }
	void Codegen(FILE *fp);
};

class ConstantExprAST : public ExprAST {
	Integer *s;
public:
	ConstantExprAST(Integer *s) : s(s) { }
	void Codegen(FILE *fp);
};

class AccessExprAST : public VariableExprAST {
	ExprAST *loc;
public:
	AccessExprAST(const string &name, ExprAST *loc)
		: VariableExprAST(name), loc(loc) {  }
};

//class MemberExpr : public ExprAST {
//	string name;
//	ExprAST *index;
//public:
//	MemberExpr(const string name, ExprAST *index)
//		: name(name), index(index) { }
//};
//
//class PointerExpr : public ExprAST {
//	string name;
//	ExprAST *index;
//public:
//	PointerExpr(const string name, ExprAST *index)
//		: name(name), index(index) { }
//};

class CallExprAST : public ExprAST {
	string callee;
	vector<ExprAST*> args;
public:
	CallExprAST() { args.clear(); }
	CallExprAST(const string &callee, vector<ExprAST*> &args)
		 : callee(callee), args(args) { ; }
	void Codegen(FILE *fp);
};

class Decl : Stmt{
	list<VariableExprAST*> ids;
public:
	void Codegen(FILE *fp);
};

class IfElse : public Stmt{
	ExprAST *cond;
	Stmt *body_t;
	Stmt *body_f;
public:
	IfElse(ExprAST *cond, Stmt *body_t, Stmt *body_f)
		: cond(cond), body_t(body_t), body_f(body_f) { }
	void Codegen(FILE *fp);
};

class WhileDo : public Stmt{
	ExprAST *cond;
	Stmt *body;
public:
	WhileDo(ExprAST *cond, Stmt *body) :cond(cond), body(body) {  }
	void Codegen(FILE *fp);
};

class DoWhile : public Stmt{
	ExprAST *cond;
	Stmt *body;
public:
	DoWhile(ExprAST *cond, Stmt *body) :cond(cond), body(body) {  }
	void Codegen(FILE *fp);
};

class For : public Stmt{
	ExprAST *init, *cond, *step;
	Stmt *body;
public:
	For(ExprAST *init, ExprAST *cond, ExprAST *step, Stmt *body) 
		: init(init), cond(cond), step(step), body(body) {  }
	void Codegen(FILE *fp);
};

typedef pair<int, Stmt*> Case;

class Switch : public Stmt {
	ExprAST *expr;
	vector<Case> cases;
public:
	Switch(ExprAST *expr, vector<pair<int, Stmt*>> &cases)
		: expr(expr), cases(cases) {  }
	void Codegen(FILE *fp);
};

class Break : public Stmt{
	Stmt *stmt;
public:
	Break(Stmt *next) : stmt(next) { }
	void Codegen(FILE *fp);
};

class Continue : public Stmt{
	Stmt *next;
public:
	Continue(Stmt *next) : next(next) { }
	void Codegen(FILE *fp);
};

class Throw : public Stmt {
	Type* exception;
public:
	Throw() {
		exception = nullptr;
	}
	void Codegen(FILE *fp);
};

class TryCatch : public Stmt{
	Stmt *pTry, *pCatch, *pFinally;
public:
	TryCatch(Stmt* pTry, Stmt* pCatch, Stmt* pFinally) 
		 : pTry(pTry), pCatch(pCatch), pFinally(pFinally) { }
	void Codegen(FILE *fp);
};

// -------------·ûºÅ±í-----------
typedef map<string, Type*> SymbolTable;

//class Symbols{
//public:
//	int id;
//	static int count;
//	list<Id*> ids;
//	Symbols *prev, *next;
//	Symbols(){
//		id = count++;
//		ids.clear();
//		prev = nullptr;
//		next = nullptr;
//	}
//};
//
//int Symbols::count = 0;

// -------------º¯Êý-----------
class ParameterAST {
	Type *type;
	string name;
public:
	ParameterAST(Type *type, string name) : type(type), name(name) { }
};

class PrototypeAST {
	Type *type;
	string name;
	vector<ParameterAST*> args;
public:
	PrototypeAST(Type *type, const string &name, vector<ParameterAST*> &args)
		: type(type), name(name), args(args) { }
	void Codegen(FILE * fp);
};

class FunctionAST {
	PrototypeAST *proto;
	Stmt *body;
public:
	FunctionAST(PrototypeAST *proto, Stmt *body) : proto(proto), body(body) { }
	void Codegen(FILE * fp);
};

class Global : ASTNode {
	map<string, Type*> symbols;
public:
	void putId(string name, Type* type) {
		symbols[name] = type;
	}
	Type* getId(string name) {
		return symbols[name];
	}
	virtual void code(FILE *fp){
		ASTNode::Codegen(fp);
	}
};

#endif // !__INTER_H_
