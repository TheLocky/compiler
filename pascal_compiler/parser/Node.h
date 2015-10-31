//
// Created by xtpvx on 07.10.2015.
//

#ifndef PASCAL_COMPILER_EXPRESSION_H
#define PASCAL_COMPILER_EXPRESSION_H

#include "tokenizer/Tokenizer.h"

class Node {
protected:
    Token tk;
public:
    Node() : tk() { };

    virtual void print(string prefix);
};

class NodeBlock {
private:
    std::list<Node *> lines;
public:
    NodeBlock(std::list<Node *> lines) : lines(lines) {}

    void print(string prefix);
};

class ExprBinary : public Node {
private:
    Node *left;
    Node *right;
public:
    ExprBinary(Token tk, Node *left, Node *right);

    void print(string prefix);
};

class ExprUnary : public Node {
private:
    Node *right;
public:
    ExprUnary(Token tk, Node *right);

    void print(string prefix);
};

class NodeIntConst : public Node {
public:
    NodeIntConst(Token tk);

    void print(string prefix);
};

class NodeRealConst : public Node {
public:
    NodeRealConst(Token tk);

    void print(string prefix);
};

class NodeStringConst : public Node {
public:
    NodeStringConst(Token tk);

    void print(string prefix);
};

class NodeCharConst : public Node {
public:
    NodeCharConst(Token tk);

    void print(string prefix);
};

class NodeVariable : public Node {
public:
    NodeVariable(Token tk);

    void print(string prefix);
};

class NodeArrayIndex : public Node {
private:
    Node *parent;
    Node *child;
public:
    NodeArrayIndex(Node *parent, Node *child);

    void print(string prefix);
};

class NodeRecordAccess : public Node {
private:
    Node *left;
    Node *right;
public:
    NodeRecordAccess(Node *left, Node *right);

    void print(string prefix);
};

class NodeFunc : public Node {
protected:
    Node *parent;
    std::vector<Node*> params;
public:
    NodeFunc(Node *parent, std::vector<Node*> params);

    void print(string prefix);
};

class NodeCast : public NodeFunc {
public:
    NodeCast(Node *parent, Node *parameter);

    void print(string prefix);
};

class NodeAssign : public Node {
private:
    Node *left;
    Node *right;
public:
    NodeAssign(Node *left, Node *right);

    void print(string prefix);
};


#endif //PASCAL_COMPILER_EXPRESSION_H
