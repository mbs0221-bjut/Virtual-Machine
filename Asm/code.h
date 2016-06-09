#ifndef __CODE_H_
#define __CODE_H_

typedef unsigned char BYTE;
typedef unsigned short int WORD;

//宏定义种别码
enum Tag{
	HALT,
	ADD, SUB, MUL, DIV, MOD, CMP,// integer operator
	JMP, JNE, JG, JE, JB, JGE, JBE,
	LOAD, STORE,
	PUSH, POP,
	NEG,
	MOV, IN, OUT,
	SHL, SHR, SAL, SAR, SRL, SRR,// 
	LOOP,
	ID = 256, NUM, END, LABEL, DATA, CODE, STACK, PROC, ENDP, CALL
};

enum REG{
	BP = 256,
	SI,
	DI,
	CS,
	DS,
	ES,
	SS,
	SP
};// 通用寄存器
#endif