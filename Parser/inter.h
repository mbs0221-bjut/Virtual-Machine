#include "lexer.h"

#ifndef __INTER_H_
#define __INTER_H_

#include <list>
#include <stack>

class Visitor;

class ASTNode{
	friend class Visitor;
public:
	virtual void code(FILE *fp){

	}
};

class Nodes :ASTNode{
	list<ASTNode*> nodes;
public:
	virtual void code(FILE *fp){
		list<ASTNode*>::iterator iter;
		for (iter = nodes.begin(); iter != nodes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

//语句
class Stmt :ASTNode{
	int line;
	int begin, next;
	static int label;
public:
	static int newlabel(){
		return label++;
	}
	virtual void code(FILE *fp){
		ASTNode::code(fp);
		printf("[%04d]", line);
	}
};

int Stmt::label = 0;

//语句块
class Stmts :Stmt{
	list<Stmt*> Ss;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("stmts\n");
		list<Stmt*>::iterator iter;
		for (iter = Ss.begin(); iter != Ss.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

//表达式
class ExprAST : Stmt{
	int label;
	static int count;
public:
	ExprAST() { label = count++; }
	virtual void code(FILE *fp){
		ASTNode::code(fp);
	}
};

int ExprAST::count = 0;

// 双目表达式
class BinaryExpr : ExprAST {
	Word opt;
	ExprAST *pL, *pR;
public:
	BinaryExpr(Word &opt, ExprAST *pL, ExprAST *pR) : opt(opt), pL(pL), pR(pR) { ; }
	virtual void code(FILE *fp) override {
		ExprAST::code(fp);
	}
};

// 单目表达式
class UnaryExpr :ExprAST{
	Word opt;
	ExprAST *E1;
public:
	UnaryExpr(Word &opt, ExprAST *E1) :opt(opt), E1(E1){  }
	virtual void code(FILE *fp){
		ExprAST::code(fp);
	}
};

// ID
class Id :ExprAST{
	Type *type;
	Word *name;
	int offset;
	bool global;
public:
	Id(Type *type, Word *name) : type(type), name(name){  }
	const char* getName() { return name->word.c_str(); }
	const char* getTypeName() { return type->word.c_str(); }
	int getWidth() { return type->width; }
	virtual void code(FILE *fp){
		ExprAST::code(fp);
		char *width = type == Type::Int ? "dw" : "b";
		if (global){
			fprintf(fp, "\tload%s $%d %s\n", width, label, name->getName());
		}else{
			fprintf(fp, "\tload%s $%d %s\n", width, label, name->getName());
		}
	}
};

class Constant : ExprAST{
	Integer *s;
public:
	Constant(Integer *s) : s(s){  }
	virtual void code(FILE *fp){
		ExprAST::code(fp);
		char width = s->value > 256 ? 'w' : 'b';
		fprintf(fp, "\tload%c $%d %d\n", width, label, s->value);
	}
};

class Decl :Stmt{
	list<Id*> ids;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("data\n");
		fprintf(fp, "\tdata\n");
		list<Id*>::iterator iter;
		for (iter = ids.begin(); iter != ids.end(); iter++){
			printf("\t$%s [%d]\n", (*iter)->getName(), (*iter)->getWidth());
			fprintf(fp, "\t\t$%s [%d]\n", (*iter)->getName(), (*iter)->getWidth());
		}
		printf("data\n");
		fprintf(fp, "\tdata\n");
	}
};

class Assign :Stmt{
	Id *id;
	ExprAST *expr;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("assign\n");
		expr->code(fp);
		if (id->global){
			fprintf(fp, "\tstore %s $%d\n", id->getName(), expr->label);
		}else{
			fprintf(fp, "\tstore %s $%d\n", id->getName(), expr->label);
		}
	}
};

class If :Stmt{
	ExprAST *cond;
	Stmt *body;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("if\n");
		next = newlabel();
		body->next = next;
		// 计算表达式
		cond->code(fp);
		// 为False跳转
		fprintf(fp, "jz L%d", next);// False跳转
		body->code(fp);
		fprintf(fp, "L%d:\n", next);
	}
};

class IfElse :Stmt{
	ExprAST *cond;
	Stmt *body_t;
	Stmt *body_f;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("if-else\n");
		body_t->next = newlabel();
		body_f->next = newlabel();
		// 计算表达式
		cond->code(fp);
		// 为False跳转
		fprintf(fp, "jz L%d:\n", next);// False跳转
		body_t->code(fp);
		fprintf(fp, "\tjmp L%d\n", next);
		fprintf(fp, "L%d:\n", next);
		body_f->code(fp);
		fprintf(fp, "L%d:\n", body_t->next);
	}
};

class While :Stmt{
	ExprAST *cond;
	Stmt *body;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("while\n");
		begin = newlabel();
		next = newlabel();
		body->next = begin;
		fprintf(fp, "L%d:\n", begin);
		cond->code(fp);
		fprintf(fp, "jz L:\n", next);// False跳转
		body->code(fp);
		fprintf(fp, "\tjmp L%d\n", begin);
		fprintf(fp, "L%d:\n", next);
	}
};

