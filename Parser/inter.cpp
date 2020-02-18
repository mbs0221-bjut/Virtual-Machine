#include "inter.h"

void Stmt::Codegen(FILE * fp)
{
	ASTNode::Codegen(fp);
	printf("[%04d]", line);
}

void Block::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	//printf("stmts\n");
	//list<Stmt*>::iterator iter;
	//for (iter = Ss.begin(); iter != Ss.end(); iter++) {
	//	(*iter)->Codegen(fp);
	//}
}

void ExprAST::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
}

void BinaryExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	pL->Codegen(fp);
	pR->Codegen(fp);
}

void UnaryExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	E1->Codegen(fp);
}

void VariableExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	/*char *width = type == Type::Int ? "dw" : "b";
	if (global) {
		fprintf(fp, "\tload%s $%d %s\n", width, label, name->getName());
	}
	else {
		fprintf(fp, "\tload%s $%d %s\n", width, label, name->getName());
	}*/
}

void AssignExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	expr->Codegen(fp);
	/*if (id->global) {
	fprintf(fp, "\tstore %s $%d\n", id->getName(), expr->label);
	}
	else {
	fprintf(fp, "\tstore %s $%d\n", id->getName(), expr->label);
	}*/
}

void ConstantExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	char width = s->value > 256 ? 'w' : 'b';
	fprintf(fp, "\tload%c $%d %d\n", width, label, s->value);
}

void CallExprAST::Codegen(FILE * fp)
{
	ExprAST::Codegen(fp);
	printf("call\n");
	// 参数计算
	vector<ExprAST*>::iterator iter;
	for (ExprAST* expr : args) {
		expr->Codegen(fp);
	}
	// 传递参数
	reverse(args.begin(), args.end());
	for (ExprAST* expr : args) {
		fprintf(fp, "\tparam $%d\n", expr->label);
	}
	// 调用语句
	fprintf(fp, "\tcall %s\n", callee);
}

void Decl::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	/*fprintf(fp, "\tdata\n");
	list<VariableExprAST*>::iterator iter;
	for (iter = ids.begin(); iter != ids.end(); iter++) {
		printf("\t$%s [%d]\n", (*iter)->getName(), (*iter)->getWidth());
		fprintf(fp, "\t\t$%s [%d]\n", (*iter)->getName(), (*iter)->getWidth());
	}
	printf("data\n");
	fprintf(fp, "\tdata\n");*/
}

void IfElse::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	cond->Codegen(fp);
	body_t->next = newlabel();
	fprintf(fp, "jz L%d:\n", body_t->next);
	// True
	body_t->Codegen(fp);
	next = newlabel();
	fprintf(fp, "jmp L%d:\n", next); 
	fprintf(fp, "L%d:\n", body_t->next);
	// False
	if (body_f) {
		body_f->next = newlabel();
		body_f->Codegen(fp);
	}
	fprintf(fp, "L%d:\n", next);
}

void WhileDo::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	begin = newlabel();
	next = newlabel();
	body->next = begin;
	fprintf(fp, "L%d:\n", begin);
	cond->Codegen(fp);
	fprintf(fp, "jz L:\n", next);// False跳转
	body->Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", begin);
	fprintf(fp, "L%d:\n", next);
}

void DoWhile::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	begin = newlabel();
	body->next = begin;
	fprintf(fp, "L%d:\n", begin);
	body->Codegen(fp);
	cond->Codegen(fp);
	fprintf(fp, "jnz L%d:\n", begin);// True跳转
}

void For::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	begin = newlabel();
	next = newlabel();
	step->begin = newlabel();
	step->next = newlabel();
	body->next = begin;
	init->Codegen(fp);
	fprintf(fp, "L%d:\n", begin);
	cond->Codegen(fp);
	fprintf(fp, "L%d:\n", cond->label); // False跳转
	body->Codegen(fp);
	fprintf(fp, "L%d:\n", step->begin);
	step->Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", begin);
	fprintf(fp, "L%d:\n", next);
}

void Switch::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	expr->Codegen(fp);
	for (Case cs : cases) {
		fprintf(fp, "= $%d $%d\n", expr->label, cs.first);
		fprintf(fp, "jne L%d", cs.second->begin);
		cs.second->Codegen(fp);
		fprintf(fp, "L%d", cs.second->next);
	}
}

void Break::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", stmt->next);
}

void Continue::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", next->begin);
}

void Throw::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", exception->kind);
}

void TryCatch::Codegen(FILE * fp)
{
	Stmt::Codegen(fp);
	// 需要异常处理的程序
	pTry->Codegen(fp);
	fprintf(fp, "\tjmp L%d\n", pCatch->next);
	// 异常处理程序
	fprintf(fp, "L%d:\n", pCatch->begin);
	pCatch->Codegen(fp);
	fprintf(fp, "L%d:\n", pCatch->next);
	// 扫尾操作
	pFinally->Codegen(fp);
}

void FunctionAST::Codegen(FILE * fp)
{
	//fprintf(fp, "proc %s:\n", word.c_str());
	//list<Id*>::iterator iter;
	//int width = type->width;
	//for (iter = params->ids.begin(); iter != params->ids.end(); iter++){
	//	(*iter)->offset = width;
	//	(*iter)->global = false;
	//	fprintf(fp, "\t;%s @%d\n", (*iter)->getName(), width);
	//	width += (*iter)->getWidth();
	//}
	//width = 0;
	//Symbols *table = symbols;
	//for (table = symbols; table != nullptr; table = table->next){
	//	for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
	//		(*iter)->offset = width;
	//		(*iter)->global = false;
	//		fprintf(fp, "\t;%s @%d\n", (*iter)->getName(), width);
	//		width += (*iter)->getWidth();
	//	}
	//}
	//fprintf(fp, "\tsub $sp %d\n", width);
	//width = 0;
	//for (table = params; table != nullptr; table = table->next){
	//	for (iter = table->ids.begin(); iter != table->ids.end(); iter++){
	//		(*iter)->offset = width;
	//		(*iter)->global = false;
	//		width += (*iter)->getWidth();
	//	}
	//}
	//body->code(fp);
	//fprintf(fp, "endp\n");
}

void PrototypeAST::Codegen(FILE * fp)
{
}
