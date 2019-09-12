#ifndef __PARSER_H_
#define __PARSER_H_

#include "inter.h"

using namespace std;

class Parser{
private:
	Token *s;
	Lexer *lexer;
	// 符号表
	Symbols *symbols;// 局部符号表
	Global *globol;// 全局符号表
	// 匹配词法单元
	bool match(int kind){
		if (s->kind == kind){
			s = lexer->scan();
			return true;
		}
		s = lexer->scan();
		printf("%s not matched.\n", kind);
		return false;
	}
	map<string, int> precedence;
	// 符号表管理
	Id* getId(){
		list<Id*>::iterator iter;
		Symbols *table = nullptr;
		for (table = symbols; table != nullptr; table = table->prev){
			for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
				if ((*iter)->s->word == ((Word*)s)->word){
					return (*iter);
				}
			}
		}
		printf("[%d] %s undeclared.\n", lexer->line, ((Word*)s)->word.c_str());
		return nullptr;
	}
	void init_precedence() {
		precedence["[]"] = 1; // level 1
		precedence["()"] = 1;
		precedence["->"] = 1;
		precedence["."] = 1;

		precedence["!"] = 2; // level 2
		precedence["~"] = 2;
		precedence["++"] = 2;
		precedence["--"] = 2;
		precedence["-"] = 2;
		//precedence["(T)"] = 2;
		//precedence["*"] = 2;
		//precedence["&"] = 2;
		precedence["sizeof"] = 2;

		precedence["*"] = 3; // level 3
		precedence["/"] = 3;
		precedence["%"] = 3;

		precedence["+"] = 4; // level 4
		precedence["-"] = 4;

		precedence["<<"] = 5; // level 5
		precedence[">>"] = 5;

		precedence["<"] = 6; // level 6
		precedence["<="] = 6;
		precedence[">="] = 6;
		precedence[">"] = 6;

		precedence["=="] = 7; // level 7
		precedence["!="] = 7;

		precedence["&"] = 8; // bit logic
		precedence["~"] = 3;
		precedence["|"] = 10;

		precedence["&&"] = 11; // logical
		precedence["||"] = 12;
	}
	bool compare(string &opa, string &opb) {
		return precedence[opa] > precedence[opb];
	}
