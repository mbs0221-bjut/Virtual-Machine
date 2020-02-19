#ifndef __PARSER_H_
#define __PARSER_H_

#include "inter.h"

using namespace std;

class Parser{
private:
	Token *s;
	Lexer *lexer;
	stack<Stmt*> blocks;
	stack<SymbolTable> symbols;
	SymbolTable top_scope, global;
	bool match(int kind){
		if (s->kind == kind){
			s = lexer->scan();
			return true;
		}
		s = lexer->scan();
		printf("%s not matched.\n", kind);
		return false;
	}
	void getNextToken() {
		s = lexer->scan();
	}
	map<string, int> precedence;
	void init_precedence() {
		precedence["[]"] = 13;
		precedence["()"] = 13;
		precedence["->"] = 12;
		precedence["."] = 12;

		precedence["!"] = 12;
		precedence["~"] = 12;
		precedence["++"] = 12;
		precedence["--"] = 12;
		precedence["-"] = 12;

		precedence["sizeof"] = 11;

		precedence["*"] = 10;
		precedence["/"] = 10;
		precedence["%"] = 10;

		precedence["+"] = 9;
		precedence["-"] = 9;

		precedence["<<"] = 8;
		precedence[">>"] = 8;

		precedence["<"] = 7;
		precedence["<="] = 7;
		precedence[">="] = 7;
		precedence[">"] = 7;

		precedence["=="] = 6;
		precedence["!="] = 6;
		precedence["&"] = 5;
		precedence["~"] = 4;
		precedence["|"] = 3;
		precedence["&&"] = 2;
		precedence["||"] = 1;
	}
	int GetTokPrecedence() {
		return precedence[((Word*)s)->str];
	}
	bool compare(string &opa, string &opb) {
		return precedence[opa] > precedence[opb];
	}
protected:
	// 外部结构
	void parseBlocks();
	void parseDefinition();
	PrototypeAST* parsePrototype(string name, Type *type);
	FunctionAST* parseFunction(string name, Type *type);
	FunctionAST *parseTopLevelExpr();
	Stmt* parseStmt();
	Stmt* parseBlock();
	Stmt* parseDeclaration();
	Stmt* parseIfElse();
	Stmt* parseWhileDo();
	Stmt* parseDoWhile();
	Stmt* parseFor();
	Stmt* parseSwitch();
	Stmt* parseBreak();
	Stmt* parseContinue();
	Stmt* parseThrow();
	Stmt* parseTryCatch();
	ExprAST* parseExpression();
	ExprAST* parseBinaryExpr(int ExprPrec, ExprAST *lhs);
	ExprAST* parsePrimary();
	ExprAST* parseIdentifierExpr();
	ExprAST* parseConstantExpr();
	ExprAST* parseBracketsExpr();
	ExprAST* parseUnaryExpr();
public:
	Parser(){
		init_precedence();
		lexer = new Lexer();
	}
	~Parser(){
		printf("~Parser\n");
		precedence.clear();
		delete lexer;
	}
	AST* parse(char *filename){
		lexer->open(filename);
		s = lexer->scan();// 预读一个词法单元，以便启动语法分析
		parseBlocks();
	}
};

#endif