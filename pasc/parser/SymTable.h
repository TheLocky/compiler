//
// Created by xtpvx on 22.10.2015.
//

#ifndef PASCAL_COMPILER_SYMTABLE_H
#define PASCAL_COMPILER_SYMTABLE_H

#include <typeinfo>
#include <string>
#include <vector>
#include <unordered_map>
#include "tokenizer/Token.h"
#include "exception/Exception.h"
#include "StatementInterface.h"
#include "generator/Asm.h"


using std::string;

class SymTable;

namespace Symbols {

    enum Type {
        TypeInt = 0, TypeDouble, TypeChar, TypeString, TypeBool, TypeSubRange, TypePointer,
        TypeArray, TypeConst, TypeVar, TypeAlias, TypeRecord, TypeProcedure, TypeFunction, TypeEnd
    };

    class Symbol {
    public:
        string name;

		Symbol(string name);
        virtual void print(string prefix) = 0;
		virtual Type typeId() { return TypeEnd; };
    };

    class SymType : public Symbol {
    public:
        SymType(string name) : Symbol(name) { }

        virtual string getTypeStr() = 0;
        virtual void print(string prefix) = 0;
        virtual SymType *peelAlias() { return this; }
    };

	class SymIdent : public Symbol {
	public:
		SymIdent(string name) : Symbol(name) { }
		virtual void print(string prefix) = 0;
	};

    class SymVar : public SymIdent {
    public:
        SymType *type;

        SymVar(string name, SymType *type);

        virtual Type typeId() { return TypeVar; };
        virtual string getTypeStr();
        virtual void print(string prefix);
    };

    class SymConst : public SymVar {
    public:
        Token value;

        SymConst(string name, SymType *type, Token value);

        string getTypeStr();
		Type typeId() { return TypeConst; };
        void print(string prefix);
    };

    class SymTypeInt : public SymType {
    public:
        SymTypeInt(string name);

        string getTypeStr() { return "Integer"; }
		Type typeId() { return TypeInt; };
        void print(string prefix) { printf("%s = system integer", name.c_str()); }
    };

    class SymTypeDouble : public SymType {
    public:
        SymTypeDouble(string name);

        string getTypeStr() { return "Double"; }
		Type typeId() { return TypeDouble; };
        void print(string prefix) { printf("%s = system double", name.c_str()); }
    };

    class SymTypeChar : public SymType {
    public:
		SymTypeChar(string name);

        string getTypeStr() { return "Char"; }
		Type typeId() { return TypeChar; };
        void print(string prefix) { printf("%s = system char", name.c_str()); }
    };

    class SymTypeString : public SymType {
    public:
        SymTypeString(string name);

        string getTypeStr() { return "String"; }
		Type typeId() { return TypeString; };
        void print(string prefix) { printf("%s = system string", name.c_str()); }
    };

    class SymTypeBool : public SymType {
    public:
        SymTypeBool(string name);

        string getTypeStr() { return "Boolean"; }
		Type typeId() { return TypeBool; };
        void print(string prefix) { printf("%s = system bool", name.c_str()); }
    };

    class SymTypeSubRange : public SymType {
    public:
        SymConst *left, *right;

		SymTypeSubRange(string name, SymConst *left, SymConst *right);

        string getTypeStr();
		Type typeId() { return TypeSubRange; };
        void print(string prefix);
        virtual SymType *peelAlias() override { return left->type; }
    };

    class SymTypePointer : public SymType {
    public:
        SymType *baseType;

        SymTypePointer(string name, SymType *baseType);

        string getTypeStr();
		Type typeId() { return TypePointer; };
        void print(string prefix);
    };

    class SymArray : public SymType {
    public:
        SymType *arrayType;
        SymType *indexType;
        bool dynamic;

        SymArray(string name, SymType *arrType, SymType *idxType, bool dynamic);

        string getTypeStr();
		Type typeId() { return TypeArray; };
        void print(string prefix);
    };

    class SymAlias : public SymType {
    public:
        SymType *type;

		SymAlias(string name, SymType *type);

        string getTypeStr();
		Type typeId() { return TypeAlias; };
        void print(string prefix);
        virtual SymType *peelAlias() override { return type->peelAlias(); }
    };

    class SymProcedure : public SymIdent {
    public:
        enum Modificator {
            MODE_VAR,
            MODE_DEFAULT
        };
        struct Parameter {
            Modificator mode;
            string name;
            Parameter(string name, Modificator mode) : name(name), mode(mode) {}
        };
        std::vector<Parameter> parameters;
        SymTable *localTable;
        Statement *def;
        SymProcedure(string name, SymTable *table);

		Type typeId() { return TypeProcedure; };
        virtual void print(string prefix);
		virtual string generate(AsmCode *code);
		virtual string generate();
    };

    class SymFunction : public SymProcedure {
    public:
        SymType *retType;
        SymFunction(string name, SymTable *table);

		Type typeId() { return TypeFunction; };
        void print(string prefix);
    };

    class SymRecord : public SymType {
	public:
        SymTable *localTable;

        SymRecord(string name, SymTable *table);

		string getTypeStr();
		Type typeId() { return TypeRecord; };
        void print(string prefix);

    };

    extern SymTypeInt *GLOBAL_INT;
    extern SymTypeChar *GLOBAL_CHAR;
    extern SymTypeString *GLOBAL_STRING;
    extern SymTypeDouble *GLOBAL_DOUBLE;
    extern SymTypeBool *GLOBAL_BOOLEAN;
}

class SymTable {
private:
    std::unordered_map<string, Symbols::SymType *> typeSymbols;
    std::unordered_map<string, Symbols::SymIdent *> varSymbols;
    int anon_count = 0;

	bool contains(string name);


public:
    SymTable *parent;

    SymTable(SymTable *parent = nullptr) : parent(parent) {}
    void addSystemTypes();

    void addType(Symbols::SymType *type);
    void addSymbol(Symbols::SymIdent *symbol);
	bool containsType(string name);
    Symbols::SymType *getType(Token name);
	Symbols::SymIdent *getSymbol(string name);
	Symbols::SymIdent *getSymbol(Token name);
    Symbols::SymIdent *getSymbol(Token name, Symbols::Type type);
	Symbols::SymIdent *getSymbolNoThrow(Token name);
	Symbols::SymIdent *getSymbolNoThrow(Token name, Symbols::Type type);
	template<class T> T getCastedSymbol(string name);
	template<class T> T getCastedSymbol(Token name);
	template<class T> T getCastedSymbol(Token name, Symbols::Type type);
	template<class T> T getCastedSymbolNoThrow(Token name);
	template<class T> T getCastedSymbolNoThrow(Token name, Symbols::Type type);

    void print(bool printSystem, string prefix);
};

template<class T>
inline T SymTable::getCastedSymbol(string name) {
	return dynamic_cast<T>(getSymbol(name));
}

template<class T>
inline T SymTable::getCastedSymbol(Token name) {
	return dynamic_cast<T>(getSymbol(name));
}

template<class T>
inline T SymTable::getCastedSymbol(Token name, Symbols::Type type) {
	return dynamic_cast<T>(getSymbol(name, type));
}

template<class T>
inline T SymTable::getCastedSymbolNoThrow(Token name) {
	return dynamic_cast<T>(getSymbolNoThrow(name));
}

template<class T>
inline T SymTable::getCastedSymbolNoThrow(Token name, Symbols::Type type) {
	return dynamic_cast<T>(getSymbolNoThrow(name, type));
}

#endif //PASCAL_COMPILER_SYMTABLE_H