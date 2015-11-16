//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_EXPRESSION_H
#define PASCAL_COMPILER_EXPRESSION_H

#include "tokenizer/Tokenizer.h"
#include "SymTable.h"

class Node {
public:
    virtual void print(string prefix) = 0;
};

class NodeExpr : public Node {
public:
    Token operatorToken;
    Symbols::SymType *type;

    virtual void print(string prefix) = 0;

    friend class NodeAssign;
};

class ExprBinary : public NodeExpr {
private:
    NodeExpr *left;
    NodeExpr *right;
public:
    ExprBinary(Token tk, NodeExpr *left, NodeExpr *right);

    void print(string prefix);
};

class ExprUnary : public NodeExpr {
private:
    NodeExpr *right;
public:
    ExprUnary(Token tk, NodeExpr *right);

    void print(string prefix);
};

class ExprIntConst : public NodeExpr {
public:
    ExprIntConst(Token tk, Symbols::SymType *symbol);

    void print(string prefix);
};

class ExprRealConst : public NodeExpr {
public:
    ExprRealConst(Token tk, Symbols::SymType *symbol);

    void print(string prefix);
};

class ExprStringConst : public NodeExpr {
public:
    ExprStringConst(Token tk, Symbols::SymType *symbol);

    void print(string prefix);
};

class ExprCharConst : public NodeExpr {
public:
    ExprCharConst(Token tk, Symbols::SymType *symbol);

    void print(string prefix);
};

class ExprBooleanConst : public NodeExpr {
public:
    ExprBooleanConst(Token tk, Symbols::SymType *symbol);

    void print(string prefix);
};

class ExprVariable : public NodeExpr {
public:
    ExprVariable(Token tk, Symbols::SymType *varType);

    void print(string prefix);
};

class NodeArrayIndex : public NodeExpr {
private:
    NodeExpr *parent;
    NodeExpr *child;
public:
    NodeArrayIndex(NodeExpr *parent, NodeExpr *child, Symbols::SymType *arrType);

    void print(string prefix);
};

class NodeRecordAccess : public NodeExpr {
private:
    NodeExpr *left;
    NodeExpr *right;
public:
    NodeRecordAccess(NodeExpr *left, NodeExpr *right);

    void print(string prefix);
};

class NodeFunc : public NodeExpr {
protected:
    NodeExpr *parent;
    std::vector<NodeExpr*> params;
public:
    NodeFunc(NodeExpr *parent, std::vector<NodeExpr*> params, Symbols::SymType *retType);

    void print(string prefix);
};

class NodeCast : public NodeExpr {
public:
    Node *parameter;

    NodeCast(NodeExpr *parameter, Symbols::SymType *castType);

    void print(string prefix);
};

class NodeAssign : public Node {
private:
    NodeExpr *left;
    NodeExpr *right;
public:
    NodeAssign(NodeExpr *left, NodeExpr *right);

    void print(string prefix);
};


#endif //PASCAL_COMPILER_EXPRESSION_H
