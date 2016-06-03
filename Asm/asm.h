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
	bool match(int kind){
		if (s->kind == kind){
			s = lexer->scan();
			return true;
		}
		s = lexer->scan();
		return false;
	}
	Code* data(){
		Data *d = new Data;
		d->opt = NULL;
		d->line = lexer->line;
		match(DATA);
		d->width = ((Integer*)s)->value;
		DS = 0;
		CS = d->width;
		match(INT);
		match(DATA);
		return d;
	}
	Code* store(){
		Store *l = new Store;
		l->line = lexer->line;
		l->opt = STORE;
		match(STORE);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(INT);
		switch (s->kind){
		case '#':l->opt |= MR_A; match('#'); break;
		case '*':l->opt |= MR_B; match('*'); break;
		default:break;
		}
		l->addr = ((Integer*)s)->value;
		l->width = 4;
		match(INT);
		return l;
	}
	Code* halt(){
		Halt *h = new Halt;
		h->line = lexer->line;
		h->opt = HALT;
		h->width = 1;
		match(HALT);
		return h;
	}
	Code* load(){
		Load *l = new Load;
		l->line = lexer->line;
		l->opt = LOAD;
		match(LOAD);
		match('$');
		l->reg = ((Integer*)s)->value;
		match(INT);
		switch (s->kind){
		case '#':l->opt |= MR_A; match('#'); break;
		case '*':l->opt |= MR_B; match('*'); break;
		default:break;
		}
		l->addr = ((Integer*)s)->value;
		match(INT);
		l->width = 4;
		return l;
	}
	Code* label(){
		match(LABEL);
		if (lables.find(((Word*)s)->word) == lables.end()){
			lables[((Word*)s)->word] = new Label((Word*)s, cs->width);
		}
		else{
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
		match(INT);
		match('$');
		u->reg2 = ((Integer*)s)->value;
		match(INT);
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
		match(INT);
		match('$');
		a->reg2 = ((Integer*)s)->value;
		match(INT);
		match('$');
		a->reg3 = ((Integer*)s)->value;
		match(INT);
		a->width = 4;
		return a;
	}
	Code* jmp(BYTE b){
		Jmp *j = new Jmp;
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
public:
	int DS = 0;
	int CS = 0;
	Asm(string fp){
		lexer = new Lexer(fp);
	}
	void parse(){
		Code *c = new Code;
		cs = new Codes;
		s = lexer->scan();
		while (s->kind != '#'){
			switch (s->kind){
			case ID:printf("[%3d]find id:%s\n", lexer->line, ((Word*)s)->word.c_str()); break;
			case DATA:c = data(); break;
			case LOAD:c = load(); break;
			case STORE:c = store(); break;
			case HALT:c = halt(); break;
			case LABEL:c = label(); break;
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
			if (c){ cs->codes.push_back(c); c->offset = cs->width; cs->width += c->width; }
		}
	}
	void write(FILE *fp){
		fwrite(&DS, sizeof(WORD), 1, fp);
		fwrite(&CS, sizeof(WORD), 1, fp);
		fwrite(&cs->width, sizeof(WORD), 1, fp);
		cs->code(fp);
	}
};

#endif