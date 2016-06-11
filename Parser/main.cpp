#include "lrparser.h"

void main(){
	char a;
	FILE file;
	FILE *fp = &file;
	//Parser *p = new Parser("Text.txt");
	//printf("开始语法分析\n");
	//Node *st = p->parse();
	//printf("语法分析结束\n");
	//printf("编译开始\n");
	//printf(" line  stmt\n");
	//fopen_s(&fp, "data.s", "w");
	//st->code(fp);
	//fclose(fp);
	//printf("编译结束\n");
	//delete p;
	fopen_s(&fp, "G.txt", "r");
	printf("开始SLR(1)语法分析\n");
	parse(fp);
	printf("SLR(1)语法分析结束\n");
	fclose(fp);
	cin >> a;
}


