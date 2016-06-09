#ifndef __CODE_H_
#define __CODE_H_

typedef unsigned char BYTE;
typedef unsigned short int WORD;

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
	ID = 256, NUM, END, LABEL, DATA, STACK, CODE, PROC, ENDP, CALL
};

enum REG{ BP, SI, DI, CS, DS, ES, SS, SP };

#endif