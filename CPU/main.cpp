#include "vm.h"

void main()
{
	char a;
	FILE fp;
	FILE *fp = &fp;
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