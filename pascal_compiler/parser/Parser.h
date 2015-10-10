//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_PARSER_H
#define PASCAL_COMPILER_PARSER_H

#include "tokenizer/Tokenizer.h"
#include "Expression.h"
#include "exception/Exception.h"

class Parser {
private:
    Tokenizer tokenizer;

public:
    Parser(ifstream *file) : tokenizer(file) {}
    Expression *ParseExpr();
    Expression *ParseTerm();
    Expression *ParseFactor();
};


#endif //PASCAL_COMPILER_PARSER_H
