#include "asm.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// �������Ŀ�����
	printf("����Ŀ�����\n");
	Asm Asm("data.asm");
	Asm.parse();
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