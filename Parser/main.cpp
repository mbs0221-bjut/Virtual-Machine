#include "parser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// ���������м����
	printf("���뿪ʼ\n");
	Parser p("Text.txt");
	Stmt *st = p.parse();
	printf(" line  stmt\n");
	fopen_s(&fp, "data.asm", "w");
	st->code(fp);
	fprintf(fp, "halt\n");
	fclose(fp);
	printf("�������\n");
	cin >> a;
}