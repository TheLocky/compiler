//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_PARSER_H
#define PASCAL_COMPILER_PARSER_H

#include "tokenizer/Tokenizer.h"
#include "Node.h"
#include "exception/Exception.h"

static std::list<LEX_TYPE> priority[] = {
        lexList(TK_ADD, TK_SUB, 0),
        lexList(TK_MUL, TK_DIV, 0),
        lexList(TK_CGT, TK_CLT, TK_CGE, TK_CLE, TK_CNE, TK_CE, 0)
};

class Parser {
private:
    Tokenizer tokenizer;

    inline void checkEof();

    std::list<Node*> ParseParameters();
    Node *ParseFunction();
    Node *ParseRecordAccess();
    Node *ParseArrayIndex();
    Node *ParseAssign();
    Node *ParseUnary();
    Node *ParseBinary(int level);
    Node *ParseFactor();

    NodeBlock *ParseBlock();

public:
    Parser(ifstream *file) : tokenizer(file) {}
    NodeBlock *Parse();
};


#endif //PASCAL_COMPILER_PARSER_H
