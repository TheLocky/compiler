//
// Created by xtpvx on 07.10.2015.
//

#include "Node.h"

void Node::print(string prefix) {
    printf("%sempty node", prefix.c_str());
}

void NodeBlock::print(string prefix) {
    for (auto i = lines.begin(); i != lines.end(); i++)
        (*i)->print(prefix);
}

ExprBinary::ExprBinary(Token tk, Node * left, Node * right) {
    Node::tk = tk;
    this->left = left;
    this->right = right;
}

ExprUnary::ExprUnary(Token tk, Node *right) {
    Node::tk = tk;
    this->right = right;
}

NodeIntConst::NodeIntConst(Token tk) {
    Node::tk = tk;
}

NodeRealConst::NodeRealConst(Token tk) {
    Node::tk = tk;
}

NodeVariable::NodeVariable(Token tk) {
    Node::tk = tk;
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

void NodeVariable::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
    }
}

NodeArrayIndex::NodeArrayIndex(Node *parent, Node *child) {
    tk.tokenType = TK_LSQB;
    this->parent = parent;
    this->child = child;
}

NodeRecordAccess::NodeRecordAccess(Node *left, Node *right) {
    tk.tokenType = TK_POINT;
    this->left = left;
    this->right = right;
}

NodeFunc::NodeFunc(Node *parent, std::list<Node*> params) {
    tk.tokenType = TK_LB;
    this->parent = parent;
    this->params = params;
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

void NodeAssign::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (left != NULL)
            left->print(prefix + "\t");
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
        if (right != NULL)
            right->print(prefix + "\t");
    }
}
