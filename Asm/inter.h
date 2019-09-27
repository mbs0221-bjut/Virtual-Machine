#include "lexer.h"


//----------------参考RISC指令集----------------
class Code {
protected:
	BYTE opt;
	WORD line = 0;// 当前指令在汇编文件中的位置
	WORD offset = 0;// 当前代码段的偏移量
public:
	int getWitdh() { return 0; }
	Code(int line, BYTE opt) :line(line), opt(opt) { ; }
	virtual void code(FILE* fp){
		printf("[%04d][%04x]", line, offset);
	}
};

class Codes : public Code{
	list<Code*> codes;
public:
	Codes(int line) : Code(line, CODE) { ; }
	void pushCode(Code *c) { codes.push_back(c); }
	int getWidth() {
		int width;
		list<Code*>::iterator iter;
		for (iter = codes.begin(); iter != codes.end(); iter++) {
			width += (*iter)->getWitdh();
		}
		return width;
	}
	virtual void code(FILE* fp){
		list<Code*>::iterator iter;
		for (iter = codes.begin(); iter != codes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

class Data : public Code{
	int width;
public:
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("data:\n");
		//for (WORD i = 0; i < width; i++){
		//	fwrite(&opt, sizeof(BYTE), 1, fp);
		//}
	}
};

class Label : public Code {
	string name;
public:
	Label(int line, string name) : Code(line, LABEL), name(name) { ; }
};

class Proc : public Code {
	string name;
	Code *body;
public:
	Proc(int line, string name, Code *body) :Code(line, PROC), name(name), body(body) { ; }
	virtual void code(FILE* fp) {
		Code::code(fp);
		body->code(fp);
	}
};

class Arith : public Code {
	BYTE reg1, reg2, reg3;
public:
	Arith(int line, BYTE opt, BYTE reg1, WORD reg2, WORD reg3) : Code(line, opt), reg1(reg1), reg2(reg2), reg3(reg3) { ; }
	virtual void code(FILE* fp) {
		Code::code(fp);
		printf("bino\t$%02x $%02x $%02x $%02x\n", opt, reg1, reg2, reg3);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg1, sizeof(BYTE), 1, fp);
		fwrite(&reg2, sizeof(BYTE), 1, fp);
		fwrite(&reg3, sizeof(BYTE), 1, fp);
	}
};

class Unary : public Code {
	BYTE reg1, reg2;
public:
	Unary(int line, BYTE opt, BYTE reg1, BYTE reg2) :Code(line, opt), reg1(reg1), reg2(reg2) { ; }
	virtual void code(FILE* fp) {
		Code::code(fp);
		printf("unary\t$%02x $%02x $%02x\n", opt, reg1, reg2);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg1, sizeof(BYTE), 1, fp);
		fwrite(&reg2, sizeof(BYTE), 1, fp);
	}
};

class Load : public Code{
	BYTE reg;
	WORD addr;
public:
	Load(int line, BYTE reg, WORD addr) : Code(line, LOAD), reg(reg), addr(addr) { }
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("load\t$%02x $%02x $%04x\n", opt, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};

class Store : public Code {
	BYTE reg;
	WORD addr;
public:
	Store(int line, BYTE reg, WORD addr) : Code(line, STORE), reg(reg), addr(addr) { }
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("store\t$%02x $%02x $%04x\n", STORE, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};

class Push : public Code{
	BYTE reg;
public:
	Push(int line, BYTE reg) : Code(line, PUSH), reg(reg) { ; }
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("push\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};

class Pop : public Code{
	BYTE reg;
public:
	Pop(int line, BYTE reg) : Code(line, POP), reg(reg) { ; }
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("pop\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};

class Jmp : public Code {
	Label *label;
public:
	Jmp(int line, Label *label) : Code(line, JMP), label(label) { ; }
	virtual void code(FILE* fp) {
		Code::code(fp);
		//printf("jmp \t$%02x $%04x\n", opt, addr->offset);
		//fwrite(&opt, sizeof(BYTE), 1, fp);
		//fwrite(&addr->offset, sizeof(WORD), 1, fp);
	}
};

class Param : public Code {
	BYTE reg;// 寄存器
	virtual void code(FILE* fp) {
		Code::code(fp);
		//printf("param\t$%04x;%s\n", opt, reg);
		//fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

class Call : public Code {
	Proc *func;// 函数
public:
	Call(int line, Proc *func) : Code(line, PROC), func(func) { ; }
	virtual void code(FILE* fp) {
		Code::code(fp);
		//printf("call\t$%04x;%s\n", func->offset, func->name.c_str());
		//fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

class Halt: public Code {
public:
	Halt(int line) : Code(line, HALT) { ; }
	virtual void code(FILE* fp){
		Code::code(fp);
		//printf("halt\t$%02x\n", opt);
		//fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};