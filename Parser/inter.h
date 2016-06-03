#include "lexer.h"

#ifndef __INTER_H_
#define __INTER_H_

#include <list>

struct Node{
	virtual void code(FILE *fp){
	}
};

//表达式
struct Expr :Node{
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
		fprintf(fp, "%c $%d $%d $%d\n", opt, E1->label, E2->label, label);
		switch (opt){
		case '>':fprintf(fp, "jg L%d\n", True); break;
		case '=':fprintf(fp, "je L%d\n", True); break;
		case '<':fprintf(fp, "jb L%d\n", True); break;
		case '!':fprintf(fp, "jne L%d\n", True); break;
		default:fprintf(fp, "jmp L%d\n", True); break;
		}
		fprintf(fp, "jmp L%d\n", False);
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
		fprintf(fp, "%c $%d $%d $%d\n", opt, E1->label, E2->label, label);
	}
};

struct Unary :Expr{
	Expr *E1;
	Unary(char opt, Expr *E1) :Expr(opt), E1(E1){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		E1->code(fp);
		fprintf(fp, "%c $%d $%d\n", opt, E1->label, label);
	}
};

// ID
struct Id :Expr{
	Type *t;
	Word *s;
	int offset;
	Id(Type *t, Word *s, int offset) :Expr('@'), t(t), s(s), offset(offset){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		fprintf(fp, "load $%d *%d\n", label, offset);
	}
};

struct Number :Expr{
	Integer *s;
	Number(Integer *s) :Expr('@'), s(s){  }
	virtual void code(FILE *fp){
		Expr::code(fp);
		fprintf(fp, "load $%d #%d\n", label, s->value);
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

int Stmt::label = 0;

struct Decl :Stmt{
	list<Id*> ids;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("decl\n");
		int width = 0;
		list<Id*>::iterator iter;
		for (iter = ids.begin(); iter != ids.end(); iter++){
			width += (*iter)->t->width;
		}
		fprintf(fp, "data %d data\n", width);
	}
};

struct Assign :Stmt{
	Id *E1;
	Expr *E2;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("assign\n");
		E2->code(fp);
		fprintf(fp, "store $%d *%d\n", E2->label, E1->offset);
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
		fprintf(fp, "label L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "label L%d:\n", next);
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
		fprintf(fp, "label L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "jmp L%d\n", next);
		fprintf(fp, "label L%d:\n", C->False);
		S2->code(fp);
		fprintf(fp, "label L%d:\n", next);
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
		fprintf(fp, "label L%d:\n", begin);
		C->code(fp);
		fprintf(fp, "label L%d:\n", C->True);
		S1->code(fp);
		fprintf(fp, "jmp L%d\n", begin);
		fprintf(fp, "label L%d:\n", next);
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
		fprintf(fp, "label L%d:\n", begin);
		S1->code(fp);
		C->code(fp);
		fprintf(fp, "label L%d:\n", next);
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
		S3->next = begin;
		S1->code(fp);
		fprintf(fp, "label L%d:\n", begin);
		C->code(fp);
		fprintf(fp, "label L%d:\n", C->True);
		S2->code(fp);
		S3->code(fp);
		fprintf(fp, "jmp L%d\n", begin);
		fprintf(fp, "label L%d:\n", next);
	}
};

struct Case :Stmt{
	Expr *E;
	map<int, Stmt*> Ss;
	virtual void code(FILE *fp){
		Stmt::code(fp);
		printf("case\n");
		//fprintf(fp, "jmp L%d\n", begin);
		//map<int, Stmt*>::iterator iter;
		//for (iter = Ss.begin(); iter != Ss.end(); iter++){
		//	fprintf(fp, "label L%d:\n", iter->second->begin);
		//	iter->second->code(fp);
		//}
		//fprintf(fp, "label L%d:\n", begin);
		//E->code(fp);
		//for (iter = Ss.begin(); iter != Ss.end(); iter++){
		//	fprintf(fp, "label L%d:\n", iter->second->begin);
		//	fprintf(fp, "= $%d $%d $%d", E->label, );
		//	iter->second->code(fp);
		//}
	}
};

#endif