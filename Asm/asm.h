#ifndef __ASM_H_
#define __ASM_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <list>

#include "inter.h"
#include "vm.h"

using namespace std;

class Asm{
private:
	int DS = 0;
	int CS = 0;
	int SS = 0;
	Token *s;
	Lexer *lexer;
	Codes *cs;
	map<string, Label*> lables;
	map<string, Func*> funcs;
	bool match(int kind){
		if (s->kind == kind){
			s = lexer->scan();
			return true;
		}
		s = lexer->scan();
		return false;
	}
	void data(){
		printf("[%03d]data.\n", lexer->line);
		match('.');
		match(DATA);
		cs->offset = cs->width;
		cs->width += ((Integer*)s)->value;
		DS = 0;
		match(NUM);
	}
	void stack(){
		printf("[%03d]stack.\n", lexer->line);
		match('.');
		match(STACK);
		cs->offset = cs->width;
		cs->width += ((Integer*)s)->value;
		SS = cs->width;
		match(NUM);
	}
	void code(){
		match('.');
		match(CODE);
		CS = cs->offset;
		s = lexer->scan();
		Code *c = new Code;
		while (s->kind == PROC){
			c = proc();
			if (c){ 
				printf("[%03d]find proc.\n", c->line);
				cs->codes.push_back(c);
				c->offset = cs->width; 
				cs->width += c->width;
			}
		}
		match('#');
	}
	Code* proc(){
		Func *f = new Func;
		printf("[%03d]proc.\n", lexer->line);
		match(PROC);
		f->line = lexer->line;
		f->offset = cs->width;
		f->name = ((Word*)s)->word;
		f->width = 0;
		match(ID);
		match(':');
		funcs[f->name] = f;
		Code *c = new Code;
		while (s->kind != ENDP){
			switch (s->kind){
			case ID:c = label(); break;
			case CALL: c = call(); break;
			case LOAD:c = load(); break;
			case STORE:c = store(); break;
			case PUSH:c = push(); break;
			case POP:c = pop(); break;
			case HALT:c = halt(); break;
			case JE: c = jmp(JE); break;
			case JNE: c = jmp(JNE); break;
			case JB: c = jmp(JB); break;
			case JG: c = jmp(JG); break;
			case JMP: c = jmp(JMP); break;
			case '~':c = unary(); break;
			case ADD:c = arith(ADD); break;
			case SUB:c = arith(SUB); break;
			case '+':c = arith(ADD); break;
			case '-':c = arith(SUB); break;
			case '*':c = arith(MUL); break;
			case '/':c = arith(DIV); break;
			case '%':c = arith(MOD); break;
			case '<':c = arith(CMP); break;
			case '>':c = arith(CMP); break;
			case '=':c = arith(CMP); break;
			case '!':c = arith(CMP); break;
			default:printf("[%3d]find unsupported cmd '%c\n", lexer->line, s->kind); break;
			}
			if (c){ f->codes.push_back(c); c->offset = f->width; f->width += c->width; }
		}
		match(ENDP);
		return f;
	}
	Code* call(){
		Call *c = new Call;
		c->line = lexer->line;
		printf("[%03d]call.\n", lexer->line);
		match(CALL);
		if (funcs.find(((Word*)s)->word) == funcs.end()){
			printf("[%3d] '%d' undeclared function.\n", lexer->line, s->kind);
			match(ID);
			return c;
		}
		c->func = funcs[((Word*)s)->word];
		match(ID);
		c->width = 3;
		return c;
	}
	Code* pop(){
		Pop *p = new Pop;
		printf("[%03d]pop.\n", lexer->line);
		p->line = lexer->line;
		p->opt = POP;
		match(POP);
		match('$');
		p->reg = ((Integer*)s)->value;
		match(NUM);
		p->width = 2;
		return p;
	}
	Code* push(){
		Push *p = new Push;
		printf("[%03d]push.\n", lexer->line);
		p->line = lexer->line;
		p->opt = PUSH;
		match(PUSH);
		match('$');
		p->reg = ((Integer*)s)->value;
		match(NUM);
		p->width = 2;
		return p;
	}
	Code* store(){
		Store *l = new Store;
		Token *i = new Integer(NUM, 0);;
		printf("[%03d]store.\n", lexer->line);
		l->line = lexer->line;
		l->opt = STORE;
		match(STORE);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(NUM);
		switch (s->kind){
		case NUM:l->am |= MR_A; i = s; match(NUM); break;// immÁ¢¼´ÊýÑ°Ö·
		case '#':l->am |= MR_B; match('#'); i = s; match(NUM); break;// #addrÖ±½ÓÑ°Ö·
		case '[':l->am |= MR_C; match('['); i = s; match(NUM); match(']'); break;// [addr]¼ä½ÓÑ°Ö·
		case REG:l->am |= MR_D; i = s; match(REG); break;// reg¼Ä´æÆ÷Ñ°Ö·
		default:break;
		}
		l->addr = ((Integer*)i)->value;
		l->width = 4;
		return l;
	}
	Code* load(){
		Load *l = new Load;
		Token *i = new Integer(NUM, 0);
		printf("[%03d]load.\n", lexer->line);
		l->line = lexer->line;
		l->opt = LOAD;
		match(LOAD);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(NUM);
		switch (s->kind){
		case NUM:l->am |= MR_A; i = s; match(NUM); break;// immÁ¢¼´ÊýÑ°Ö·
		case '#':l->am |= MR_B; match('#'); i = s; match(NUM); break;// #addrÖ±½ÓÑ°Ö·
		case '[':l->am |= MR_C; match('['); i = s; match(NUM); match(']'); break;// [addr]¼ä½ÓÑ°Ö·
		case REG:l->am |= MR_D; i = s; match(REG); break;// reg¼Ä´æÆ÷Ñ°Ö·
		default:break;
		}
		l->addr = ((Integer*)i)->value;
		l->width = 4;
		return l;
	}
	Code* label(){
		printf("[%03d]label.\n", lexer->line);
		if (lables.find(((Word*)s)->word) == lables.end()){
			lables[((Word*)s)->word] = new Label((Word*)s, cs->width);
		}else{
			lables[((Word*)s)->word]->offset = cs->width;
		}
		match(ID);
		match(':');
		return nullptr;
	}
	Code* unary(){
		Unary *u = new Unary;
		printf("[%03d]unary.\n", lexer->line);
		u->line = lexer->line;
		match('~');
		u->opt = NEG;
		match('$');
		u->reg1 = ((Integer*)s)->value;
		match(NUM);
		match('$');
		u->reg2 = ((Integer*)s)->value;
		match(NUM);
		u->width = 3;
		return u;
	}
	Code* arith(BYTE b){
		Arith *a = new Arith;
		printf("[%03d]arith.\n", lexer->line);
		a->line = lexer->line;
		a->opt = b;
		match(s->kind);
		match('$');
		a->reg1 = ((Integer*)s)->value;
		match(NUM);
		match('$');
		a->reg2 = ((Integer*)s)->value;
		match(NUM);
		match('$');
		a->reg3 = ((Integer*)s)->value;
		match(NUM);
		a->width = 4;
		return a;
	}
	Code* jmp(BYTE b){
		Jmp *j = new Jmp;
		printf("[%03d]jmp.\n", lexer->line);
		j->line = lexer->line;
		j->opt = b;
		match(s->kind);
		if (lables.find(((Word*)s)->word) == lables.end()){
			// Ã»ÓÐ±êÇ©
			j->addr = new Label((Word*)s, cs->width);
			lables[((Word*)s)->word] = j->addr;
		}
		else{
			j->addr = lables[((Word*)s)->word];
		}
		match(ID);
		j->width = 3;
		return j;
	}
	Code* halt(){
		Halt *h = new Halt;
		printf("[%03d]halt.\n", lexer->line);
		h->line = lexer->line;
		h->opt = HALT;
		h->width = 1;
		match(HALT);
		return h;
	}
public:
	Asm(string fp){
		lexer = new Lexer(fp);
	}
	void parse(){
		cs = new Codes;
		data();
		stack();
		code();
	}
	void write(FILE *fp){
		BYTE b = 0x00;
		fwrite(&DS, sizeof(WORD), 1, fp);
		fwrite(&CS, sizeof(WORD), 1, fp);
		fwrite(&SS, sizeof(WORD), 1, fp);
		fwrite(&cs->width, sizeof(WORD), 1, fp);
		//fwrite(&b, sizeof(BYTE)*CS, 1, fp);
		cs->code(fp);
	}
};

#endif