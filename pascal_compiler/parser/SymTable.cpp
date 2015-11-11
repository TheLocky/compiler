//
// Created by xtpvx on 22.10.2015.
//

#include "SymTable.h"

using namespace Symbols;

void SymTable::addType(SymType *symbol) {
    string name = containsType(symbol->name) ? symbol->name + std::to_string(++anon_count) : symbol->name;
    typeSymbols[name] = symbol;
}

void SymTable::addSymbol(SymVar *symbol) {
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

SymVar *SymTable::getSymbol(string name) {
    if (contains(name))
        return varSymbols[name];
    else
        return nullptr;
}

SymTable::SymTable() {
    anon_count = 0;
    addType(new SymTypeInt(TYPENAME_INT));
    addType(new SymTypeChar(TYPENAME_CHAR));
    addType(new SymTypeDouble(TYPENAME_DOUBLE));
    addType(new SymTypeString(TYPENAME_STRING));
    addType(new SymTypeBool(TYPENAME_BOOLEAN));
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
            if ((type->name != TYPENAME_INT && type->name != TYPENAME_CHAR && type->name != TYPENAME_STRING
                && type->name != TYPENAME_DOUBLE && type->name != TYPENAME_BOOLEAN) || printSystem) {
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
