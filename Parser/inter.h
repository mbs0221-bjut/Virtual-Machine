#include "lexer.h"

#ifndef __INTER_H_
#define __INTER_H_

#include <list>
#include <stack>

struct Node{
	virtual void code(FILE *fp){
	}
};

struct Nodes :Node{
	list<Node*> nodes;
	virtual void code(FILE *fp){
		list<Node*>::iterator iter;
		for (iter = nodes.begin(); iter != nodes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

//语句
struct Stmt :Node{
	int line;
	int begin, next;
	static int label;
	static int newlabel(){
		return label++;
	}
	virtual void code(FILE *fp){
		Node::code(fp);
		printf("[%04d]", line);
	}
};

int Stmt::label = 0;

//语句块
struct Stmts :Stmt{
	list<Stmt*> Ss;
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
struct Expr :Stmt{
	char opt;
	int label;
	static int count;
	Expr(char opt) :opt(opt){ label = count++; }
	virtual void code(FILE *fp){
		Node::code(fp);
	}
};

int Expr::count = 0;

// 条件表达式
struct Cond :Expr{
	int True, False;
	Expr *E1, *E2;
	Cond(char opt, Expr *E1, Expr *E2) :Expr(opt), E1(E1), E2(E2){}
	virtual void code(FILE *fp){
		Expr::code(fp);
		E1->code(fp);
		E2->code(fp);
		fprintf(fp, "\t%c $%d $%d $%d\n", opt, E1->label, E2->label, label);
		switch (opt){
		case '>':fprintf(fp, "\tjg L%d\n", True); break;
		case '=':fprintf(fp, "\tje L%d\n", True); break;
		case '<':fprintf(fp, "\tjb L%d\n", True); break;
		case '!':fprintf(fp, "\tjne L%d\n", True); break;
		default:fprintf(fp, "\tjmp L%d\n", True); break;
		}
		fprintf(fp, "\tjmp L%d\n", False);
	}
};

// 算术表达式
struct Arith :Expr{
	Expr *E1, *E2;
	Arith(char opt, Expr *E1, Expr *E2) :Expr(opt), E1(E1), E2(E2){}
	virtual void code(FILE *fp){
		Expr::code(fp);
		E1->code(fp);
		E2->code(fp);
		switch (opt){
		case '+':fprintf(fp, "\tadd "); break;
		case '-':fprintf(fp, "\tsub "); break;
		case '*':fprintf(fp, "\tmul "); break;
		case '/':fprintf(fp, "\tdiv "); break;
		case '&':fprintf(fp, "\tand "); break;
		default:break;
		}
		fprintf(fp, "$%d $%d $%d\n", E1->label, E2->label, label);
	}
};

struct Unary :Expr{
	Expr *E1;
	Unary(char opt, Expr *E1) :Expr(opt), E1(E1){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		E1->code(fp);
		fprintf(fp, "\t%c $%d $%d\n", opt, E1->label, label);
	}
};

// ID
struct Id :Expr{
	Type *t;
	Word *s;
	int offset;
	bool global;
	Id(Type *t, Word *s) :Expr('@'), t(t), s(s){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		char width = t == Type::Int ? 'w' : 'b';
		if (global){
			fprintf(fp, "\tload%c $%d %s\n", width, label, s->word.c_str());
		}else{
			fprintf(fp, "\tload%c $%d %s\n", width, label, s->word.c_str());
		}
	}
};

struct Number :Expr{
	Integer *s;
	Number(Integer *s) :Expr('@'), s(s){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		char width = s->value > 256 ? 'w' : 'b';
		fprintf(fp, "\tload%c $%d %d\n", width, label, s->value);
	}
};

struct Decl :Stmt{
	list<Id*> ids;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("data\n");
		//fprintf(fp, "\tdata\n");
		//list<Id*>::iterator iter;
		//for (iter = ids.begin(); iter != ids.end(); iter++){
		//	printf("\t$%s [%d]\n", (*iter)->s->word.c_str(), (*iter)->t->width);
		//	fprintf(fp, "\t\t$%s [%d]\n", (*iter)->s->word.c_str(), (*iter)->t->width);
		//}
		//printf("data\n");
		//fprintf(fp, "\tdata\n");
	}
};

struct Assign :Stmt{
	Id *E1;
	Expr *E2;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("assign\n");
		E2->code(fp);
		if (E1->global){
			fprintf(fp, "\tstore %s $%d\n", E1->s->word.c_str(), E2->label);
		}else{
			fprintf(fp, "\tstore %s $%d\n", E1->s->word.c_str(), E2->label);
		}
	}
};

struct If :Stmt{
	Cond *C;
	Stmt *S1;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("if\n");
		next = newlabel();
		C->True = newlabel();
		C->False = next;
		S1->next = next;
		C->code(fp);
		fprintf(fp, "L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "L%d:\n", next);
	}
};

struct Else :Stmt{
	Cond *C;
	Stmt *S1;
	Stmt *S2;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("if-else\n");
		next = newlabel();
		C->True = newlabel();
		C->False = newlabel();
		S1->next = next;
		C->code(fp);
		fprintf(fp, "L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "\tjmp L%d\n", next);
		fprintf(fp, "L%d:\n", C->False);
		S2->code(fp);
		fprintf(fp, "L%d:\n", next);
	}
};

struct While :Stmt{
	Cond *C;
	Stmt *S1;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("while\n");
		begin = newlabel();
		next = newlabel();
		C->True = newlabel();
		C->False = next;
		S1->next = begin;
		fprintf(fp, "L%d:\n", begin);
		C->code(fp);
		fprintf(fp, "L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "\tjmp L%d\n", begin);
		fprintf(fp, "L%d:\n", next);
	}
};

struct Do :Stmt{
	Cond *C;
	Stmt *S1;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("do-while\n");
		begin = newlabel();
		next = newlabel();
		C->True = begin;
		C->False = next;
		S1->next = begin;
		fprintf(fp, "L%d:\n", begin);
		S1->code(fp);
		C->code(fp);
		fprintf(fp, "L%d:\n", next);
	}
};

struct For :Stmt{
	Stmt *S1;
	Cond *C;
	Stmt *S2;
	Stmt *S3;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("for\n");
		begin = newlabel();
		next = newlabel();
		C->True = newlabel();
		C->False = next;
		S2->begin = newlabel();
		S2->next = newlabel();
		S3->next = begin;
		S1->code(fp);
		fprintf(fp, "L%d:\n", begin);
		C->code(fp);
		fprintf(fp, "L%d:\n", C->True);
		S3->code(fp);
		fprintf(fp, "L%d:\n", S2->begin);
		S2->code(fp);
		fprintf(fp, "\tjmp L%d\n", begin);
		fprintf(fp, "L%d:\n", next);
	}
};

