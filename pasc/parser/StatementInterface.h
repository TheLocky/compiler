//
// Created by xtpvx on 16.11.2015.
//

#ifndef PASCAL_COMPILER_STATEMENTINTERFACE_H
#define PASCAL_COMPILER_STATEMENTINTERFACE_H

#include <string>
#include "generator/Asm.h"

class Statement {
public:
    Statement *parent;

    Statement(Statement *parent = nullptr) : parent(parent) {}

    virtual void print(std::string prefix) = 0;
	virtual void generate(AsmCode *code) = 0;
};

#endif //PASCAL_COMPILER_STATEMENTINTERFACE_H
