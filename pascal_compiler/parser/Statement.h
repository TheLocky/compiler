//
// Created by xtpvx on 05.11.2015.
//

#ifndef PASCAL_COMPILER_STATEMENT_H
#define PASCAL_COMPILER_STATEMENT_H

#include <string>
#include <vector>
#include "Node.h"

using std::string;
using std::vector;

class Statement {
public:
    Statement *parent;

    Statement(Statement *parent = nullptr) : parent(parent) {}

    virtual void print(string prefix) = 0;
};

class StmtExpression : public Statement {
public:
    Node *expression;

    StmtExpression(Statement *parent = nullptr) : Statement(parent) {}

    void print(string prefix);
};

class StmtCompound : public Statement {
public:
    vector<Statement*> stmtList;

    StmtCompound(Statement *parent = nullptr) : Statement(parent) {}

    void add(Statement *statement) { stmtList.push_back(statement); }
    void print(string prefix);
};

class StmtIf : public Statement {
public:
    ExprBinary *condition;
    Statement *then;
    Statement *otherwise;

    StmtIf(Statement *parent = nullptr) : Statement(parent) {}

    void print(string prefix);
};

class StmtCase : public Statement {
public:
    ExprBinary *condition;
    struct Case {
        Token left;
        Token right;
        Statement *statement;
    };
    vector<Case*> cases;

    StmtCase(Statement *parent = nullptr) : Statement(parent) {}

    void add(Token left, Token right, Statement *stmt);
    void print(string prefix);
};

class StmtFor : public Statement {
public:
    bool to;
    NodeAssign *initExpr;
    ExprBinary *condition;
    Statement *statement;

    StmtFor(Statement *parent = nullptr) : Statement(parent) {}

    void print(string prefix);
};

class StmtWhile : public Statement {
public:
    ExprBinary *condition;
    Statement *statement;

    StmtWhile(Statement *parent = nullptr) : Statement(parent){}

    void print(string prefix);
};

class StmtRepeat : public Statement {
public:
    ExprBinary *condition;
    Statement *statement;

    StmtRepeat(Statement *parent = nullptr) : Statement(parent) {}

    void print(string prefix);
};


#endif //PASCAL_COMPILER_STATEMENT_H
