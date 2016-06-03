#include "parser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	// 编译生成中间代码
	printf("编译开始\n");
	Parser p("Text.txt");
	Stmt *st = p.parse();
	printf(" line  stmt\n");
	fopen_s(&fp, "data.asm", "w");
	st->code(fp);
	fprintf(fp, "halt\n");
	fclose(fp);
	printf("编译结束\n");
	cin >> a;
}