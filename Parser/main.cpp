#include "parser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// ���������м����
	printf("���������м����\n");
	Parser p("Text.txt");
	Stmt *st = p.parse();
	fopen_s(&fp, "data.asm", "w");
	st->code(fp);
	fprintf(fp, "halt\n");
	fclose(fp);
	cout << "parsing complete." << endl;
	cin >> a;
}