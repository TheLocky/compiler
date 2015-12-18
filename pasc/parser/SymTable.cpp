//
// Created by xtpvx on 22.10.2015.
//

#include "SymTable.h"

using namespace Symbols;

const string TypeStr[] = { "type Integer", "type Double", "typeChar", "type String", "type Boolean", "type Sub Range",
	"type Pointer", "type Array", "Const", "Variable", "", "Record", "Procedure", "Function" };

namespace Symbols {
    SymTypeInt *GLOBAL_INT = new SymTypeInt("Integer");
    SymTypeChar *GLOBAL_CHAR = new SymTypeChar("Char");
    SymTypeDouble *GLOBAL_DOUBLE = new SymTypeDouble("Double");
    SymTypeString *GLOBAL_STRING = new SymTypeString("String");
    SymTypeBool *GLOBAL_BOOLEAN = new SymTypeBool("Boolean");

	Symbol::Symbol(string name) : name(name) {}

	SymVar::SymVar(string name, SymType *type) : SymIdent(name), type(type) {}

	SymConst::SymConst(string name, SymType *type, Token value) : SymVar(name, type), value(value) {}

	SymTypeInt::SymTypeInt(string name) : SymType(name) {}

	SymTypeDouble::SymTypeDouble(string name) : SymType(name) {}

	SymTypeChar::SymTypeChar(string name) : SymType(name) {}

	SymTypeString::SymTypeString(string name) : SymType(name) {}

	SymTypeBool::SymTypeBool(string name) : SymType(name) {}

	SymTypeSubRange::SymTypeSubRange(string name, SymConst *left, SymConst *right) :
		SymType(name), left(left), right(right) {
	}

	SymTypePointer::SymTypePointer(string name, SymType *baseType) : SymType(name), baseType(baseType) {}

	SymArray::SymArray(string name, SymType *arrType, SymType *idxType, bool dynamic) :
		SymType(name), arrayType(arrType), indexType(idxType), dynamic(dynamic) {}

	SymAlias::SymAlias(string name, SymType *type) : SymType(name), type(type) {}
}

void SymTable::addSystemTypes() {
    addType(GLOBAL_INT);
    addType(GLOBAL_CHAR);
    addType(GLOBAL_DOUBLE);
    addType(GLOBAL_STRING);
    addType(GLOBAL_BOOLEAN);
}

void SymTable::addType(SymType *symbol) {
	//TODO: create throw
    string name = containsType(symbol->name) ? symbol->name + std::to_string(++anon_count) : symbol->name;
    typeSymbols[name] = symbol;
}

void SymTable::addSymbol(SymIdent *symbol) {
    varSymbols[symbol->name] = symbol;
}

bool SymTable::containsType(string name) {
    return typeSymbols.find(name) != typeSymbols.end();
}

bool SymTable::contains(string name) {
    return varSymbols.find(name) != varSymbols.end();
}

Symbols::SymType * SymTable::getType(Token name) {
    if (containsType(name.text))
        return typeSymbols[name.text];
	else if (parent)
		return parent->getType(name);
	else
		throw SyntaxException(name, string("unknown type '") + name.text + "'");
}

Symbols::SymIdent * SymTable::getSymbol(string name)
{
	if (contains(name))
		return varSymbols[name];
	else if (parent)
		return parent->getSymbol(name);
	else
		return nullptr;
}

Symbols::SymIdent *SymTable::getSymbol(Token name) {
	if (contains(name.text))
		return varSymbols[name.text];
	else if (parent)
		return parent->getSymbol(name);
	else
		throw SyntaxException(name, string("undefined symbol '") + name.text + "'");
}

Symbols::SymIdent * SymTable::getSymbol(Token name, Symbols::Type type)
{
	auto symb = getSymbol(name);
	if (symb->typeId() == type)
		return symb;
	else
		throw SyntaxException(name, string("symbol '") + name.text + "' is not " + TypeStr[type]);
}

Symbols::SymIdent * SymTable::getSymbolNoThrow(Token name)
{
	if (contains(name.text))
		return varSymbols[name.text];
	else if (parent)
		return parent->getSymbolNoThrow(name);
	else
		return nullptr;
}

Symbols::SymIdent * SymTable::getSymbolNoThrow(Token name, Symbols::Type type)
{
	auto symb = getSymbolNoThrow(name);
	if (symb->typeId() == type)
		return symb;
	else
		return nullptr;
}

string SymVar::getTypeStr() {
    return string("Variable of ") + type->getTypeStr();
}

void Symbols::SymVar::print(string prefix)
{
    printf("%sVar %s is %s", prefix.c_str(), name.c_str(), type->getTypeStr().c_str());
	if (type->typeId() == TypeRecord) {
		printf("\n");
		type->print(prefix + "\t");
	}
}

