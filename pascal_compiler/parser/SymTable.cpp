//
// Created by xtpvx on 22.10.2015.
//

#include "SymTable.h"

using namespace Symbols;

void SymTable::addTypeSymbol(SymType *symbol) {
    string name = containsType(symbol->name) ? symbol->name + std::to_string(++anon_count) : symbol->name;
    typeSymbols[name] = symbol;
}

void SymTable::addVarSymbol(SymVar *symbol) {
    varSymbols[symbol->name] = symbol;
}

bool SymTable::containsType(string name) {
    return typeSymbols.find(name) != typeSymbols.end();
}

bool SymTable::containsVar(string name) {
    return varSymbols.find(name) != varSymbols.end();
}

SymType *SymTable::getTypeSymbol(string name) {
    if (containsType(name))
        return typeSymbols[name];
    else
        return nullptr;
}

SymVar *SymTable::getVarSymbol(string name) {
    if (containsVar(name))
        return varSymbols[name];
    else
        return nullptr;
}

SymTable::SymTable() {
    anon_count = 0;
    addTypeSymbol(new SymTypeInt("Integer"));
    addTypeSymbol(new SymTypeChar("Char"));
    addTypeSymbol(new SymTypeDouble("Double"));
    addTypeSymbol(new SymTypeString("String"));
    addTypeSymbol(new SymTypeBool("Bool"));
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
            if ((type->name != "Integer"
                && type->name != "Char"
                && type->name != "String"
                && type->name != "Double"
                && type->name != "Bool") || printSystem) {
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

bool SymTable::compareTypes(Symbols::Symbol *one, Symbols::Symbol *two) {
    while (one->isAlias()) one = one->toAlias()->type;
    while (two->isAlias()) two = two->toAlias()->type;
    if (one->isConst()) one = one->toConst()->type;
    if (two->isConst()) two = two->toConst()->type;
    if (one->isVar()) one = one->toVar()->type;
    if (two->isVar()) two = two->toVar()->type;

    if (one->typeId <= TypeBool && two->typeId <= TypeBool)
        return one->typeId == two->typeId;
    else if (one->isArray() && two->isArray())
        return compareTypes(one->toArray()->indexType, two->toArray()->indexType) &&
               compareTypes(one->toArray()->arrayType, two->toArray()->arrayType);
    else if (one->isPointer() && two->isPointer())
        return compareTypes(one->toPointer()->baseType, two->toPointer()->baseType);
    else if (one->isSubRange() && two->isSubRange())
        return compareTypes(one->toSubRange()->left->type, two->toSubRange()->left->type);

    return false;
}

bool SymTable::compareTypes(Symbols::Symbol *one, Symbols::Type type) {
    while (one->isAlias()) one = one->toAlias()->type;
    if (one->isConst()) one = one->toConst()->type;
    if (one->isVar()) one = one->toVar()->type;

    if (one->typeId <= TypeBool && type <= TypeBool)
        return one->typeId == type;
    else if (one->isSubRange())
        return compareTypes(one->toSubRange()->left->type, type);
    return false;
}

bool SymTable::compareTypes(Symbols::Symbol *one, std::vector<Symbols::Type> typeList) {
    for (auto type : typeList)
        if (compareTypes(one, type))
            return true;
    return false;
}
