#include <stdio.h>
#include <string>
#include <stack>
#include <iostream>

using namespace std;

#define acc 0
#define err 15

int Action[12][6] = {
	{ 5, err, err, 4, err, err },	// 0
	{ err, 6, err, err, err, acc },	//
	{ err, -2, 7, err, -2, -2 },	// 2 E->T.
	{ err, -4, -4, err, -4, -4 },	// 3 T->F.
	{ 5, err, err, 4, err, err },	// 4 
	{ err, -6, -6, err, -6, -6 },	// 5 F->id.
	{ 5, err, err, 4, err, err },	//
	{ 5, err, err, 4, err, err },	//
	{ err, 6, err, err, 11, err },	//
	{ err, -1, 7, err, -1, -1 },	// 9 E->E+T.
	{ err, -3, -3, err, -3, -3 },	// 10 T->T*F
	{ err, -5, -5, err, -5, -5 }	// 11 F->(E).
};

int GoTo[12][3] = {
	{ 1, 2, 3 },// 0
	{ err, err, err }, // 1
	{ err, err, err }, // 2
	{ err, err, err }, // 3
	{ 8, 2, 3 }, // 4
	{ err, err, err }, // 5
	{ err, 9, 3 }, // 6
	{ err, err, 10 }, // 7
	{ err, err, err }, // 8
	{ err, err, err }, // 9
	{ err, err, err }, // 10
	{ err, err, err } // 11
};

string str = "@+*()#";
string nt = "ETF";

struct Symbol{
	char group;
	string code;
};

int lookup(char c, int state){
	int index = str.find_first_of(c);
	printf("action:%d %c -> %d\n", state, c, Action[state][index]);
	return Action[state][index];
}

int go(char c, int state){
	int index = nt.find_first_of(c);
	printf("goto:%d %c -> %d\n", state, c, GoTo[state][index]);
	return GoTo[state][index];
}

void parse(FILE *fp){
	int state = 0, temp = 1;
	char c;
	stack<int> ss;
	stack<char> sts;
	ss.push(state);
	fread(&c, sizeof(char), 1, fp);
	while (temp){
		//cin >> a;
		state = ss.top();
		temp = lookup(c, state);
		if (temp > 0){
			// 移入
			ss.push(temp);
			sts.push(c);
			// IP指向下一个符号
			fread(&c, sizeof(char), 1, fp);
		}else if (temp < 0){
			// 规约
			switch (-temp){
			case 1://E->E+T.
				printf("E->E+T.\n");
				ss.pop();
				ss.pop();
				ss.pop();
				sts.pop();
				sts.pop();
				sts.pop();
				// reduce
				sts.push('E');
				break;
			case 2://E->T.
				printf("E->T.\n");
				ss.pop();
				sts.pop();
				// reduce
				sts.push('E');
				break;
			case 3://T->T*F.
				printf("T->T*F.\n");
				ss.pop();
				ss.pop();
				ss.pop();
				sts.pop();
				sts.pop();
				sts.pop();
				// reduce
				sts.push('T');
				break;
			case 4://T->F.
				printf("T->F.\n");
				ss.pop();
				sts.pop();
				// reduce
				sts.push('T');
				break;
			case 5://F->(E).
				printf("F->(E).\n");
				ss.pop();
				ss.pop();
				ss.pop();
				sts.pop();
				sts.pop();
				sts.pop();
				// reduce
				sts.push('F');
				break;
			case 6://F->id.
				printf("F->id.\n");
				ss.pop();
				sts.pop();
				// reduce
				sts.push('F');
				break;
			default://ERR
				printf("error action.\n");
				break;
			}
			// GoTo
			int r = go(sts.top(), ss.top());
			if (r != err){
				ss.push(r);
			}else {
				temp = 0;
				printf("error goto.\n");
			}
		}else{
			printf("pass.\n");
		}
	}
}