string SymConst::getTypeStr() {
    return type->getTypeStr();
}

void Symbols::SymConst::print(string prefix)
{
    printf("%sConst ", prefix.c_str());
    SymVar::print("");
    printf(" = %s", value.getStr().c_str());
}

string SymTypeSubRange::getTypeStr() {
    return string("SubRange of ") + left->getTypeStr();
}

void Symbols::SymTypeSubRange::print(string prefix)
{
    printf("%s%s is %s SubRange = %s..%s", prefix.c_str(), name.c_str(), left->getTypeStr().c_str(),
           left->value.getStr().c_str(), right->value.getStr().c_str());
}

string SymTypePointer::getTypeStr() {
    return string("Pointer of ") + baseType->getTypeStr();
}

void Symbols::SymTypePointer::print(string prefix)
{
    printf("%s%s is Pointer of %s", prefix.c_str(), name.c_str(), baseType->getTypeStr().c_str());
}

string SymArray::getTypeStr() {
    return string("Array[") + indexType->getTypeStr() + "] of " + arrayType->getTypeStr();
}

void Symbols::SymArray::print(string prefix)
{
    printf("%s%s is Array of %s with index type %s", prefix.c_str(), name.c_str(), arrayType->getTypeStr().c_str(),
           indexType->getTypeStr().c_str());
}

string SymAlias::getTypeStr() {
    return string("Alias of ") + type->getTypeStr();
}

void Symbols::SymAlias::print(string prefix)
{
    printf("%s%s is Alias of %s", prefix.c_str(), name.c_str(), type->getTypeStr().c_str());
}

void SymTable::print(bool printSystem, string prefix) {
    if (((typeSymbols.size() > 0 && printSystem) || (typeSymbols.size() > 5) && !printSystem)
            || varSymbols.size() > 0) {
        printf("%s====== SYMBOLS TABLE: \n", prefix.c_str());
        for (auto s : typeSymbols) {
            SymType *type = (SymType *) s.second;
            if ((type != GLOBAL_INT && type != GLOBAL_CHAR && type != GLOBAL_STRING &&
                    type != GLOBAL_DOUBLE && type != GLOBAL_BOOLEAN) || printSystem) {
                type->print(prefix + "\t");
                printf("\n");
            }
        }
        for (auto s : varSymbols) {
            auto var = s.second;
            var->print(prefix + "\t");
            printf("\n");
        }
        printf("%s====== END TABLE\n", prefix.c_str());
    }
}

SymProcedure::SymProcedure(string name, SymTable *table)  : SymIdent(name), localTable(table) {}

void Symbols::SymProcedure::print(string prefix)
{
    printf("%s====== PROCEDURE %s:\n", prefix.c_str(), name.c_str());
    localTable->print(false, prefix + "\t");
    if (parameters.size() > 0) {
        printf("%s====== PARAMETERS: ", prefix.c_str());
        for (Parameter param : parameters) {
			printf("%s:%s; ", param.name.c_str(),
				((SymVar*)localTable->getSymbol(param.name))->type->name.c_str());
        }
        printf("\n");
    }
    printf("%s====== DEFINE: \n", prefix.c_str());
    def->print(prefix + "\t");
}

string Symbols::SymProcedure::generate(AsmCode *code) {
	return string();
}

string Symbols::SymProcedure::generate() {
	auto asmCode = new AsmCode();
	auto lblStart = new AsmLabel("start");
	asmCode->add(lblStart);
	def->generate(asmCode);
	asmCode->add(push, 0);
	asmCode->add(call, new AsmIdentificator("ExitProcess"));
	asmCode->add(end, lblStart);
	return asmCode->code();
}

SymFunction::SymFunction(string name, SymTable *table) : SymProcedure(name, table) {}

void Symbols::SymFunction::print(string prefix)
{
    printf("%s====== FUNCTION %s:\n", prefix.c_str(), name.c_str());
    localTable->print(false, prefix + "\t");
    if (parameters.size() > 0) {
        printf("%s====== PARAMETERS: ", prefix.c_str());
        for (Parameter param : parameters) {
			printf("%s:%s; ", param.name.c_str(),
				((SymVar*)localTable->getSymbol(param.name))->type->name.c_str());
        }
        printf("\n");
    }
    printf("%s====== RETURN: %s\n", prefix.c_str(), retType->getTypeStr().c_str());
    printf("%s====== DEFINE: \n", prefix.c_str());
    def->print(prefix + "\t");
}

SymRecord::SymRecord(string name, SymTable *table) : SymType(name), localTable(table) {}

void Symbols::SymRecord::print(string prefix)
{
    printf("%s====== RECORD %s:\n", prefix.c_str(), name.c_str());
    localTable->print(false, prefix + "\t");
}

string SymRecord::getTypeStr() {
	return "Record";
}