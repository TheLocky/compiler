//
// Created by xtpvx on 22.10.2015.
//

#include "SymTable.h"

using namespace Symbols;

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

SymType *SymTable::getType(string name) {
    if (containsType(name))
        return typeSymbols[name];
    else
        return nullptr;
}

SymIdent *SymTable::getSymbol(string name) {
	if (contains(name))
		return varSymbols[name];
	else
		return nullptr;
}

Symbols::SymIdent * SymTable::getSymbol(string name, Symbols::Type type)
{
	if (contains(name))
		return varSymbols[name];
	else
		return nullptr;
}

Symbols::SymIdent * SymTable::getSymbolNoThrow(string name)
{
	if (contains(name))
		return varSymbols[name];
	else
		return nullptr;
}

string SymVar::getTypeStr() {
    return string("Variable of ") + type->getTypeStr();
}

void SymVar::print() {
    printf("Var %s is %s", name.c_str(), type->getTypeStr().c_str());
}

string SymConst::getTypeStr() {
    return type->getTypeStr();
}

void SymConst::print() {
    printf("Const ");
    SymVar::print();
    printf(" = %s", value.getStr().c_str());
}

string SymTypeSubRange::getTypeStr() {
    return string("SubRange of ") + left->getTypeStr();
}

void SymTypeSubRange::print() {
    printf("%s is %s SubRange = %s..%s", name.c_str(), left->getTypeStr().c_str(),
           left->value.getStr().c_str(), right->value.getStr().c_str());
}

string SymTypePointer::getTypeStr() {
    return string("Pointer of ") + baseType->getTypeStr();
}

void SymTypePointer::print() {
    printf("%s is Pointer of %s", name.c_str(), baseType->getTypeStr().c_str());
}

string SymArray::getTypeStr() {
    return string("Array[") + indexType->getTypeStr() + "] of " + arrayType->getTypeStr();
}

void SymArray::print() {
    printf("%s is Array of %s with index type %s", name.c_str(), arrayType->getTypeStr().c_str(),
           indexType->getTypeStr().c_str());
}

string SymAlias::getTypeStr() {
    return string("Alias of ") + type->getTypeStr();
}

void SymAlias::print() {
    printf("%s is Alias of %s", name.c_str(), type->getTypeStr().c_str());
}

void SymTable::print(bool printSystem) {
    if (((typeSymbols.size() > 0 && printSystem) || (typeSymbols.size() > 5) && !printSystem)
            || varSymbols.size() > 0) {
        printf("====== SYMBOLS TABLE: \n");
        for (auto s : typeSymbols) {
            SymType *type = (SymType *) s.second;
            if ((type != GLOBAL_INT && type != GLOBAL_CHAR && type != GLOBAL_STRING &&
                    type != GLOBAL_STRING && type != GLOBAL_BOOLEAN) || printSystem) {
                type->print();
                printf("\n");
            }
        }
        for (auto s : varSymbols) {
            SymVar *var = (SymVar *) s.second;
            var->print();
            printf("\n");
        }
        printf("====== END TABLE\n\n");
    }
}

SymProcedure::SymProcedure(string name, SymTable *table)  : SymIdent(name), localTable(table) {}

void SymProcedure::print() {
    printf("\n====== PROCEDURE %s:\n", name.c_str());
    localTable->print(false);
    if (parameters.size() > 0) {
        printf("====== PARAMETERS: ");
        for (Parameter param : parameters) {
            printf("%s:%s ", param.name.c_str(), 
				((SymVar*)localTable->getSymbol(param.name))->getTypeStr().c_str());
        }
        printf("\n");
    }
    printf("====== DEFINE: \n");
    def->print("\t");
    printf("\n");
}

SymFunction::SymFunction(string name, SymTable *table) : SymProcedure(name, table) {}

void SymFunction::print() {
    printf("\n====== FUNCTION %s:\n", name.c_str());
    localTable->print(false);
    if (parameters.size() > 0) {
        printf("====== PARAMETERS: ");
        for (Parameter param : parameters) {
            printf("%s:%s ", param.name.c_str(), 
				((SymVar*)localTable->getSymbol(param.name))->getTypeStr().c_str());
        }
        printf("\n");
    }
    printf("====== RETURN: %s", retType->getTypeStr().c_str());
    printf("====== DEFINE: \n");
    def->print("\t");
    printf("\n");
}

SymRecord::SymRecord(string name, SymTable *table) : SymType(name), localTable(table) {}

void SymRecord::print() {
    printf("\n====== RECORD %s:\n", name.c_str());
    localTable->print(false);
    printf("\n");
}

string SymRecord::getTypeStr() {
	return "Record";
}