struct Case :Stmt{
	Expr *E;
	map<int, Stmt*> Ss;
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
		E->code(fp);
		for (iter = Ss.begin(); iter != Ss.end(); iter++){
			fprintf(fp, "L%d:\n", iter->second->begin);
			fprintf(fp, "= $%d $%d #%d\n", E->label, iter->first);
			iter->second->code(fp);
		}
	}
};

struct Break :Stmt{
	Stmt *stmt;
	static stack<Stmt*> cur;
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

struct Continue :Stmt{
	Stmt *stmt;
	static stack<Stmt*> cur;
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
struct Throw : Stmt {
	Type* exception;// 抛出异常
	Throw() {
		exception = nullptr;
	}
	virtual void code(FILE *fp) {
		Stmt::code(fp);
		// 跳转异常处理程序
		fprintf(fp, "\tjmp L%d\n", exception->kind);
	}
};

struct TryCatch : Stmt{
	Stmt *pTry, *pCatch, *pFinnaly;
	TryCatch() {
		pTry = pCatch = pFinnaly = nullptr;
	}
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
		pFinnaly->code(fp);
	}
};

// 符号表
struct Symbols{
	int id;
	static int count;
	list<Id*> ids;
	Symbols *prev, *next;
	Symbols(){
		id = count++;
		ids.clear();
		prev = nullptr;
		next = nullptr;
	}
};

int Symbols::count = 0;

struct Function : Word{
	Type *type;
	Symbols *params;
	Symbols *symbols;
	Stmt* body;
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
			fprintf(fp, "\t;%s @%d\n", (*iter)->s->word.c_str(), width);
			width += (*iter)->t->width;
		}
		width = 0;
		Symbols *table = symbols;
		for (table = symbols; table != nullptr; table = table->next){
			for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = false;
				fprintf(fp, "\t;%s @%d\n", (*iter)->s->word.c_str(), width);
				width += (*iter)->t->width;
			}
		}
		fprintf(fp, "\tsub $sp %d\n", width);
		width = 0;
		for (table = params; table != nullptr; table = table->next){
			for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = false;
				width += (*iter)->t->width;
			}
		}
		body->code(fp);
		fprintf(fp, "endp\n");
	}
};

struct Call :Expr{
	list<Expr*> args;
	Function *func;
	Call() :Expr('@'){ args.clear(); }
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("call\n");
		// 参数计算
		list<Expr*>::iterator iter;
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

struct Global :Node{
	list<Id*> ids;
	list<Function*> funcs;
	virtual void code(FILE *fp){
		Node::code(fp);
		if (!ids.empty()){
			int width = 0;
			fprintf(fp, ".data\n");
			list<Id*>::iterator iter;
			for (iter = ids.begin(); iter != ids.end(); iter++){
				(*iter)->offset = width;
				(*iter)->global = true;
				fprintf(fp, "\t%s %s\n", (*iter)->t->word.c_str(), (*iter)->s->word.c_str());
				width += (*iter)->t->width;
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