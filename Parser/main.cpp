#include "parser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	Parser *p = new Parser("Text.txt");
	printf("��ʼ�﷨����\n");
	Node *st = p->parse();
	printf("�﷨��������\n");
	printf("���뿪ʼ\n");
	printf(" line  stmt\n");
	fopen_s(&fp, "data.s", "w");
	st->code(fp);
	fclose(fp);
	printf("�������\n");
	delete p;
	cin >> a;
}


