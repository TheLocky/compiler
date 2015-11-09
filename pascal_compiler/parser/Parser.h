//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_PARSER_H
#define PASCAL_COMPILER_PARSER_H

#include "tokenizer/Tokenizer.h"
#include "Node.h"
#include "exception/Exception.h"
#include "SymTable.h"
#include "Statement.h"

using namespace Symbols;

class Parser {
private:
    Tokenizer tokenizer;
    SymTable symTable;

    inline void thExpectedExpr(Node *node);
    void requireToken(LEX_TYPE tok, string tokStr);

    void ParseTypeBlock();
    SymType *ParseType(string name, bool createAlias);
    SymArray *ParseTypeArray(string name);
    SymConst *ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes);
    SymTypeSubRange *ParseTypeSubRange(string name);
    void ParseConstBlock();
    void ParseVarBlock();
    std::vector<Token> ParseIdentList();

    NodeCast *createCast(Node *left, Node *right, bool *leftUsed);
    NodeCast *createCast(Node *left, Node *right, bool *leftUsed, string err);
    NodeCast *createCast(Symbol *type, Node *right);
    NodeCast *createCast(Symbol *type, Node *right, string err);

    void ParseDeclSection();
    Statement *ParseStatement(Statement *parent = nullptr);
    Statement *ParseStmtCompound(Statement *parent = nullptr);

    std::vector<Node*> ParseParameters();
    Node *ParseDesignator();
    Node *ParseAssign(bool mustBe = false);
    Node *ParseBinary(int level);
    Node *ParseFactor();

public:
    Parser(ifstream *file) : tokenizer(file) {}
    Statement *Parse();
    SymTable getSymbolsTable() const;
};


#endif //PASCAL_COMPILER_PARSER_H
