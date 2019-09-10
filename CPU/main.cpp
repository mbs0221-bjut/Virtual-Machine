#include "vm.h"

void main()
{
	char a;
	FILE file;
	FILE *fp = &file;
	// 虚拟机执行
	printf("虚拟机执行\n");
	CPU CPU;
	CPU.init();
	CPU.load("data.obj");
	CPU.execute();
	CPU.store();
	cout << "exit." << endl;
	cin >> a;
}