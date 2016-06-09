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
		fprintf(fp, "\t%c $%d $%d $%d\n", opt, E1->label, E2->label, label);
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
		if (global){
			fprintf(fp, "\tload $%d #ds + %d;%s\n", label, offset, s->word.c_str());
		}else{
			fprintf(fp, "\tload $%d #sp + %d;%s\n", label, offset, s->word.c_str());
		}
	}
};

struct Number :Expr{
	Integer *s;
	Number(Integer *s) :Expr('@'), s(s){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		int width = s->value > 256 ? 2 : 1;
		fprintf(fp, "\tload $%d %d\n", label, s->value);
	}
};

struct Decl :Stmt{
	list<Id*> ids;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		//printf("\ndata\n");
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
		fprintf(fp, "\tstore $%d %s\n", E2->label, E1->s->word.c_str());
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
			fprintf(fp, "= $%d $%d #%d", E->label, iter->first);
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

struct Symbols{
	int id;
	static int count;
	list<Id*> ids;
	Symbols *prev;
	Symbols(){
		id = count++;
		ids.clear();
		prev = nullptr;
	}
};

int Symbols::count = 0;

struct Function : Word{
	Type *type;
	Symbols *symbols;
	Stmt* body;
	Function(string name, Type *type) :Word(FUNCTION, name), type(type){
		kind = FUNCTION;
		symbols = new Symbols;
	}
	virtual void code(FILE *fp){
		int width = 0;
		list<Id*>::iterator iter;
		for (iter = symbols->ids.begin(); iter != symbols->ids.end(); iter++){
			width += (*iter)->t->width;
			(*iter)->offset = width;
			(*iter)->global = false;
		}
		fprintf(fp, "proc %s:\n", word.c_str());
		fprintf(fp, "\tpush bp\n");
		fprintf(fp, "\tmov bp sp\n");
		fprintf(fp, "\tsub sp %d\n", width);// 为参数和局部变量预留空间
		fprintf(fp, "\tsub sp %d\n", type->width);// 为返回值预留空间
		body->code(fp);
		fprintf(fp, "\tret\n");
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
		fprintf(fp, "\tpush bp\n");
		fprintf(fp, "\tmov bp sp\n");
		// 传递参数
		list<Expr*>::iterator iter;
		for (iter = args.begin(); iter != args.end(); iter++){
			(*iter)->code(fp);
		}
		// 传递参数入栈
		reverse(args.begin(), args.end());
		for (iter = args.begin(); iter != args.end(); iter++){
			fprintf(fp, "\tpush $%d\n", (*iter)->label);
		}
		// 跳转到函数体处开始执行
		fprintf(fp, "\tcall %s\n", func->word.c_str());
		// 返回参数出栈
		reverse(args.begin(), args.end());
		for (iter = args.begin(); iter != args.end(); iter++){
			fprintf(fp, "\tpop $%d\n", (*iter)->label);
		}
	}
};

struct Global :Node{
	list<Id*> ids;
	list<Function*> funcs;
	virtual void code(FILE *fp){
		Node::code(fp);
		if (!ids.empty()){
			fprintf(fp, "data\n");
			int width = 0;
			list<Id*>::iterator iter;
			for (iter = ids.begin(); iter != ids.end(); iter++){
				fprintf(fp, "\t\t$%s [%d]\n", (*iter)->s->word.c_str(), (*iter)->t->width);
				width += (*iter)->t->width;
				(*iter)->offset = width;
				(*iter)->global = true;
			}
			fprintf(fp, "data\n");
		}
		if (!funcs.empty()){
			list<Function*>::iterator iter2;
			for (iter2 = funcs.begin(); iter2 != funcs.end(); iter2++){
				(*iter2)->code(fp);
			}
		}
	}
};

#endif