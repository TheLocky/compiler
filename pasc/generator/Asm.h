#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

enum AsmOperators { errOp = -1, add = 0, sub, mul, _div, push, pop, call, mov, ret, _xor, end };
enum AsmRegisters { errReg = -1, eax = 0, ebx, ecx, edx, esi, edi, esp, ebp };
enum AsmTypes { db = 0, dd };

class AsmOperand {
public:
	virtual string code() = 0;
};

class AsmRegister : public AsmOperand {
public:
	AsmRegisters reg;

	AsmRegister(AsmRegisters reg);
	string code();
};

class AsmConst : public AsmOperand {
public:
	virtual string code() = 0;
};

class AsmStringConst : public AsmConst {
public:
	string str;

	AsmStringConst(string str);
	string code();
};

class AsmIntConst : public AsmConst {
public:
	int val;

	AsmIntConst(int val);
	string code();
};

class AsmData {
public:
	string name;
	AsmTypes type;
	vector<AsmConst *> init;

	AsmData(string name, AsmTypes type, vector<AsmConst *> init);
	string code();
};

class AsmIdentificator : public AsmOperand {
public:
	string name;

	AsmIdentificator(string name);
	AsmIdentificator(AsmData *var);
	string code();
};

class AsmOffset : public AsmOperand {
public:
	string name;

	AsmOffset(AsmData *var);
	string code();
};

class AsmLabel : public AsmOperand {
public:
	string lbl;

	AsmLabel(string lbl);
	string code();
};

class AsmAdress : public AsmOperand {
public:
	AsmIdentificator *variable;
	AsmRegister base;
	AsmRegister index;
	int scale;
	int displasement;

	AsmAdress(AsmRegister base, AsmRegister index, int scale, int displasement);
	AsmAdress(AsmIdentificator *var, AsmRegister index, int scale, int displasement);
	string code();
};

class AsmCmd {
public:
	AsmOperators cmd;

	AsmCmd(AsmOperators cmd);
	virtual string code() = 0;
};

class AsmBinaryCmd : public AsmCmd {
public:
	AsmOperand *first;
	AsmOperand *two;

	AsmBinaryCmd(AsmOperators cmd, AsmOperand *first, AsmOperand *two);
	string code();
};

class AsmUnaryCmd : public AsmCmd {
public:
	AsmOperand *operand;

	AsmUnaryCmd(AsmOperators cmd, AsmOperand *operand);
	string code();
};

class AsmLblCmd : public AsmCmd {
public:
	AsmLabel *lbl;

	AsmLblCmd(AsmLabel *lbl);
	string code();
};

class AsmCode {
private:
	vector<AsmData *> dataSection;
	vector<AsmCmd *> codeSection;

public:
	AsmData *add(string name, AsmTypes type, string s1);
	AsmData *add(string name, AsmTypes type, string s1, string s2);
	AsmData *add(string name, AsmTypes type, string s1, string s2, string s3);
	AsmData *add(string name, AsmTypes type, int i1);
	AsmData *add(string name, AsmTypes type, int i1, int i2);
	AsmData *add(string name, AsmTypes type, int i1, int i2, int i3);
	AsmData *add(string name, AsmTypes type, string s1, int i1);

	void add(AsmOperators cmd, AsmOperand *first, AsmOperand *two);
	void add(AsmOperators cmd, AsmOperand *operand);
	void add(AsmOperators cmd, AsmRegisters reg1, AsmRegisters reg2);
	void add(AsmOperators cmd, AsmRegisters reg);
	void add(AsmOperators cmd, AsmRegisters reg, int i);
	void add(AsmOperators cmd, AsmData *first, AsmRegisters reg);
	void add(AsmOperators cmd, AsmRegisters reg, AsmData *two);
	void add(AsmOperators cmd, AsmData *var);
	void add(AsmOperators cmd, AsmLabel *lbl);
	void add(AsmOperators cmd, int i);
	void add(AsmOperators cmd, string s);
	
	void add(AsmLabel *lbl);

	string code();
};
