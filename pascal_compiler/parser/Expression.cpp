//
// Created by xtpvx on 07.10.2015.
//

#include "Expression.h"

void Expression::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        if (left != NULL)
            left->print(prefix + "\t\t");
        printf("%sexpr\n", prefix.c_str());
        if (right != NULL)
            right->print(prefix + "\t\t");
    }
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

void ExprIntConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        string data = tk.getStr();
        int tabCount = (int)data.length() / 4;
        string tabs(tabCount + 1, '\t');
        if (left != NULL)
            left->print(prefix + tabs);
        printf("%s%s\n", prefix.c_str(), data.c_str());
        if (right != NULL)
            right->print(prefix + tabs);
    }
}

void ExprRealConst::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        string data = tk.getStr();
        int tabCount = (int)data.length() / 4;
        string tabs(tabCount + 1, '\t');
        if (left != NULL)
            left->print(prefix + tabs);
        printf("%s%s\n", prefix.c_str(), data.c_str());
        if (right != NULL)
            right->print(prefix + tabs);
    }
}

void ExprVariable::print(string prefix) {
    if (tk.tokenType != TK_ERROR) {
        int tabCount = (int)tk.text.length() / 4;
        string tabs(tabCount + 1, '\t');
        if (left != NULL)
            left->print(prefix + tabs);
        printf("%s%s\n", prefix.c_str(), tk.text.c_str());
        if (right != NULL)
            right->print(prefix + tabs);
    }
}

ExprBinary::ExprBinary(Token tk, Expression* left, Expression* right) {
    Expression::tk = tk;
    Expression::left = left;
    Expression::right = right;
}

ExprIntConst::ExprIntConst(Token tk) {
    Expression::tk = tk;
}

ExprRealConst::ExprRealConst(Token tk) {
    Expression::tk = tk;
}

ExprVariable::ExprVariable(Token tk) {
    Expression::tk = tk;
}