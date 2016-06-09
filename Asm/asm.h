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
		match('.');
		match(DATA);
		DS = 0;
		cs->offset += ((Integer*)s)->value;
	}
	void stack(){
		match('.');
		match(STACK);
		cs->offset += ((Integer*)s)->value;
	}
	void code(){
		SS = cs->offset;
		CS = cs->offset;
		match('.');
		match(CODE);
		cs = new Codes;
		s = lexer->scan();
		Code *c = new Code;
		while (s->kind == PROC){
			c = proc();
			if (c){ 
				cs->codes.push_back(c);
				cs->offset = cs->width; 
				cs->width += c->width;
			}
		}
	}
	Code* proc(){
		Func *f = new Func;
		match(PROC);
		f->name = ((Word*)s)->word;
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
			case HALT:c = halt(); break;
			case JE: c = jmp(JE); break;
			case JNE: c = jmp(JNE); break;
			case JB: c = jmp(JB); break;
			case JG: c = jmp(JG); break;
			case JMP: c = jmp(JMP); break;
			case '~':c = unary(); break;
			case '+':c = arith(ADD); break;
			case '-':c = arith(SUB); break;
			case '*':c = arith(MUL); break;
			case '/':c = arith(DIV); break;
			case '%':c = arith(MOD); break;
			case '<':c = arith(CMP); break;
			case '>':c = arith(CMP); break;
			case '=':c = arith(CMP); break;
			case '!':c = arith(CMP); break;
			default:printf("[%3d]find unsupport cmd '%d'\n", lexer->line, s->kind); break;
			}
			if (c){ f->codes.push_back(c); c->offset = f->width; f->width += c->width; }
		}
		match(ENDP);
		return f;
	}
	Code* call(){
		Call *c = new Call;
		match(CALL);
		if (funcs.find(((Word*)s)->word) == funcs.end()){
			printf("[%3d]find unsupport cmd '%d'\n", lexer->line, s->kind);
			match(ID);
			return c;
		}
		c->func = funcs[((Word*)s)->word];
		match(ID);
		return c;
	}
	Code* store(){
		Store *l = new Store;
		l->line = lexer->line;
		l->opt = STORE;
		match(STORE);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(NUM);
		switch (s->kind){
		case '#':l->opt |= MR_A; match('#'); break;// 立即数寻址
		case '@':l->opt |= MR_B; match('*'); break;// BP间接寻址
		case '&':l->opt |= MR_B; match('*'); break;// DS间接寻址
		case ID:l->opt |= MR_B; match(ID); break;
		default:break;
		}
		l->addr = ((Integer*)s)->value;
		l->width = 4;
		match(NUM);
		return l;
	}
	Code* load(){
		Load *l = new Load;
		l->line = lexer->line;
		l->opt = LOAD;
		match(LOAD);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(NUM);
		switch (s->kind){
		case '#':l->opt |= MR_A; match('#'); break;
		case '*':l->opt |= MR_B; match('*'); break;
		case '$':l->opt |= MR_B; match('$'); break;
		case NUM:l->opt |= MR_A; match(NUM); break;
		case ID:l->opt |= MR_B; match(ID); break;
		default:break;
		}
		l->addr = ((Integer*)s)->value;
		match(NUM);
		l->width = 4;
		return l;
	}
	Code* label(){
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
		j->line = lexer->line;
		j->opt = b;
		match(s->kind);
		if (lables.find(((Word*)s)->word) == lables.end()){
			// 没有标签
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
		h->line = lexer->line;
		h->opt = HALT;
		h->width = 1;
		match(HALT);
		return h;
	}
public:
	int DS = 0;
	int CS = 0;
	int SS = 0;
	Asm(string fp){
		lexer = new Lexer(fp);
	}
	void parse(){
		data();
		stack();
		code();
	}
	void write(FILE *fp){
		fwrite(&DS, sizeof(WORD), 1, fp);
		fwrite(&CS, sizeof(WORD), 1, fp);
		fwrite(&SS, sizeof(WORD), 1, fp);
		fwrite(&cs->width, sizeof(WORD), 1, fp);
		cs->code(fp);
	}
};

#endif