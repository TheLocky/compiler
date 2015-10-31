//
// Created by xtpvx on 22.10.2015.
//

#include "SymTable.h"

void SymTable::addSymbol(Symbol *symbol) {
    string name = contains(symbol->name) ? symbol->name + std::to_string(++anon_count) : symbol->name;
    symbols[name] = symbol;
}

bool SymTable::contains(string name) {
    return symbols.find(name) != symbols.end();
}


Symbol *SymTable::getSymbol(string name) {
    if (contains(name))
        return symbols[name];
    else
        return nullptr;
}

SymTable::SymTable() {
    anon_count = 0;
    addSymbol(new SymTypeInt("Integer"));
    addSymbol(new SymTypeChar("Char"));
    addSymbol(new SymTypeDouble("Double"));
    addSymbol(new SymTypeString("String"));
}

string SymVar::getType() {
    return string("Variable of ") + type->getType();
}

void SymVar::print() {
    printf("var %s is %s", name.c_str(), type->getType().c_str());
}

Token SymConst::getValue() const {
    return value;
}

string SymConst::getType() {
    return /*string("Const of ") + */type->getType();
}

void SymConst::print() {
    printf("Const ");
    SymVar::print();
    printf(" = %s", value.getStr().c_str());
}

string SymTypeSubRange::getType() {
    return string("SubRange of ") + left->getType();
}

void SymTypeSubRange::print() {
    printf("%s is %s SubRange = %s..%s", name.c_str(), left->getType().c_str(),
           left->getValue().getStr().c_str(), right->getValue().getStr().c_str());
}

string SymTypePointer::getType() {
    return string("Pointer of ") + baseType->getType();
}

void SymTypePointer::print() {
    printf("%s is Pointer of %s", name.c_str(), baseType->getType().c_str());
}

string SymArray::getType() {
    return string("Array[") + indexType->getType() + "] of " + arrayType->getType();
}

void SymArray::print() {
    printf("%s is Array of %s with index type %s", name.c_str(), arrayType->getType().c_str(),
           indexType->getType().c_str());
}

string SymAlias::getType() {
    return string("Alias of ") + type->getType();
}

void SymAlias::print() {
    printf("%s is Alias of %s", name.c_str(), type->getType().c_str());
}

void SymTable::print(bool printSystem) {
    if ((symbols.size() > 0 && printSystem) || (symbols.size() > 4) && !printSystem) {
        printf("====== SYMBOLS TABLE: \n");
        for (auto s : symbols) {
            Symbol *symb = (Symbol *) s.second;
            if ((symb->name != "Integer"
                && symb->name != "Char") || printSystem) {
                symb->print();
                printf("\n");
            }
        }
        printf("====== END TABLE \n\n");
    }
}
