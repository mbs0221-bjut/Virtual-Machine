#include "parser.h"

void Parser::parseBlocks()
{
	while (s->kind == BASIC) {
		parseDefinition();
	}
}

void Parser::parseDefinition()
{
	Type *type = (Type*)s;
	match(BASIC);
	Word *word = (Word*)s;
	match(ID);
	if (word->kind != '(') {
		top_scope[word->word] = type;
		while (s->kind == ',') {
			match(',');
			top_scope[word->word] = type;
			match(ID);
		}
		match(';');
		return;
	}
	FunctionAST *f = parseFunction(word->word, type);
}

PrototypeAST * Parser::parsePrototype(string name, Type * type)
{
	vector<ParameterAST*> args;
	match('(');
	int offset = 0;
	if (s->kind == BASIC) {
		Type *type = (Type*)s;
		match(BASIC);
		args.push_back(new ParameterAST(type, ((Word*)s)->word));
		match(ID);
		while (s->kind == ',') {
			match(',');
			match(BASIC);
			args.push_back(new ParameterAST(type, ((Word*)s)->word));
			match(ID);
		}
	}
	match(')');
	return new PrototypeAST(type, name, args);
}

FunctionAST * Parser::parseFunction(string name, Type * type)
{

	PrototypeAST *proto = parsePrototype(name, type);
	if (proto == nullptr) return 0;

	if (Stmt *body = parseBlock())
		return new FunctionAST(proto, body);

	return 0;
}

Stmt * Parser::parseStmt()
{
	Stmt *st = nullptr;
	// Óï·¨·ÖÎö
	switch (s->kind) {
	case BASIC:st = parseDeclaration(); break;
	case ID:parseExpression(); break;
	case IF:st = parseIfElse(); break;
	case WHILE:st = parseWhileDo(); break;
	case DO:st = parseDoWhile(); break;
	case FOR:st = parseFor(); break;
	case CASE:st = parseSwitch(); break;
	case BREAK:st = parseBreak(); break;
	case CONTINUE:st = parseContinue(); break;
	case TRY:st = parseTryCatch(); break;
	case ';':match(';'); break;
	case '{':st = parseBlock(); break;
	default:match(s->kind); break;
	}
	return st;
}

Stmt * Parser::parseBlock()
{
	vector<Stmt*> block;
	match('{');
	while (s->kind != '}') {
		Stmt *st = parseStmt();
		if (st) {
			block.push_back(st);
		}
	}
	match('}');
	return new Block(block);
}

Stmt * Parser::parseDeclaration()
{
	Type *type = (Type*)s;
	match(BASIC);
	top_scope[((Word*)s)->word] = type;
	match(ID);
	while (s->kind == ',') {
		match(',');
		top_scope[((Word*)s)->word] = type;
		match(ID);
	}
	match(';');
}

Stmt * Parser::parseIfElse()
{
	match(IF);
	match('(');
	ExprAST *cond = parseExpression();
	match(')');
	Stmt *body_t = parseStmt();
	if (s->kind == ELSE) {
		match(ELSE);
		Stmt *body_f = parseStmt();
		return new IfElse(cond, body_t, body_f);
	}
	return new IfElse(cond, body_t, nullptr);
}

Stmt * Parser::parseWhileDo()
{
	match(WHILE);
	match('(');
	ExprAST *cond = parseExpression();
	match(')');
	Stmt *body = parseStmt();
	return new WhileDo(cond, body);
}

Stmt * Parser::parseDoWhile()
{
	match(DO);
	Stmt *body = parseStmt();
	blocks.pop();
	match(WHILE);
	match('(');
	ExprAST *cond = parseExpression();
	match(')');
	match(';');
	return new DoWhile(cond, body);
}

Stmt * Parser::parseFor()
{
	match(FOR);
	match('(');
	ExprAST *init = parseExpression();
	match(';');
	ExprAST *cond = parseExpression();
	match(';');
	ExprAST *step = parseExpression();
	match(')');
	Stmt *body = parseStmt();
	return new For(init, cond, step, body);
}

