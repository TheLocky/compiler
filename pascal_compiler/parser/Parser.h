//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_PARSER_H
#define PASCAL_COMPILER_PARSER_H

#include "tokenizer/Tokenizer.h"
#include "Node.h"
#include "exception/Exception.h"
#include "SymTable.h"

class Parser {
private:
    Tokenizer tokenizer;
    SymTable symTable;

    inline void thExpectedExpr(Node *node);
    void requireToken(LEX_TYPE tok, string tokStr);

    std::vector<Node*> ParseParameters();
    Node *ParseDesignator();
    Node *ParseAssign();
    Node *ParseBinary(int level);
    Node *ParseFactor();

    NodeBlock *ParseBlock();
    //type decl
    void ParseTypeBlock();
    SymType *ParseType(string name);
    SymConst *ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes);
    SymTypeSubRange *ParseTypeSubRange(string name);


public:
    Parser(ifstream *file) : tokenizer(file) {}
    NodeBlock *Parse();
    SymTable getSymbolsTable() const;
};


#endif //PASCAL_COMPILER_PARSER_H