class Do :Stmt{
	ExprAST *cond;
	Stmt *body;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("do-while\n");
		begin = newlabel();
		body->next = begin;
		fprintf(fp, "L%d:\n", begin);
		body->code(fp);
		cond->code(fp);
		fprintf(fp, "jnz L%d:\n", begin);// True跳转
	}
};

class For : Stmt{
	Stmt *init;
	ExprAST *cond;
	Stmt *step;
	Stmt *body;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("for\n");
		begin = newlabel();
		next = newlabel();
		step->begin = newlabel();
		step->next = newlabel();
		body->next = begin;
		init->code(fp);
		fprintf(fp, "L%d:\n", begin);
		cond->code(fp);
		fprintf(fp, "L%d:\n", cond->label); // False跳转
		body->code(fp);
		fprintf(fp, "L%d:\n", step->begin);
		step->code(fp);
		fprintf(fp, "\tjmp L%d\n", begin);
		fprintf(fp, "L%d:\n", next);
	}
};

class Case :Stmt{
	ExprAST *expr;
	map<int, Stmt*> Ss;
public:
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("case\n");
		fprintf(fp, "jmp L%d\n", begin);
		map<int, Stmt*>::iterator iter;
		for (iter = Ss.begin(); iter != Ss.end(); iter++){
			fprintf(fp, "L%d:\n", iter->second->begin);
			iter->second->code(fp);
		}
		fprintf(fp, "L%d:\n", begin);
		expr->code(fp);
		for (iter = Ss.begin(); iter != Ss.end(); iter++){
			fprintf(fp, "L%d:\n", iter->second->begin);
			fprintf(fp, "= $%d $%d #%d\n", expr->label, iter->first);
			iter->second->code(fp);
		}
	}
};

class Break :Stmt{
	Stmt *stmt;
	static stack<Stmt*> cur;
public:
	Break(){
		stmt = Break::cur.top();
	}
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("break\n", stmt->next);
		fprintf(fp, "\tjmp L%d\n", stmt->next);
	}
};

stack<Stmt*> Break::cur = stack<Stmt*>();

class Continue :Stmt{
	Stmt *stmt;
	static stack<Stmt*> cur;
public:
	Continue(){
		stmt = Continue::cur.top();
	}
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("continue\n", stmt->begin);
		fprintf(fp, "\tjmp L%d\n", stmt->begin);
	}
};

stack<Stmt*> Continue::cur = stack<Stmt*>();

// 异常处理程序
class Throw : Stmt {
	Type* exception;// 抛出异常
public:
	Throw() {
		exception = nullptr;
	}
	virtual void code(FILE *fp) {
		Stmt::code(fp);
		// 跳转异常处理程序
		fprintf(fp, "\tjmp L%d\n", exception->kind);
	}
};

