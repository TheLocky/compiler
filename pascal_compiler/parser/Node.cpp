//
// Created by xtpvx on 07.10.2015.
//

#include "Node.h"

ExprBinary::ExprBinary(Token tk, NodeExpr * left, NodeExpr * right) {
    operatorToken = tk;
    this->left = left;
    this->right = right;
    type = left->type;
}

ExprUnary::ExprUnary(Token tk, NodeExpr *right) {
    operatorToken = tk;
    this->right = right;
    type = right->type;
}

ExprIntConst::ExprIntConst(Token tk, Symbols::SymType *symbol) {
    operatorToken = tk;
    type = symbol;
}

ExprRealConst::ExprRealConst(Token tk, Symbols::SymType *symbol) {
    operatorToken = tk;
    type = symbol;
}

ExprStringConst::ExprStringConst(Token tk, Symbols::SymType *symbol) {
    operatorToken = tk;
    type = symbol;
}

ExprCharConst::ExprCharConst(Token tk, Symbols::SymType *symbol) {
    operatorToken = tk;
    type = symbol;
}

ExprBooleanConst::ExprBooleanConst(Token tk, Symbols::SymType *symbol) {
    operatorToken = tk;
    type = symbol;
}

ExprVariable::ExprVariable(Token tk, Symbols::SymType *varType) {
    operatorToken = tk;
    type = varType;
}

NodeArrayIndex::NodeArrayIndex(NodeExpr *parent, NodeExpr *child, Symbols::SymType *arrType) {
    operatorToken.tokenType = TK_LSQB;
    this->parent = parent;
    this->child = child;
    type = arrType;
}

NodeRecordAccess::NodeRecordAccess(NodeExpr *left, NodeExpr *right) {
    operatorToken.tokenType = TK_POINT;
    this->left = left;
    this->right = right;
    type = right->type;
}

NodeFunc::NodeFunc(NodeExpr *parent, std::vector<NodeExpr*> params, Symbols::SymType *retType) {
    operatorToken.tokenType = TK_LB;
    this->parent = parent;
    this->params = params;
    type = retType;
}

NodeCast::NodeCast(NodeExpr *parameter, Symbols::SymType *castType) : parameter(parameter) {
    type = castType;
}

NodeAssign::NodeAssign(NodeExpr *left, NodeExpr *right) {
    this->left = left;
    this->right = right;
}

void ExprBinary::print(string prefix) {
    if (left != NULL)
        left->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), operatorToken.text.c_str());
    if (right != NULL)
        right->print(prefix + "\t");
}

void ExprUnary::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.text.c_str());
    if (right != NULL)
        right->print(prefix + "\t");
}

void ExprIntConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprRealConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprStringConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprCharConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprBooleanConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprVariable::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.text.c_str());
}

void NodeArrayIndex::print(string prefix) {
    if (parent != NULL)
        parent->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), "[ ]");
    if (child != NULL)
        child->print(prefix + "\t");
}

void NodeRecordAccess::print(string prefix) {
    if (left != NULL)
        left->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), ".");
    if (right != NULL)
        right->print(prefix + "\t");
}

void NodeFunc::print(string prefix) {
    if (parent != NULL)
        parent->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), "( )");
    for (auto i = params.begin(); i != params.end(); i++)
        (*i)->print(prefix + "\t");
}

void NodeCast::print(string prefix) {
    if (parameter != NULL) {
        printf((prefix + "(" + type->name.c_str() + ")\n").c_str());
        parameter->print(prefix + "\t");
    }
}

void NodeAssign::print(string prefix) {
    if (left != NULL)
        left->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), ":=");
    if (right != NULL)
        right->print(prefix + "\t");
}