Stmt * Parser::parseSwitch()
{
	vector<Case> cases;
	match(SWITCH);
	match('(');
	ExprAST *expr = parseExpression();
	match(')');
	match(CASE);
	while (s->kind == CASE) {
		Integer *i = (Integer*)s;
		match(INT);
		match(':');
		cases.push_back(Case(i->value, parseStmt()));
	}
	match(END);
	return new Switch(expr, cases);
}

Stmt * Parser::parseBreak()
{
	Break *st = new Break(blocks.top());
	match(BREAK);
	match(';');
	return st;
}

Stmt * Parser::parseContinue()
{
	Continue *st = new Continue(blocks.top());
	st->line = lexer->line;
	match(CONTINUE);
	match(';');
	return st;
}

Stmt * Parser::parseThrow()
{
	match(THROW);
}

Stmt * Parser::parseTryCatch()
{
	match(TRY);
	Stmt* pTry = parseStmt();
	match(CATCH);
	Stmt* pCatch = parseStmt();
	match(FINALLY);
	Stmt* pFinnaly = parseStmt();
	return new TryCatch(pTry, pCatch, pFinnaly);
}

ExprAST * Parser::parseExpression()
{
	ExprAST *lhs = parsePrimary();
	if (!lhs) return 0;

	return parseBinaryExpr(0, lhs);
}

ExprAST * Parser::parseBinaryExpr(int ExprPrec, ExprAST * lhs)
{
	// If this is a binop, find its precedence.
	while (true) {
		int TokPrec = GetTokPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return lhs;

		// Okay, we know this is a binop.
		int opt = s->kind;
		match(s->kind);

		// Parse the primary expression after the binary operator.
		ExprAST *rhs = parsePrimary();
		if (!rhs) return 0;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = GetTokPrecedence();
		if (TokPrec < NextPrec) {
			rhs = parseBinaryExpr(TokPrec + 1, rhs);
			if (!rhs) return 0;
		}

		// Merge LHS/RHS.
		lhs = new BinaryExprAST(opt, lhs, rhs);
	}
}

ExprAST * Parser::parsePrimary()
{
	// primary ::= id | num | unary
	switch (s->kind) {
	case ID: return parseIdentifierExpr();
	case NUM: return parseConstantExpr();
	case '(': return parseBracketsExpr();
	case INC:
	case DEC:
	case '!':
	case '~':
	case '-': return parseUnaryExpr();
	default: return 0;
	}
}

ExprAST * Parser::parseIdentifierExpr()
{
	// id ::= id | assign | call
	string name = ((Word*)s)->word;
	match(ID);
	// assign
	if (s->kind == '=') {
		match('=');
		ExprAST *expr = parseExpression();
		return new AssignExprAST(name, expr);
	}
	// call
	if (s->kind == '(') {
		match('(');
		vector<ExprAST*> args;
		if (s->kind != ')') {
			while (true) {
				ExprAST *arg = parseExpression();
				if (arg == nullptr)
					return 0;
				args.push_back(arg);

				if (s->kind == ')')
					break;

				if (s->kind != ',')
					return 0;
				match(',');
			}
			match(')');
			return new CallExprAST(name, args);
		}
	}
	return new VariableExprAST(name);
}

ExprAST * Parser::parseConstantExpr()
{
	ExprAST *Result = new ConstantExprAST((Integer*)s);
	match(NUM);
	return Result;
}

ExprAST * Parser::parseBracketsExpr()
{
	match('(');
	ExprAST *e = parseExpression();
	if (!e) return 0;
	if (s->kind != ')') return 0;
	match(')');
	return e;
}

ExprAST * Parser::parseUnaryExpr()
{
	int opt = s->kind;
	match(opt);
	return new UnaryExprAST(opt, parseExpression());
}
