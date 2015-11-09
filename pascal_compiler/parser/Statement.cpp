//
// Created by xtpvx on 05.11.2015.
//

#include "Statement.h"

void StmtCase::add(Token left, Token right, Statement *stmt) {
    Case *tmp = new Case();
    tmp->left = left;
    tmp->right = right;
    tmp->statement = stmt;
    cases.push_back(tmp);
}

void StmtExpression::print(string prefix) {
    expression->print(prefix);
}

void StmtCompound::print(string prefix) {
    printf(string(prefix + "====== BEGIN BLOCK:\n").c_str());
    for (Statement *stmt : stmtList) {
        stmt->print(prefix + "\t");
    }
    printf(string(prefix + "====== END BLOCK\n").c_str());
}

void StmtIf::print(string prefix) {
    printf(string(prefix + "====== IF:\n").c_str());
    condition->print(prefix + "\t");
    printf(string(prefix + "====== THEN:\n").c_str());
    then->print(prefix + "\t");
    if (otherwise != nullptr) {
        printf(string(prefix + "====== ELSE:\n").c_str());
        otherwise->print(prefix + "\t");
    }
    printf(string(prefix + "====== ENDIF\n").c_str());
}

void StmtCase::print(string prefix) {

}

void StmtFor::print(string prefix) {

}

void StmtWhile::print(string prefix) {

}

void StmtRepeat::print(string prefix) {

}
