#include "vm.h"

void CPU::init(){
	DS = CS = BP = IP = 0;
	SS = 0xffff;// 栈基址
	SP = SS;// 栈指针
}
void CPU::load(string fp){
	WORD LENGTH = 0;
	ifstream fout(fp);
	fout.read((char*)&DS, sizeof(WORD));
	fout.read((char*)&CS, sizeof(WORD));
	fout.read((char*)&LENGTH, sizeof(WORD));
	for (int i = 0; i < LENGTH; i++){
		fout.read((char*)&RAM[i], sizeof(WORD));
	}
	fout.close();
	IP += CS;
}
void CPU::store(){
}
void CPU::execute(){
	WORD ABUS, DBUS;
	BYTE TYPE, MR;
	BYTE OP = RAM[IP++];
	while (OP != HALT&&IP < 80){
		TYPE = OP & MR_BYTE;
		MR = OP & MR_B;
		OP &= (~MR_BYTE);
		OP &= (~MR_B);
		switch (OP){
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case MOD:
		case CMP:
			ALU.OP = OP;
			if (TYPE == MR_BYTE){
				ABUS = ReadB();
				ALU.RA ^= ALU.RA;
				ALU.RA |= REG[ABUS];
				ABUS = ReadB();
				ALU.RB ^= ALU.RB;
				ALU.RB |= REG[ABUS];// 低位
				ALU.execute();
				ABUS = ReadB();
				REG[ABUS] = ALU.R;// 低位
				cout << "BYTE:" << (int)ALU.R << "=";
				cout << (int)ALU.RA << " OP " << (int)ALU.RB << endl;
			}else{
				ABUS = ReadB();
				ABUS <<= 1;
				cout << "ABUS:" << ABUS << ",";
				ALU.RA ^= ALU.RA;
				ALU.RA |= REG[ABUS] << 8;// 高位
				ALU.RA |= REG[ABUS + 1];// 低位
				ABUS = ReadB();
				ABUS <<= 1;
				cout << "ABUS:" << ABUS << ",";
				ALU.RB ^= ALU.RB;
				ALU.RB |= REG[ABUS] << 8;// 高位
				ALU.RB |= REG[ABUS + 1];// 低位
				ALU.execute();
				ABUS = ReadB();
				ABUS <<= 1;
				cout << "ABUS:" << ABUS << endl;
				REG[ABUS] = ALU.R >> 8;// 高位
				REG[ABUS + 1] = ALU.R;// 地位
				cout << "WORD:" << ALU.R << "=";
				cout << ALU.RA << " OP " << ALU.RB << endl;
			}
			break;
		case JE:
			if (ALU.FR & BIT_EQ){
				IP = ReadW();
				cout << "JE " << dec << (int)IP << endl;
			}else{
				IP++; IP++;
			}
			break;
		case JNE:
			if (ALU.FR & BIT_EQ){
				IP++; IP++;
			}else{
				IP = ReadW();
				cout << "JNE " << dec << (int)IP << endl;
			}
			break;
		case PUSH:// REG[X]->RAM[SP]
			ABUS = ReadB();
			if (TYPE == MR_BYTE){
				printf("pushb ABUS=%02d SP=%04d\n", ABUS, SP);
				RAM[SP--] = REG[ABUS];
			}else{
				printf("pushw ABUS=%02d SP=%04d\n", ABUS, SP);
				ABUS <<= 1;
				RAM[SP--] = REG[ABUS + 1];
				RAM[SP--] = REG[ABUS];
			}
			break;
		case POP:// RAM[SP]->REG[X]
			ABUS = ReadB();
			if (TYPE == MR_BYTE){
				printf("popb ABUS=%02d SP=%04d\n", ABUS, SP);
				REG[ABUS] = RAM[SP++];
			}else{
				printf("popw ABUS=%02d SP=%04d\n", ABUS, SP);
				ABUS <<= 1;
				REG[ABUS] = RAM[SP++];
				REG[ABUS + 1] = RAM[SP++];
			}
			break;
		case LOAD:// RAM->REG
			ABUS = ReadB();
			if (TYPE == MR_BYTE){
				switch (MR){// 01100000
				case MR_A:DBUS = ReadB(); break;
				case MR_B:DBUS = ReadB(ReadB());  break;
				default:printf("ERROR %d\n", OP); break;
				}
				REG[ABUS] = DBUS;
				printf("loadb %02d %02d\n", ABUS, DBUS);
			}else{
				switch (MR){
				case MR_A:DBUS = ReadW(); break;
				case MR_B:DBUS = ReadW(ReadW()); break;
				default:printf("ERROR %d\n", OP); break;
				}
				ABUS <<= 1;
				REG[ABUS] = DBUS >> 8;
				REG[ABUS + 1] = DBUS;
				printf("loadw %04d %04d\n", ABUS, DBUS);
			}
			break;
		case STORE:// REG->RAM
			ABUS = ReadB();
			if (TYPE == MR_BYTE){
				DBUS = REG[ABUS];
				switch (MR){
				case MR_B:ABUS = ReadW(); printf("STORE MR_B WORD "); break;
				default: printf("error storew"); break;
				}
				RAM[ABUS] = DBUS;
				printf("%02d %02d\n", ABUS, DBUS);
			}else{
				ABUS <<= 1;
				DBUS ^= DBUS;
				DBUS |= REG[ABUS] << 8;
				DBUS |= REG[ABUS + 1];
				switch (MR){
				case MR_B:ABUS = ReadW(); printf("STORE MR_B WORD "); break;
				default: printf("error storew"); break;
				}
				ABUS <<= 1;
				RAM[ABUS] = DBUS >> 8;
				RAM[ABUS + 1] = DBUS;
				printf("%04d %04d\n", ABUS, DBUS);
			}
			break;
		default:
			cout << "invalid OP=" << OP << " at IP=" << IP << endl;
			break;
		}
		OP = RAM[IP++];
		trace();
	}
}
void CPU::trace(){
	char a;
	cin >> a;
	cout << "[" << (int)RAM[0];
	for (int i = 1; i < 12; i++){
		cout << ' ';
		cout << (int)RAM[i];
	}
	cout << "]" << endl;
	cout << "[";
	for (int i = SS; i > SP; i--){
		cout << ' ';
		cout << (int)RAM[i];
	}
	cout << "]" << endl;
	for (int i = 0; i < 8; i++){
		cout << (int)REG[i * 8 + 0];
		for (int j = 1; j < 32; j++){
			cout << ' ';
			cout << (int)REG[i * 8 + j];
		}
		cout << endl;
	}
}