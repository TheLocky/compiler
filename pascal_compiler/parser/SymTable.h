//
// Created by xtpvx on 22.10.2015.
//

#ifndef PASCAL_COMPILER_SYMTABLE_H
#define PASCAL_COMPILER_SYMTABLE_H

#include <string>
#include "tokenizer/Token.h"
#include <unordered_map>
#include "exception/Exception.h"

using std::string;

class Symbol {
public:
    string name;
    Symbol(string name) : name(name){}

    virtual string getType() = 0;
    virtual void print() = 0;
};

class SymType : public Symbol {
public:
    SymType(string name) : Symbol(name){}

    virtual string getType() = 0;
    virtual void print() = 0;
};

class SymVar : public Symbol {
public:
    SymType *type;

    SymVar(string name, SymType *type) : Symbol(name), type(type) {}

    string getType();
    virtual void print();
};

class SymConst : public SymVar {
private:
    Token value;
public:
    SymConst(string name, SymType *type, Token value) : SymVar(name, type), value(value){}

    Token getValue() const;
    string getType();
    void print();
};

class SymTypeInt : public SymType {
public:
    SymTypeInt(string name) : SymType(name){}

    string getType() { return "Integer"; }
    void print() { printf("%s = system integer", name.c_str()); }
};

class SymTypeDouble : public SymType {
public:
    SymTypeDouble(string name) : SymType(name){}

    string getType() { return "Double"; }
    void print() { printf("%s = system double", name.c_str()); }
};

class SymTypeChar : public SymType {
public:
    SymTypeChar(string name) : SymType(name){}

    string getType() { return "Char"; }
    void print() { printf("%s = system char", name.c_str()); }
};

class SymTypeString : public SymType {
public:
    SymTypeString(string name) : SymType(name){}

    string getType() { return "String"; }
    void print() { printf("%s = system string", name.c_str()); }
};

class SymTypeSubRange : public SymType {
private:
    SymConst *left, *right;
public:
    SymTypeSubRange(string name, SymConst *left, SymConst *right) :
            SymType(name), left(left), right(right) {}

    string getType();
    void print();
};

class SymTypePointer : public SymType {
private:
    SymType *baseType;
public:
    SymTypePointer(string name, SymType *baseType) : SymType(name), baseType(baseType) {}

    string getType();
    void print();
};

class SymArray : public SymType {
private:
    SymType *arrayType;
    SymType *indexType;
    bool dynamic;
public:
    SymArray(string name, SymType *arrType, SymType *idxType, bool dynamic) : SymType(name),
             arrayType(arrType), indexType(idxType), dynamic(dynamic){}

    string getType();
    void print();
};

class SymAlias : public SymType {
private:
    SymType *type;
public:
    SymAlias(string name, SymType *type) : SymType(name), type(type){}

    string getType();
    void print();
};

class SymTable {
private:
    std::unordered_map<string, Symbol *> symbols;
    int anon_count;

public:
    SymTable();

    void addSymbol(Symbol *symbol);
    bool contains(string name);
    Symbol *getSymbol(string name);

    void print(bool printSystem);
};


#endif //PASCAL_COMPILER_SYMTABLE_H
