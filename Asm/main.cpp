#include "asm.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// 汇编生成目标代码
	Asm Asm("data.s");
	printf("语法分析开始\n");
	Asm.parse();
	printf("语法分析结束\n");
	printf("汇编开始\n");
	printf("line  width offset\n");
	fopen_s(&fp, "data.bin", "w");
	Asm.write(fp);
	fclose(fp);
	printf("汇编结束\n");
	// 虚拟机执行
	printf("虚拟机执行\n");
	//CPU CPU;
	//CPU.init();
	//fopen_s(&fp, "data.bin", "r");
	//CPU.load(fp);
	//fclose(fp);
	//CPU.execute();
	//CPU.store();
	printf("执行结束\n");
	cin >> a;
}