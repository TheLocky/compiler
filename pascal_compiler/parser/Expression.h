//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_EXPRESSION_H
#define PASCAL_COMPILER_EXPRESSION_H

#include "tokenizer/Tokenizer.h"

class Expression {
protected:
    Expression *left;
    Expression *right;
    Token tk;

public:
    Expression() : left(NULL), right(NULL), tk() {};

    virtual void print(string prefix);
};

class ExprBinary : Expression {
public:
    ExprBinary(Token tk, Expression* left, Expression* right);
    void print(string prefix);
};

class ExprIntConst : Expression {
public:
    ExprIntConst(Token tk);
    void print(string prefix);
};

class ExprRealConst : Expression {
public:
    ExprRealConst(Token tk);
    void print(string prefix);
};

class ExprVariable : Expression {
public:
    ExprVariable(Token tk);
    void print(string prefix);
};


#endif //PASCAL_COMPILER_EXPRESSION_H
