#ifndef __CODE_H_
#define __CODE_H_

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int UINT;

// �ʷ���Ԫ����
enum Tag{ ID = 256, NUM, REG, RTYPE, ITYPE, JTYPE, END, LABEL, DATA, STACK, CODE, PROC, ENDP, CALL };

// ָ�
enum Inst{
	HALT,
	ADD, SUB, MUL, DIV, MOD, CMP,	// Scalar
	ADDV, SUBV, MULV, DIVV,			// Vector
	JMP, JNE, JG, JE, JB, JGE, JBE,	// Jump
	LOAD, STORE,					// Load/Store
	PUSH, POP,						// Push/Pop
	NEG,							// Negtive
	MOV, IN, OUT,					// I/O
	SHL, SHR, SAL, SAR, SRL, SRR,	// Shift
	LOOP,							// Loop
};

// MIPSָ�
enum MIPS{
	ADD, ADDU, SUB, SUBU, AND, OR, XOR, NOR, SLT, SLTU, SL1, SR1, SRA, SLLV, SRLV, SRAV, JR,// R-Type
	ADDI, ADDIU, ANDI, ORI, XORI, LUI, LW, SW, BEQ, BNE, SLTI, SLTIU,// I-Type
	J, JAL// J-Type
};

// �Ĵ���
enum Register{ AX, BX, CX, DX, BP, SI, DI, CS, DS, ES, SS, SP };

#endif