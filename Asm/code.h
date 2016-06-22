#ifndef __CODE_H_
#define __CODE_H_

typedef unsigned char BYTE;
typedef unsigned short int WORD;

enum Tag{
	// 虚拟机指令集
	HALT,
	ADD, SUB, MUL, DIV, MOD, CMP,
	JMP, JNE, JG, JE, JB, JGE, JBE,
	LOAD, STORE,
	PUSH, POP,
	NEG,
	MOV, IN, OUT,
	SHL, SHR, SAL, SAR, SRL, SRR,// 
	LOOP,
	// 关键字
	ID = 256, INT, END, LABEL, DATA, CODE, STACK
};

#endif