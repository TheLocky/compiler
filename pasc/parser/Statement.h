//
// Created by xtpvx on 05.11.2015.
//

#ifndef PASCAL_COMPILER_STATEMENT_H
#define PASCAL_COMPILER_STATEMENT_H

#include <string>
#include <vector>
#include "Node.h"
#include "StatementInterface.h"

using std::string;
using std::vector;

class StmtExpression : public Statement {
public:
    Node *expression;

    StmtExpression(Statement *parent = nullptr) : Statement(parent) {}

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtCompound : public Statement {
public:
    vector<Statement*> stmtList;

    StmtCompound(Statement *parent = nullptr) : Statement(parent) {}

    void add(Statement *statement) { stmtList.push_back(statement); }
    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtIf : public Statement {
public:
    ExprBinary *condition;
    Statement *then;
    Statement *otherwise;

    StmtIf(Statement *parent = nullptr) : Statement(parent) { otherwise = nullptr; }

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtCase : public Statement {
public:
    ExprBinary *condition;
    struct Case {
        Token left;
        Token right; //TK_ERROR if case is not subrange
        Statement *statement;
    };
    vector<Case*> cases;
    Statement *otherwise;

    StmtCase(Statement *parent = nullptr) : Statement(parent) { otherwise = nullptr; }

    void add(Token left, Token right, Statement *stmt);
    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtLoop : public Statement {
public:
    StmtLoop(Statement *parent = nullptr) : Statement(parent) {}

    virtual void print(string prefix) = 0;
	virtual void generate(AsmCode *code) = 0;
};

class StmtFor : public StmtLoop {
public:
    bool downto;
    Symbols::SymVar *variable;
    NodeExpr *initExpr;
    NodeExpr *resExpr;
    Statement *statement;

    StmtFor(Statement *parent = nullptr) : StmtLoop(parent) {}

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtWhile : public StmtLoop {
public:
    ExprBinary *condition;
    Statement *statement;

    StmtWhile(Statement *parent = nullptr) : StmtLoop(parent){}

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtRepeat : public StmtLoop {
public:
    ExprBinary *condition;
    StmtCompound *statement;

    StmtRepeat(Statement *parent = nullptr) : StmtLoop(parent) {}

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtBreak : public Statement {
public:
    StmtBreak(Statement *parent) : Statement(parent) {}

    void print(string prefix);
	void generate(AsmCode *code);
};

class StmtContinue: public Statement {
public:
    StmtContinue(Statement *parent) : Statement(parent) {}

    void print(string prefix);
	void generate(AsmCode *code);
};


#endif //PASCAL_COMPILER_STATEMENT_H
