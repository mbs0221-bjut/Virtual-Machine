#pragma once

typedef char S1;
typedef short int S2;
typedef int S4;
typedef long int S8;

typedef unsigned char U1;
typedef unsigned short int U2;
typedef unsigned int U4;
typedef unsigned long int U8;

typedef float F4;
typedef double F8;

// ָ�
enum Inst {
	LBI, LWI, LDI, LQI, LF1I, LF2I,
	LAD, LAI,
	LB, LW, LD, LQ, LF1, LF2,
	SB, SW, SD, SQ, SF1, SF2,
	PUSHB, PUSHW, PUSHD, PUSHQ, PUSHF1, PUSHF2,
	POPB, POPW, POPD, POPQ, POPF1, POPF2,
	MOV, MOVF, MOVD,
	JMP, JE, JNE, SLT, INT, DI, EI, HALT, NOP,
	AND, OR, XOR, NOT, BT, BS,
	SRA, SRL, SL,
	ADD, SUB, MUL, DIV,
	CAST_IF, CAST_ID, CAST_FI, CAST_FD, CAST_DI, CAST_DF,
	FADD, FSUB, FMUL, FDIV, FSLT,
	DADD, DSUB, DMUL, DDIV, DSLT
};

// Load

#define EXEC_LBI();		R[RAM[IP+1]]=(S1)RAM[IP+2];\
						IP+=3;

#define EXEC_LWI();		R[RAM[IP+1]]=(S2)*((S2*)&RAM[IP+2]);\
						IP+=4;

#define EXEC_LDI();		R[RAM[IP+1]]=(S4)*((S4*)&RAM[IP+2]);\
						IP+=6;

#define EXEC_LQI();		R[RAM[IP+1]]=(S8)*((S8*)&RAM[IP+2]);\
						IP+=10;

#define EXEC_LF1I();	RF[RAM[IP+1]]=(F4)*((F4*)&RAM[IP+2]);\
						IP+=6;

#define EXEC_LF2I();	RF[RAM[IP+1]]=(F8)*((F8*)&RAM[IP+2]);\
						IP+=10;

#define EXEC_LAD();		R[RAM[IP+1]]=(U8)*((U8*)&RAM[IP+2]);\
						IP+=10;

#define EXEC_LAI();		R[RAM[IP+1]]=R[RAM[IP+2]]+((S8)*((S8*)&RAM[IP+3]));\
						IP+=11;

#define EXEC_LB();		R[RAM[IP+1]]=(S8)(*((S1*)&RAM[R[RAM[IP+2]]]));\
						IP+=3;

#define EXEC_LW();		R[RAM[IP+1]]=(S8)(*((S2*)&RAM[(U8)R[RAM[IP+2]]]));\
						IP+=3;

#define EXEC_LD();		R[RAM[IP+1]]=(S8)(*((S4*)&RAM[(U8)R[RAM[IP+2]]]));\
						IP+=3;

#define EXEC_LQ();		R[RAM[IP+1]]=(S8)(*((S8*)&RAM[(U8)R[RAM[IP+2]]]));\
						IP+=3;

#define EXEC_LF1();		RF[RAM[IP+1]]=*((F4*)&RAM[(U8)R[RAM[IP+2]]]);\
						IP+=3;

#define EXEC_LF2();		RD[RAM[IP+1]]=*((F8*)&RAM[(U8)R[RAM[IP+2]]]);\
						IP+=3;

// Store

#define EXEC_SB();		RAM[R[RAM[IP+2]]]=(S1)R[RAM[IP+1]];\
						IP+=3;

#define EXEC_SW();		*((S2*)&RAM[R[RAM[IP+2]]])=(S2)R[RAM[IP+1]];\
						IP+=3;

#define EXEC_SD();		*((S4*)&RAM[R[RAM[IP+2]]])=(S4)R[RAM[IP+1]];\
						IP+=3;

#define EXEC_SQ();		*((S8*)&RAM[R[RAM[IP+2]]])=(S8)R[RAM[IP+1]];\
						IP+=3;

#define EXEC_SF1();		*((F4*)&RAM[R[RAM[IP+2]]])=RF[RAM[IP+1]];\
						IP+=3;

#define	EXEC_SF2();		*((F8*)&RAM[R[RAM[IP+2]]])=RD[RAM[IP+1]];\
						IP+=3;

// Push

#define EXEC_PUSHB();	SP--;\
						RAM[SP]=(S1)(R[RAM[IP+1]]);\
						IP+=2;

#define EXEC_PUSHW();	SP-=2;\
						*((S2*)&RAM[SP])=(S2)(R[RAM[IP+1]]);\
						IP+=2;

#define EXEC_PUSHD();	SP-=4;\
						*((S4*)&RAM[SP])=(S4)(R[RAM[IP+1]]);\
						IP+=2;

