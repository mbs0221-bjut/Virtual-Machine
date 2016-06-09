#ifndef __LEXER_H_
#define __LEXER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

enum Tag{
	IF = 256, THEN, ELSE, DO, WHILE, FOR, CASE, BREAK, CONTINUE, 
	BASIC, INT, CHAR, 
	END,
	ID, NUM, FUNCTION
};

// 词法单元
struct Token{
	int	kind;
	Token(int tag) :kind(tag){  }
	virtual string place(){
		ostringstream s;
		s << kind;
		return s.str();
	}
	virtual string code(){
		ostringstream s;
		switch (kind){
		case IF:s << "IF"; break;
		case THEN:s << "THEN"; break;
		case ELSE:s << "ELSE"; break;
		case DO:s << "DO"; break;
		case WHILE:s << "WHILE"; break;
		case FOR:s << "FOR"; break;
		case CASE:s << "CASE"; break;
		case BREAK:s << "BREAK"; break;
		case CONTINUE:s << "CONTINUE"; break;
		case ID:s << "ID"; break;
		case BASIC:s << "BASIC"; break;
		case INT:s << "INT"; break;
		case CHAR:s << "CHAR"; break;
		case END:s << "END"; break;
		default:s << (char)kind; break;
		}
		return s.str();
	}
};

struct Word :Token{
	string word;
	Word(int tag, string word) :Token(tag), word(word) {  }
	virtual string place(){
		ostringstream s;
		s << word;
		return s.str();
	}
	virtual string code(){
		return word;
	}
};

struct Type :Word{
	int width;
	static Type *Int, *Char, *Void;
	Type(int kind, string word, int width) :Word(kind, word), width(width){  }
	virtual string place(){
		ostringstream s;
		s << word << ":" << width;
		return s.str();
	}
	virtual string code(){
		return word;
	}
};

Type* Type::Int = new Type(BASIC, "int", 2);
Type* Type::Char = new Type(BASIC, "char", 1);
Type* Type::Void = new Type(BASIC, "void", 0);

struct Integer :Token{
	int value;
	Integer(int tag, int value) :Token(tag), value(value) {  }
	virtual string place(){
		ostringstream s;
		s << value;
		return s.str();
	}
	virtual string code(){
		return "INT";
	}
};

// 词法分析器
class Lexer{
	ifstream inf;
	map<string, Token*> words;
public:
	int line = 1;
	Lexer(string fp){
		words["int"] = Type::Int;
		words["char"] = Type::Char;
		words["void"] = Type::Void;
		words["if"] = new Word(IF, "if");
		words["then"] = new Word(THEN, "then");
		words["else"] = new Word(ELSE, "else");
		words["do"] = new Word(DO, "do");
		words["while"] = new Word(WHILE, "while");
		words["for"] = new Word(FOR, "for");
		words["case"] = new Word(CASE, "case");
		words["break"] = new Word(BREAK, "break");
		words["continue"] = new Word(CONTINUE, "continue");
		words["end"] = new Word(END, "end");
		inf.open(fp, ios::in);
	}
	~Lexer(){
		inf.close();
		words.clear();
		printf("~Lexer\n");
	}
	Token *scan()
	{
		int i = 0;
		char ch;
		do{
			inf.read(&ch, sizeof(ch));
			if (ch == '\n')line++;
		} while (ch == ' ' || ch == '\n' || ch == '\t');
		if (ch == EOF){
			return new Token(END);
		}
		if (isalpha(ch)){
			string str;
			do{
				str.push_back(ch);
				inf.read(&ch, sizeof(ch));
			} while (isalnum(ch));  //1状态
			inf.seekg(-1, ios::cur);//回退一个字符
			if (words.find(str) == words.end()){
				return new Word(ID, str);
			}
			return words[str];
		}
		if (isdigit(ch)){
			int value = 0;
			if (ch == '0'){
				inf.read(&ch, sizeof(ch));
				if (ch == 'x' || ch == 'X'){
					inf.read(&ch, sizeof(ch));
					if (isdigit(ch) || (ch >= 'a'&&ch <= 'f') || (ch >= 'A'&&ch <= 'F')){
						do{
							if (isalpha(ch)){
								value = 16 * value + ch - 'A' + 10;
							}
							else{
								value = 16 * value + ch - '0';
							}
							inf.read(&ch, sizeof(ch));
						} while (isdigit(ch) || (ch >= 'a'&&ch <= 'f') || (ch >= 'A'&&ch <= 'F'));
						inf.seekg(-1, ios::cur);
						return new Integer(NUM, value);
					}
					else{
						printf("错误的十六进制!");
					}
				}
				else if (ch >= '0'&&ch <= '7'){
					//八进制整数
					do{
						value = 8 * value + ch - '0';
						inf.read(&ch, sizeof(ch));
					} while (ch >= '0'&&ch <= '7');
					inf.seekg(-1, ios::cur);
					return new Integer(NUM, value);
				}
				else{
					//十进制整数0
					inf.seekg(-1, ios::cur);
					return new Integer(NUM, 0);
				}
			}
			else{
				//除0外十进制整数,5状态
				do{
					value = 10 * value + ch - '0';
					inf.read(&ch, sizeof(ch));
				} while (isdigit(ch));
				inf.seekg(-1, ios::cur);//回退一个字符
				return new Integer(NUM, value);
			}
		}
		return new Token(ch);
	}
};

#endif