protected:
	// 外部结构
	void node(){
		while (s->kind == BASIC){
			node_decl();
		}
	}
	void node_decl(){
		Type *type = (Type*)s;
		match(BASIC);
		Token *token = s;
		match(ID);
		if (s->kind == '('){
			// 函数体分析
			Function *f = node_func(((Word*)token)->word, type);
			globol->funcs.push_back(f);
		}
		else{
			Id *id = new Id(type, (Word*)token);
			symbols->ids.push_back(id);
			globol->ids.push_back(id);
			while (s->kind == ','){
				match(',');
				id = new Id(type, (Word*)s);
				symbols->ids.push_back(id);
				globol->ids.push_back(id);
				match(ID);
			}
			match(';');
		}
		return;
	}
	Function* node_func(string name, Type *type){
		Function *f = new Function(name, type);
		symbols->ids.push_back(new Id(type, f));
		// 符号表入栈
		f->params->prev = symbols;
		symbols = f->params;
		match('(');
		int offset = 0;
		if (s->kind == BASIC){
			Type *type = (Type*)s;
			match(BASIC);
			symbols->ids.push_back(new Id(type, (Word*)s));
			match(ID);
			while (s->kind == ','){
				match(',');
				match(BASIC);
				symbols->ids.push_back(new Id(type, (Word*)s));
				match(ID);
			}
		}
		match(')');
		f->symbols->prev = symbols;
		symbols = f->symbols;
		f->body = stmts();
		// 符号表出栈
		symbols = symbols->prev;
		symbols = symbols->prev;
		return f;
	}
	// 语法分析子程序
	Stmt* stmt()
	{
		Stmt *st = nullptr;
		// 语法分析
		switch (s->kind){
		case BASIC:st = stmt_decl(); break;
		case ID:
		{
				   switch (getId()->s->kind){
				   case ID:st = stmt_assign(); break;
				   case FUNCTION:st = expr_call(); match(';'); break;
				   default:printf("unknown id.\n"); break;
				   }
				   break;
		}
		case IF:st = stmt_if(); break;
		case WHILE:st = stmt_while(); break;
		case DO:st = stmt_do(); break;
		case FOR:st = stmt_for(); break;
		case CASE:st = stmt_case(); break;
		case BREAK:st = stmt_break(); break;
		case CONTINUE:st = stmt_continue(); break;
		case TRY:st = stmt_try(); break;
		case ';':match(';'); break;
		case '{':st = stmts(); break;
		default:match(s->kind); break;
		}
		return st;
	}
	Stmt* stmts(){
		Stmts *sts = new Stmts;
		// 符号表入栈
		Symbols *table = new Symbols;
		symbols->next = table;
		table->prev = symbols;
		symbols = table;
		// 语法分析
		sts->line = lexer->line;
		match('{');
		while (s->kind != '}'){
			Stmt *st = stmt();
			if (st)sts->Ss.push_back(st);
		}
		match('}');
		// 符号表出栈
		symbols = symbols->prev;
		return sts;
	}
	// 定义
	Stmt* stmt_decl(){
		Decl *d = new Decl;
		d->line = lexer->line;
		Type *type = (Type*)s;
		match(BASIC);
		symbols->ids.push_back(new Id(type, (Word*)s));
		match(ID);
		while (s->kind == ','){
			match(',');
			symbols->ids.push_back(new Id(type, (Word*)s));
			match(ID);
		}
		match(';');
		return d;
	}
	// 赋值
	Stmt* stmt_assign(){
		Assign *a = new Assign;
		a->line = lexer->line;
		a->E1 = getId();
		match(ID);
		match('=');
		a->E2 = expr_expr();
		return a;
	}
	// 条件语句
	Stmt* stmt_if(){
		If *i = new If;
		i->line = lexer->line;
		match(IF);
		match('(');
		i->C = expr_cond();
		match(')');
		i->S1 = stmt();
		if (s->kind == ELSE){
			Else *e = new Else;
			match(ELSE);
			e->line = i->line;
			e->C = i->C;
			e->S1 = i->S1;
			e->S2 = stmt();
			return e;
		}
		return i;
	}
	// 循环语句
	Stmt* stmt_while(){
		While *w = new While;
		w->line = lexer->line;
		match(WHILE);
		match('(');
		w->C = expr_cond();
		match(')');
		Break::cur.push(w);
		Continue::cur.push(w);// continue 跳转到语句开头执行
		w->S1 = stmt();
		Continue::cur.pop();
		Break::cur.pop();
		return w;
	}
	Stmt* stmt_do(){
		Do *d = new Do;
		d->line = lexer->line;
		match(DO);
		Break::cur.push(d);
		Continue::cur.push(d);// continue 跳转到语句开头执行
		d->S1 = stmt();
		Continue::cur.pop();
		Break::cur.pop();
		match(WHILE);
		match('(');
		d->C = expr_cond();
		match(')');
		match(';');
		return d;
	}
	Stmt* stmt_for(){
		For *f = new For;
		f->line = lexer->line;
		match(FOR);
		match('(');
		f->S1 = stmt_assign();
		match(';');
		f->C = expr_cond();
		match(';');
		f->S2 = stmt_assign();
		match(')');
		Break::cur.push(f);
		Continue::cur.push(f->S2);// continue直接跳转到第二条赋值语句开头执行
		f->S3 = stmt();
		Continue::cur.pop();
		Break::cur.pop();
		return f;
	}
	// 分支语句
	Stmt* stmt_case(){
		Case *c = new Case;
		c->line = lexer->line;
		match(CASE);
		c->E = expr_expr();
		while (s->kind != END){
			Integer *i = (Integer*)s;
			match(INT);
			match(':');
			c->Ss[i->value] = stmt();
		}
		match(END);
		return c;
	}
	Stmt* stmt_break(){
		Break *st = new Break();
		st->line = lexer->line;
		match(BREAK);
		match(';');
		return st;
	}
	Stmt* stmt_continue(){
		Continue *st = new Continue();
		st->line = lexer->line;
		match(CONTINUE);
		match(';');
		return st;
	}
	// 异常处理
	Stmt* stmt_throw() {
		match(THROW);
	}
	Stmt* stmt_try() {
		match(TRY);
		Stmt* pTry = stmt();
		match(CATCH);
		Stmt* pCatch = stmt();
		match(FINALLY);
		Stmt* pFinnaly = stmt();
		return new TryCatch(pTry, pCatch, pFinnaly);
	}
	// 表达式语句
	BinaryExpr* expr_binary() {
		Expr* pL = expr_binary();
		while (s->kind) {
			Word* w = (Word*)s;
			if (compare(w->word, pL->opt)) {
				Expr* pR = expr_binary();
			}
			pL = new BinaryExpr(w->word, pL, pR);
		}
		return pL;
	}
	UnaryExpr* expr_unary() {
		match('-');
	}
	UnaryExpr* expr_unary(){
		Expr *u;
		if (s->kind == '~'){
			match('~');
			u = new UnaryExpr("~", expr_unary());
		}
		else{
			u = expr_factor();
		}
		return u;
	}
	Expr* expr_factor()
	{
		Expr* e = nullptr;
		switch (s->kind){
		case '(': match('('); e = expr_binary(); match(')'); break;
		case ID: 			
		{
				   switch (getId()->s->kind){
				   case ID:e = getId(); match(ID); break;
				   case FUNCTION:e = expr_call(); break;
				   default:printf("unknown id.\n"); break;
				   }
				   break;
		}
		case NUM:  e = new Number((Integer*)s); match(NUM); break;
		default: printf("F->('%c')\n", s->kind); match(s->kind); break;
		}
		return e;
	}
	// 函数调用
	Expr* expr_call(){
		Call *c = new Call;
		c->line = lexer->line;
		c->func = (Function*)getId()->s;
		match(ID);
		// 函数调用
		match('(');
		while (s->kind == ID || s->kind == NUM){
			Expr *e = expr_binary();
			if (e)c->args.push_back(e);
			if (s->kind == ',') match(',');
		}
		match(')');
		return c;
	}
public:
	Parser(){
		init_precedence();
		lexer = new Lexer();
		symbols = new Symbols();
		globol = new Global();
	}
	~Parser(){
		printf("~Parser\n");
		precedence.clear();
		delete lexer;
		delete symbols;
		delete globol;
	}
	Node* parse(char *filename){
		lexer->open(filename);
		s = lexer->scan();// 预读一个词法单元，以便启动语法分析
		node();
		return globol;
	}
};

#endif