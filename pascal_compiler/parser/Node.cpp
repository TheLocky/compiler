//
// Created by xtpvx on 07.10.2015.
//

#include "Node.h"

ExprBinary::ExprBinary(Token tk, Node * left, Node * right) {
    Node::tk = tk;
    this->left = left;
    this->right = right;
    Node::symbol = left->symbol;
}

void ExprBinary::setSymbol(Symbols::Symbol *symbol) {
    Node::symbol = symbol;
}

ExprUnary::ExprUnary(Token tk, Node *right) {
    Node::tk = tk;
    this->right = right;
    Node::symbol = right->symbol;
}

NodeIntConst::NodeIntConst(Token tk, Symbols::Symbol *symbol) {
    Node::tk = tk;
    Node::symbol = symbol;
}

NodeRealConst::NodeRealConst(Token tk, Symbols::Symbol *symbol) {
    Node::tk = tk;
    Node::symbol = symbol;
}

NodeStringConst::NodeStringConst(Token tk, Symbols::Symbol *symbol) {
    Node::tk = tk;
    Node::symbol = symbol;
}

NodeCharConst::NodeCharConst(Token tk, Symbols::Symbol *symbol) {
    Node::tk = tk;
    Node::symbol = symbol;
}

NodeVariable::NodeVariable(Token tk, Symbols::Symbol *symbol) {
    Node::tk = tk;
    Node::symbol = symbol;
}

void ExprBinary::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (left != NULL)
            left->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
        if (right != NULL)
            right->print(prefix + "\t");
    }
}

void ExprUnary::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
        if (right != NULL)
            right->print(prefix + "\t");
    }
}

void NodeIntConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.getStr().c_str());
    }
}

void NodeRealConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.getStr().c_str());
    }
}

void NodeStringConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.getStr().c_str());
    }
}

void NodeCharConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.getStr().c_str());
    }
}

void NodeVariable::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
    }
}

NodeArrayIndex::NodeArrayIndex(Node *parent, Node *child) {
    tk.tokenType = TK_LSQB;
    this->parent = parent;
    this->child = child;
    Node::symbol = parent->symbol;
}

NodeRecordAccess::NodeRecordAccess(Node *left, Node *right) {
    tk.tokenType = TK_POINT;
    this->left = left;
    this->right = right;
}

NodeFunc::NodeFunc(Node *parent, std::vector<Node*> params) {
    tk.tokenType = TK_LB;
    this->parent = parent;
    this->params = params;
}

NodeCast::NodeCast(Symbols::Symbol *type, Node *parameter) : parameter(parameter) {
    Node::symbol = type;
}

NodeAssign::NodeAssign(Node *left, Node *right) {
    tk.tokenType = TK_ASSIGN;
    this->left = left;
    this->right = right;
}

void NodeArrayIndex::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (parent != NULL)
            parent->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), "[ ]");
        if (child != NULL)
            child->print(prefix + "\t");
    }
}

void NodeRecordAccess::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (left != NULL)
            left->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), ".");
        if (right != NULL)
            right->print(prefix + "\t");
    }
}

void NodeFunc::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (parent != NULL)
            parent->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), "( )");
        for (auto i = params.begin(); i != params.end(); i++)
            (*i)->print(prefix + "\t");
    }
}

void NodeCast::print(string prefix) {
        if (parameter != NULL)
            parameter->print(prefix + "(" + symbol->name.c_str() + ")" + "\t");
}

void NodeAssign::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (left != NULL)
            left->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), ":=");
        if (right != NULL)
            right->print(prefix + "\t");
    }
}
