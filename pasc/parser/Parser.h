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
    SymTable *currentTable;

    inline void thExpectedExpr(Node *node);
    void requireToken(LEX_TYPE tok, string tokStr);

    Statement *ParseStatement(Statement *parent = nullptr);
    Statement *ParseStmtCompound(Statement *parent = nullptr);
    Statement *ParseStmtIf(Statement *parent = nullptr);
    Statement *ParseStmtCase(Statement *parent = nullptr);
    Statement *ParseStmtFor(Statement *parent = nullptr);
    Statement *ParseStmtWhile(Statement *parent = nullptr);
    Statement *ParseStmtRepeat(Statement *parent = nullptr);

    std::vector<NodeExpr*> ParseParameters();
    NodeExpr *ParseDesignator();
    NodeAssign *ParseAssign(NodeExpr *left);
    NodeExpr *ParseBinary(int level);
    NodeExpr *ParseFactor();

    void ParseDeclarationSection();
    void ParseTypeBlock();
    SymType *ParseType(string name, bool createAlias);
    SymArray *ParseTypeArray(string name);
    SymConst *ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes);
    SymTypeSubRange *ParseTypeSubRange(string name);
    void ParseConstBlock();
    void ParseVarBlock();
    std::vector<Token> ParseIdentificatorsList();
    void ParseCallDeclaration();
	std::vector<SymProcedure::Parameter> ParseCallDeclParameters();
    SymRecord *ParseRecord(string name);

    int createCast(NodeExpr **left, NodeExpr **right);
    int createCast(NodeExpr **left, NodeExpr **right, string err);
    int createCast(SymType *type, NodeExpr **right);
    int createCast(SymType *type, NodeExpr **right, string err);

public:
    Parser(ifstream *file) : tokenizer(file) {}
    SymProcedure *Parse();
};


#endif //PASCAL_COMPILER_PARSER_H
