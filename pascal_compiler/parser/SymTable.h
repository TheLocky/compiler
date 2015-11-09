//
// Created by xtpvx on 22.10.2015.
//

#ifndef PASCAL_COMPILER_SYMTABLE_H
#define PASCAL_COMPILER_SYMTABLE_H

#include <typeinfo>
#include <string>
#include "tokenizer/Token.h"
#include <unordered_map>
#include "exception/Exception.h"
#include <vector>

using std::string;

namespace Symbols {

    enum Type {
        TypeInt = 0, TypeDouble, TypeChar, TypeString, TypeBool, TypeSubRange, TypePointer,
        TypeArray, TypeConst, TypeVar, TypeAlias, TypeEnd
    };

    class SymTypeInt;
    class SymTypeDouble;
    class SymTypeChar;
    class SymTypeString;
    class SymTypeBool;
    class SymTypeSubRange;
    class SymTypePointer;
    class SymTypeArray;
    class SymArray;
    class SymVar;
    class SymConst;
    class SymAlias;

    class Symbol {
    public:
        Type typeId = TypeEnd;
        string name;

        Symbol(string name) : name(name) { }

        virtual void print() = 0;

#define __is(x) inline bool is##x() { return typeId == Type##x; }

        __is(SubRange) __is(Pointer)  __is(Array) __is(Const) __is(Var) __is(Alias)

#define __to(x) inline Sym##x *to##x() { return (Sym##x *) this; }
#define __toType(x) inline SymType##x *to##x() { return (SymType##x *) this; }

        __toType(SubRange) __toType(Pointer) __to(Array) __to(Const) __to(Var) __to(Alias)
    };

    class SymType : public Symbol {
    public:
        SymType(string name) : Symbol(name) { }

        virtual Type getTypeId() { return typeId; };

        virtual string getTypeStr() = 0;

        virtual void print() = 0;
    };

    class SymVar : public Symbol {
    public:
        SymType *type;

        SymVar(string name, SymType *type) : Symbol(name), type(type) {
            Symbol::typeId = TypeVar;
        }

        virtual Type getTypeId() { return typeId; };

        virtual string getTypeStr();

        virtual void print();
    };

    class SymConst : public SymVar {
    public:
        Token value;

        SymConst(string name, SymType *type, Token value) : SymVar(name, type), value(value) {
            SymVar::typeId = TypeConst;
        }

        string getTypeStr();

        void print();
    };

    class SymTypeInt : public SymType {
    public:
        SymTypeInt(string name) : SymType(name) {
            Symbol::typeId = TypeInt;
        }

        string getTypeStr() { return "Integer"; }

        void print() { printf("%s = system integer", name.c_str()); }
    };

    class SymTypeDouble : public SymType {
    public:
        SymTypeDouble(string name) : SymType(name) {
            Symbol::typeId = TypeDouble;
        }

        string getTypeStr() { return "Double"; }

        void print() { printf("%s = system double", name.c_str()); }
    };

    class SymTypeChar : public SymType {
    public:
        SymTypeChar(string name) : SymType(name) {
            Symbol::typeId = TypeChar;
        }

        string getTypeStr() { return "Char"; }

        void print() { printf("%s = system char", name.c_str()); }
    };

    class SymTypeString : public SymType {
    public:
        SymTypeString(string name) : SymType(name) {
            Symbol::typeId = TypeString;
        }

        string getTypeStr() { return "String"; }

        void print() { printf("%s = system string", name.c_str()); }
    };

    class SymTypeBool : public SymType {
    public:
        SymTypeBool(string name) : SymType(name) {
            Symbol::typeId = TypeBool;
        }

        string getTypeStr() { return "Bool"; }

        void print() { printf("%s = system bool", name.c_str()); }
    };

    class SymTypeSubRange : public SymType {
    public:
        SymConst *left, *right;

        SymTypeSubRange(string name, SymConst *left, SymConst *right) :
                SymType(name), left(left), right(right) {
            Symbol::typeId = TypeSubRange;
        }

        string getTypeStr();

        void print();
    };

    class SymTypePointer : public SymType {
    public:
        SymType *baseType;

        SymTypePointer(string name, SymType *baseType) : SymType(name), baseType(baseType) {
            Symbol::typeId = TypePointer;
        }

        string getTypeStr();

        void print();
    };

    class SymArray : public SymType {
    public:
        SymType *arrayType;
        SymType *indexType;
        bool dynamic;

        SymArray(string name, SymType *arrType, SymType *idxType, bool dynamic) :
                SymType(name), arrayType(arrType), indexType(idxType), dynamic(dynamic) {
            Symbol::typeId = TypeArray;
        }

        string getTypeStr();

        void print();
    };

    class SymAlias : public SymType {
    public:
        SymType *type;

        SymAlias(string name, SymType *type) : SymType(name), type(type) {
            Symbol::typeId = TypeAlias;
        }

        string getTypeStr();

        void print();
    };
}

class SymTable {
private:
    std::unordered_map<string, Symbols::SymType *> typeSymbols;
    std::unordered_map<string, Symbols::SymVar *> varSymbols;
    int anon_count;

public:
    SymTable();

    void addTypeSymbol(Symbols::SymType *symbol);
    void addVarSymbol(Symbols::SymVar *symbol);
    bool containsType(string name);
    bool containsVar(string name);
    Symbols::SymType *getTypeSymbol(string name);
    Symbols::SymVar *getVarSymbol(string name);

    static bool compareTypes(Symbols::Symbol *one, Symbols::Symbol *two);
    static bool compareTypes(Symbols::Symbol *one, Symbols::Type type);
    static bool compareTypes(Symbols::Symbol *one, std::vector<Symbols::Type> typeList);

    void print(bool printSystem);
};


#endif //PASCAL_COMPILER_SYMTABLE_H
