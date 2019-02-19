#include "lexer.h"

// 标签
struct Label{
	Word *w;
	WORD offset;
	Label(Word *w, int offset) :w(w), offset(offset) {  }
};

//----------------参考RISC指令集----------------
// 代码
struct Code{
	BYTE opt;
	WORD line = 0;// 当前指令在汇编文件中的位置
	WORD width = 0;// 当前代码所占用的宽度
	WORD offset = 0;// 当前代码段的偏移量
	virtual void code(FILE* fp){
		printf("[%04d][%04d][%04x]", line, width, offset);
	}
};

// 代码块
struct Codes :Code{
	list<Code*> codes;
	virtual void code(FILE* fp){
		list<Code*>::iterator iter;
		for (iter = codes.begin(); iter != codes.end(); iter++){
			(*iter)->code(fp);
		}
	}
};

// 数据段
struct Data :Code{
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("data:%2d\n", width);
		for (WORD i = 0; i < width; i++){
			fwrite(&opt, sizeof(BYTE), 1, fp);
		}
	}
};

// 函数
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

// 参数传递
struct Param :Code{
	BYTE reg;// 寄存器
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("param\t$%04x;%s\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

// 函数调用
struct Call :Code{
	Func *func;// 函数
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("call\t$%04x;%s\n", func->offset, func->name.c_str());
		fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

// Load指令
struct Load :Code{
	BYTE am;// 内存地址寻址方式
	BYTE reg;// 通用寄存器
	WORD addr;// RAM地址
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("load\t$%02x $%02x $%04x\n", opt, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};// 直接寻址

// Store指令
struct Store :Code{
	BYTE am;// 内存地址寻址方式
	BYTE reg;// 通用寄存器
	WORD addr;// RAM地址
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("store\t$%02x $%02x $%04x\n", opt, reg, addr);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
		fwrite(&addr, sizeof(WORD), 1, fp);
	}
};

// 入栈指令
struct Push :Code{
	BYTE reg;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("push\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};

// 出栈指令
struct Pop :Code{
	BYTE reg;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("pop\t$%02x $%02x\n", opt, reg);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&reg, sizeof(BYTE), 1, fp);
	}
};

// 停机指令
struct Halt:Code{
	virtual void code(FILE* fp){
		Code::code(fp);
		opt = HALT;
		printf("halt\t$%02x\n", opt);
		fwrite(&opt, sizeof(BYTE), 1, fp);
	}
};

// 跳转指令
struct Jmp :Code{
	Label *addr;
	virtual void code(FILE* fp){
		Code::code(fp);
		printf("jmp \t$%02x $%04x\n", opt, addr->offset);
		fwrite(&opt, sizeof(BYTE), 1, fp);
		fwrite(&addr->offset, sizeof(WORD), 1, fp);
	}
};

// 双目运算指令
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

// 单目运算指令
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
