#include "parser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	Parser *p = new Parser();
	printf("��ʼ�﷨����\n");
	AST *st = p->parse("Text.txt");
	printf("�﷨��������\n");
	printf("���뿪ʼ\n");
	printf(" line  stmt\n");
	fopen_s(&fp, "data.s", "w");
	st->Codegen();
	fclose(fp);
	printf("�������\n");
	delete p;
	//fopen_s(&fp, "G.txt", "r");
	//printf("��ʼSLR(1)�﷨����\n");
	//parse(fp);
	//printf("SLR(1)�﷨��������\n");
	//fclose(fp);
	//cin >> a;
}