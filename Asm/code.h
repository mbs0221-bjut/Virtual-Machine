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
	ID = 256, INT, END, LABEL, DATA, CODE, STACK
};

#endif