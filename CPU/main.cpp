#include "vm.h"

void main()
{
	char a;
	FILE file;
	FILE *fp = &file;
	// �����ִ��
	printf("�����ִ��\n");
	CPU CPU;
	CPU.init();
	CPU.load("data.obj");
	CPU.execute();
	CPU.store();
	cout << "exit." << endl;
	cin >> a;
}