class TryCatch : Stmt{
	Stmt *pTry, *pCatch, *pFinally;
public:
	TryCatch() {
		pTry = pCatch = pFinally = nullptr;
	}
	TryCatch(Stmt* pTry, Stmt* pCatch, Stmt* pFinally) :pTry(pTry), pCatch(pCatch), pFinally(pFinally) {  }
	virtual void code(FILE *fp) {
		Stmt::code(fp);
		// 需要异常处理的程序
		pTry->code(fp);
		fprintf(fp, "\tjmp L%d\n", pCatch->next);
		// 异常处理程序
		fprintf(fp, "L%d:\n", pCatch->begin);
		pCatch->code(fp);
		fprintf(fp, "L%d:\n", pCatch->next);
		// 扫尾操作
		pFinally->code(fp);
	}
};

// -------------符号表-----------

class Symbols{
	int id;
	static int count;
	list<Id*> ids;
	Symbols *prev, *next;
public:
	Symbols(){
		id = count++;
		ids.clear();
		prev = nullptr;
		next = nullptr;
	}
};

int Symbols::count = 0;

// -------------函数-----------

class Function : Word{
	Type *type;
	Symbols *params;
	Symbols *symbols;
	Stmt* body;
public:
	Function(string name, Type *type) :Word(FUNCTION, name), type(type){
		kind = FUNCTION;
		params = new Symbols;
		symbols = new Symbols;
	}
	virtual void code(FILE *fp){
		fprintf(fp, "proc %s:\n", word.c_str());
		list<Id*>::iterator iter;
		int width = type->width;
		for (iter = params->ids.begin(); iter != params->ids.end(); iter++){
			(*iter)->offset = width;
			(*iter)->global = false;
			fprintf(fp, "\t;%s @%d\n", (*iter)->getName(), width);
			width += (*iter)->getWidth();
		}
		width = 0;
		Symbols *table = symbols;
		for (table = symbols; table != nullptr; table = table->next){
			for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = false;
				fprintf(fp, "\t;%s @%d\n", (*iter)->getName(), width);
				width += (*iter)->getWidth();
			}
		}
		fprintf(fp, "\tsub $sp %d\n", width);
		width = 0;
		for (table = params; table != nullptr; table = table->next){
			for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = false;
				width += (*iter)->getWidth();
			}
		}
		body->code(fp);
		fprintf(fp, "endp\n");
	}
};

class Call :ExprAST{
	list<ExprAST*> args;
	Function *func;
public:
	Call() { args.clear(); }
	Call(int line, Function *func) :ExprAST(), func(func) { ; }
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("call\n");
		// 参数计算
		list<ExprAST*>::iterator iter;
		for (iter = args.begin(); iter != args.end(); iter++){
			(*iter)->code(fp);
		}
		// 传递参数
		reverse(args.begin(), args.end());
		for (iter = args.begin(); iter != args.end(); iter++){
			fprintf(fp, "\tparam $%d\n", (*iter)->label);
		}
		// 调用语句
		fprintf(fp, "\tcall %s %d\n", func->word.c_str(), args.size());
	}
};

class Global :ASTNode{
	list<Id*> ids;
	list<Function*> funcs;
	virtual void code(FILE *fp){
		ASTNode::code(fp);
		if (!ids.empty()){
			int width = 0;
			fprintf(fp, ".data\n");
			list<Id*>::iterator iter;
			for (iter = ids.begin(); iter != ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = true;
				fprintf(fp, "\t%s %s\n", (*iter)->getTypeName(), (*iter)->getName());
				width += (*iter)->getWidth();
			}
		}
		fprintf(fp, ".stack\n");
		fprintf(fp, "\t\db dup(0x1000)\n");
		if (!funcs.empty()){
			fprintf(fp, ".code\n");
			list<Function*>::iterator iter2;
			for (iter2 = funcs.begin(); iter2 != funcs.end(); iter2++){
				(*iter2)->code(fp);
			}
		}
	}
};

#endif

#ifndef __VISITOR_H_

class Visitor {
public:
	virtual void visit(ASTNode *node) = 0;
};

class BinaryExpr : Visitor {
public:
	virtual void visit(BinaryExpr *expr)  {

	}
};

class UnaryExprVisitor : Visitor {
public:
	virtual void visit(UnaryExpr *expr) {

	}
};

class StmtVisitor : Visitor{
public:
	virtual void visit(Stmt *stmt) {

	}
};

#endif // !__VISITOR_H_