#define EXEC_PUSHQ();	SP-=8;\
						*((S8*)&RAM[SP])=(S8)(R[RAM[IP+1]]);\
						IP+=2;

#define	EXEC_PUSHF1();	SP-=4;\
						*((F4*)&RAM[SP])=(F4)(RF[RAM[IP+1]]);\
						IP+=2;

#define	EXEC_PUSHF2();	SP-=8;\
						*((F8*)&RAM[SP])=(F8)(RF[RAM[IP+1]]);\
						IP+=2;

// Pop

#define EXEC_POPB();	R[RAM[IP+1]]=(S1)*((S1*)&RAM[SP]);\
						SP+=1;\
						IP+=2;

#define	EXEC_POPW();	R[RAM[IP+1]]=(S2)*((S2*)&RAM[SP]);\
						SP+=2;\
						IP+=2;

#define EXEC_POPD();	R[RAM[IP+1]]=(S4)*((S4*)&RAM[SP]);\
						SP+=4;\
						IP+=2;

#define EXEC_POPQ();	R[RAM[IP+1]]=(S8)*((S8*)&RAM[SP]);\
						SP+=8;\
						IP+=2;

#define EXEC_POPF1();	RF[RAM[IP+1]]=(F4)*((F4*)&RAM[SP]);\
						SP+=4;\
						IP+=2;

#define EXEC_POPF2();	RD[RAM[IP+1]]=(F8)*((F8*)&RAM[SP]);\
						SP+=8;\
						IP+=2;

// Mov

#define EXEC_MOV();		R[RAM[IP+1]]=R[RAM[IP+2]];\
						IP+=3;

#define EXEC_MOVF();	RF[RAM[IP+1]]=RF[RAM[IP+2]];\
						IP+=3;

#define EXEC_MOVD();	RD[RAM[IP+1]]=RD[RAM[IP+2]];\
						IP+=3;

// Jump

#define EXEC_JMP();		IP=R[RAM[IP+1]];

#define EXEC_JE();		if(R[RAM[IP+1]]==R[RAM[IP+2]])\
							IP=R[RAM[IP+3]];\
						else\
							IP+=4;

#define EXEC_JNE();		if(R[RAM[IP+1]]!=R[RAM[IP+2]])\
							IP=R[RAM[IP+3]];\
						else\
							IP+=4;

#define EXEC_SLT();		if(R[RAM[IP+2]]<R[RAM[IP+3]])\
							R[RAM[IP+1]]=(U8)0x1;\
						else\
							R[RAM[IP+1]]=(U8)0;\
						IP+=4;

#define EXEC_INT();		HANDLE_INT((U1)RAM[IP+1]);\
						IP+=2;

#define EXEC_DI();		INT0=FALSE;

#define EXEC_EI();		INT0=TRUE;

#define EXEC_HALT();	;

#define EXEC_NOP();		IP++;

// Bit Wise

#define EXEC_AND();		R[RAM[IP+1]]=R[RAM[IP+2]] & R[RAM[IP+3]];\
						IP+=4;

#define EXEC_OR();		R[RAM[IP+1]]=R[RAM[IP+2]] | R[RAM[IP+3]];\
						IP+=4;

#define EXEC_XOR();		R[RAM[IP+1]]=R[RAM[IP+2]] ^ R[RAM[IP+3]];\
						IP+=4;

#define EXEC_NOT();		R[RAM[IP+1]]=~R[RAM[IP+2]];\
						IP+=3;

// Bit Test
#define EXEC_BT();

// Bit Set
#define EXEC_BS();

// Shift

#define EXEC_SRA();		R[RAM[IP+1]]=((S8)R[RAM[IP+2]]) >> ((S8)R[RAM[IP+3]]);\
						IP+=4;

#define EXEC_SRL();		R[RAM[IP+1]]=R[RAM[IP+2]] >> R[RAM[IP+3]];\
						IP+=4;	

#define EXEC_SL();		R[RAM[IP+1]]=R[RAM[IP+2]] << R[RAM[IP+3]];\
						IP+=4;

// Integer Math

#define EXEC_ADD();		R[RAM[IP+1]]=((S8)R[RAM[IP+2]]) + ((S8)R[RAM[IP+3]]);\
						IP+=4;

#define EXEC_SUB();		R[RAM[IP+1]]=((S8)R[RAM[IP+2]]) - ((S8)R[RAM[IP+3]]);\
						IP+=4;

#define EXEC_MUL();		R[RAM[IP+1]]=((S8)R[RAM[IP+2]]) * ((S8)R[RAM[IP+3]]);\
						IP+=4;

#define EXEC_DIV();		R[RAM[IP+1]]=((S8)R[RAM[IP+3]]) / ((S8)R[RAM[IP+4]]);\
						R[RAM[IP+2]]=((S8)R[RAM[IP+3]]) % ((S8)R[RAM[IP+4]]);\
						IP+=5;