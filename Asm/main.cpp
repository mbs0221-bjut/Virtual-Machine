#include "asm.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// �������Ŀ�����
	Asm Asm("data.s");
	printf("�﷨������ʼ\n");
	Asm.parse();
	printf("�﷨��������\n");
	printf("��࿪ʼ\n");
	printf("line  width offset\n");
	fopen_s(&fp, "data.bin", "w");
	Asm.write(fp);
	fclose(fp);
	printf("������\n");
	// �����ִ��
	printf("�����ִ��\n");
	//CPU CPU;
	//CPU.init();
	//fopen_s(&fp, "data.bin", "r");
	//CPU.load(fp);
	//fclose(fp);
	//CPU.execute();
	//CPU.store();
	printf("ִ�н���\n");
	cin >> a;
}