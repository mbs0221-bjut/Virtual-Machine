#include "lexer.h"

struct Label{
	Word *w;
	WORD offset;
	Label(Word *w, int offset) :w(w), offset(offset) {  }
};

struct Code{
	BYTE opt;
	WORD line = 0;// ��ǰָ���ڻ���ļ��е�λ��
	WORD width = 0;// ��ǰ������ռ�õĿ��
	WORD offset = 0;// ��ǰ����ε�ƫ����
	virtual void code(FILE* fp){
		printf("[%04d][%04d][%04x]", line, width, offset);
	}
};

struct Codes :Code{
	list<Code*> codes;
	virtual void code(FILE* fp){
		list<Code*>::iterator iter;
		for (iter = codes.begin(); iter != codes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

struct Data :Code{
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("data:%2d\n", width);
		for (WORD i = 0; i < width; i++){
			fwrite(&opt, sizeof(BYTE), 1, fp);
		}
	}
};

struct Func :Code{
	string name;
	list<Code*> codes;
	virtual void code(FILE* fp){
		list<Code*>::iterator iter;
		for (iter = codes.begin(); iter != codes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

struct Call :Code{
	Func *func;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("call\t$%04x;%s\n", func->offset, func->name.c_str());
		fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

struct Load :Code{
	BYTE reg;
	WORD addr;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("load\t$%02x $%02x $%04x\n", opt, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};// ֱ��Ѱַ

struct Store :Code{
	BYTE reg;
	WORD addr;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("store\t$%02x $%02x $%04x\n", opt, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};

struct Push :Code{
	BYTE reg;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("push\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};// ֱ��Ѱַ

struct Pop :Code{
	BYTE reg;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("pop\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};

struct Halt:Code{
	virtual void code(FILE* fp){
		Code::code(fp);
		opt = HALT;
		printf("halt\t$%02x\n", opt);
		fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

struct Jmp :Code{
	Label *addr;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("jmp \t$%02x $%04x\n", opt, addr->offset);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&addr->offset, sizeof(WORD), 1, fp);
	}
};

struct Arith :Code{
	BYTE reg1, reg2, reg3;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("bino\t$%02x $%02x $%02x $%02x\n", opt, reg1, reg2, reg3);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg1, sizeof(BYTE), 1, fp);
		fwrite(&reg2, sizeof(BYTE), 1, fp);
		fwrite(&reg3, sizeof(BYTE), 1, fp);
	}
};

struct Unary :Code{
	BYTE reg1, reg2;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("unary\t$%02x $%02x $%02x\n", opt, reg1, reg2);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg1, sizeof(BYTE), 1, fp);
		fwrite(&reg2, sizeof(BYTE), 1, fp);
	}
};