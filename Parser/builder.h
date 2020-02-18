#pragma once

#include<string>

using namespace std;

class Value {

};

class Builder {
public:
	Value *CreateFAdd(Value *L, Value *R, string name);
	Value *CreateFSub(Value *L, Value *R, string name);
	Value *CreateFMul(Value *L, Value *R, string name);
	Value *CreateFDiv(Value *L, Value *R, string name);
};