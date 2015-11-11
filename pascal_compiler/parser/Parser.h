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

    void ParseDeclarationSection();
    Statement *ParseStatement(Statement *parent = nullptr);
    Statement *ParseStmtCompound(Statement *parent = nullptr);

    std::vector<NodeExpr*> ParseParameters();
    NodeExpr *ParseDesignator();
    NodeAssign *ParseAssign(NodeExpr *left);
    NodeExpr *ParseBinary(int level);
    NodeExpr *ParseFactor();

    void ParseTypeBlock();
    SymType *ParseType(string name, bool createAlias);
    SymArray *ParseTypeArray(string name);
    SymConst *ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes);
    SymTypeSubRange *ParseTypeSubRange(string name);
    void ParseConstBlock();
    void ParseVarBlock();
    std::vector<Token> ParseIdentificatorsList();

    int createCast(NodeExpr **left, NodeExpr **right);
    int createCast(NodeExpr **left, NodeExpr **right, string err);
    int createCast(SymType *type, NodeExpr **right);
    int createCast(SymType *type, NodeExpr **right, string err);

public:
    Parser(ifstream *file) : tokenizer(file) {}
    Statement *Parse();
    SymTable getSymbolsTable() const;
};


#endif //PASCAL_COMPILER_PARSER_H
