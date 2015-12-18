#include "asm.h"

const string AsmOperatorsCode[] = { "add", "sub", "mul", "div", "push", "pop", "call", "mov", "ret", "xor", "end" };
const string AsmRegistersCode[] = { "eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp" };
const string AsmTypesCode[] = { "db", "dd" };

const string Head = ".686\n.model flat, stdcall\noption casemap : none\n\n";
const string Includes = "include /masm32/include/msvcrt.inc\ninclude /masm32/include/kernel32.inc\ninclude /masm32/include/windows.inc\n\n";
const string Libs = "includelib /masm32/lib/msvcrt.lib\nincludelib /masm32/lib/kernel32.lib\n\n";
const string Data = ".data\n";
const string Code = ".code\n";

const string Tab = "\t";
const string Quot = "\"";

AsmRegister::AsmRegister(AsmRegisters reg) : reg(reg) {}

string AsmRegister::code()
{
	return AsmRegistersCode[reg];
}

AsmStringConst::AsmStringConst(string str) : str(str) {}

string AsmStringConst::code()
{
	return Quot + str + Quot;
}

AsmIntConst::AsmIntConst(int val) : val(val) {}

string AsmIntConst::code()
{
	return std::to_string(val);
}

AsmData::AsmData(string name, AsmTypes type, vector<AsmConst*> init) : name(name), type(type), init(init) {}

string AsmData::code()
{
	string initial = "";
	int i;
	for (i = 0; i < (int)init.size() - 1; i++)
		initial += init[i]->code() + ",";
	initial += init[i]->code();
	return name + " " + AsmTypesCode[type] + " " + initial;
}

AsmIdentificator::AsmIdentificator(string name) : name(name) {}

AsmIdentificator::AsmIdentificator(AsmData * var) : name(var->name) {}

string AsmIdentificator::code()
{
	return name;
}

AsmOffset::AsmOffset(AsmData * var) : name(var->name) {}

string AsmOffset::code() {
	return string("offset ") + name;
}

AsmLabel::AsmLabel(string lbl) : lbl(lbl) {}

string AsmLabel::code() {
	return lbl;
}

AsmAdress::AsmAdress(AsmRegister base, AsmRegister index, int scale, int displasement) :
	base(base), index(index), scale(scale), displasement(displasement) {}

AsmAdress::AsmAdress(AsmIdentificator *var, AsmRegister index, int scale, int displasement) :
	variable(var), base(errReg), index(index), scale(scale), displasement(displasement) {}

string AsmAdress::code()
{
	string baseStr = base.reg == errReg ? variable->code() : base.code();
	string scaleStr = scale > 0 ? string("*") + std::to_string(scale) : "";
	string sign = displasement < 0 ? "" : "+";
	string displasementStr = displasement != 0 ? sign + std::to_string(displasement) : "";
	return string("[") + baseStr + "+" + index.code() + scaleStr + displasementStr + "]";
}

AsmCmd::AsmCmd(AsmOperators cmd) : cmd(cmd) {}

AsmBinaryCmd::AsmBinaryCmd(AsmOperators cmd, AsmOperand * first, AsmOperand * two) :
	AsmCmd(cmd), first(first), two(two) {}

string AsmBinaryCmd::code()
{
	return AsmOperatorsCode[cmd] + " " + first->code() + "," + two->code();
}

AsmUnaryCmd::AsmUnaryCmd(AsmOperators cmd, AsmOperand * operand) :
	AsmCmd(cmd), operand(operand) {}

string AsmUnaryCmd::code()
{
	return AsmOperatorsCode[cmd] + " " + operand->code();
}

AsmLblCmd::AsmLblCmd(AsmLabel *lbl) : AsmCmd(errOp), lbl(lbl) {}

string AsmLblCmd::code() {
	return lbl->lbl + ":";
}

AsmData *AsmCode::add(string name, AsmTypes type, string s1) {
	auto d = new AsmData(name, type, { new AsmStringConst(s1) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, string s1, string s2) {
	auto d = new AsmData(name, type, { new AsmStringConst(s1), new AsmStringConst(s2) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, string s1, string s2, string s3) {
	auto d = new AsmData(name, type, { new AsmStringConst(s1), new AsmStringConst(s2), new AsmStringConst(s3) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, int i1) {
	auto d = new AsmData(name, type, { new AsmIntConst(i1) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, int i1, int i2) {
	auto d = new AsmData(name, type, { new AsmIntConst(i1), new AsmIntConst(i2) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, int i1, int i2, int i3) {
	auto d = new AsmData(name, type, { new AsmIntConst(i1), new AsmIntConst(i2), new AsmIntConst(i3) });
	dataSection.push_back(d);
	return d;
}

AsmData *AsmCode::add(string name, AsmTypes type, string s1, int i1) {
	auto d = new AsmData(name, type, { new AsmStringConst(s1), new AsmIntConst(i1) });
	dataSection.push_back(d);
	return d;
}

void AsmCode::add(AsmOperators cmd, AsmOperand * first, AsmOperand * two) {
	codeSection.push_back(new AsmBinaryCmd(cmd, first, two));
}

void AsmCode::add(AsmOperators cmd, AsmOperand * operand) {
	codeSection.push_back(new AsmUnaryCmd(cmd, operand));
}

void AsmCode::add(AsmOperators cmd, AsmRegisters reg1, AsmRegisters reg2){
	codeSection.push_back(new AsmBinaryCmd(cmd, new AsmRegister(reg1), new AsmRegister(reg2)));
}

void AsmCode::add(AsmOperators cmd, AsmRegisters reg) {
	codeSection.push_back(new AsmUnaryCmd(cmd, new AsmRegister(reg)));
}

void AsmCode::add(AsmOperators cmd, AsmRegisters reg, int i){
	codeSection.push_back(new AsmBinaryCmd(cmd, new AsmRegister(reg), new AsmIntConst(i)));
}

void AsmCode::add(AsmOperators cmd, AsmData * first, AsmRegisters reg) {
	codeSection.push_back(new AsmBinaryCmd(cmd, new AsmIdentificator(first), new AsmRegister(reg)));
}

void AsmCode::add(AsmOperators cmd, AsmRegisters reg, AsmData * two) {
	codeSection.push_back(new AsmBinaryCmd(cmd, new AsmRegister(reg), new AsmIdentificator(two)));
}

void AsmCode::add(AsmOperators cmd, AsmData * var) {
	codeSection.push_back(new AsmUnaryCmd(cmd, new AsmOffset(var)));
}

void AsmCode::add(AsmOperators cmd, AsmLabel * lbl) {
	codeSection.push_back(new AsmUnaryCmd(cmd, lbl));
}

void AsmCode::add(AsmOperators cmd, int i) {
	codeSection.push_back(new AsmUnaryCmd(cmd, new AsmIntConst(i)));
}

void AsmCode::add(AsmOperators cmd, string s) {
	codeSection.push_back(new AsmUnaryCmd(cmd, new AsmStringConst(s)));
}

void AsmCode::add(AsmLabel * lbl) {
	codeSection.push_back(new AsmLblCmd(lbl));
}

string AsmCode::code() {
	string result = Head;
	result += Includes;
	result += Libs;

	if (dataSection.size() > 0) {
		result += Data;
		for (auto d : dataSection) {
			result += Tab + d->code() + "\n";
		}
		result += "\n";
	}

	result += Code;
	if (codeSection.size() > 0) {
		for (auto c : codeSection) {
			result += Tab + c->code() + "\n";
		}
	}
	else {
		result += Tab + "xor eax, eax\n";
		result += Tab + "ret\n";
	}

	return result;
}
