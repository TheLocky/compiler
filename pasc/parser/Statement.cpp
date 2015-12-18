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

void StmtExpression::generate(AsmCode * code) {
}

void StmtCompound::print(string prefix) {
    printf(string(prefix + "====== BEGIN BLOCK:\n").c_str());
    for (Statement *stmt : stmtList) {
        stmt->print(prefix + "\t");
    }
    printf(string(prefix + "====== END BLOCK\n").c_str());
}

void StmtCompound::generate(AsmCode * code) {
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
    printf(string(prefix + "====== END IF\n").c_str());
}

void StmtIf::generate(AsmCode * code) {
}

void StmtCase::print(string prefix) {
    printf(string(prefix + "====== CASE:\n").c_str());
    condition->print(prefix + "\t");
    printf(string(prefix + "====== OF:\n").c_str());
    for (auto caseLabel : cases) {
        string right = caseLabel->right.tokenType != TK_ERROR ? string(" .. ") + caseLabel->right.text : "";
        printf("\t%s====== %s%s:\n", prefix.c_str(), caseLabel->left.text.c_str(), right.c_str());
        caseLabel->statement->print(prefix + "\t\t");
    }
    if (otherwise != nullptr) {
        printf(string(prefix + "\t====== ELSE:\n").c_str());
        otherwise->print(prefix + "\t\t");
    }
    printf(string(prefix + "====== END CASE\n").c_str());
}

void StmtCase::generate(AsmCode * code) {
}

void StmtFor::print(string prefix) {
    printf(string(prefix + "====== FOR:\n").c_str());
    printf(string(prefix + "\t====== VARIABLE: " + variable->name + "\n").c_str());
    printf(string(prefix + "\t====== INIT:\n").c_str());
    initExpr->print(prefix + "\t\t");
    printf(string(prefix + "\t====== " + (downto ? "DOWNTO" : "TO") + ":\n").c_str());
    resExpr->print(prefix + "\t\t");
    printf(string(prefix + "\t====== DO:\n").c_str());
    statement->print(prefix + "\t\t");
    printf(string(prefix + "====== END FOR\n").c_str());
}

void StmtFor::generate(AsmCode * code) {
}

void StmtWhile::print(string prefix) {
    printf(string(prefix + "====== WHILE:\n").c_str());
    condition->print(prefix + "\t");
    printf(string(prefix + "====== DO:\n").c_str());
    statement->print(prefix + "\t");
    printf(string(prefix + "====== END WHILE\n").c_str());
}

void StmtWhile::generate(AsmCode * code) {
}

void StmtRepeat::print(string prefix) {
    printf(string(prefix + "====== REPEAT:\n").c_str());
    statement->print(prefix + "\t");
    printf(string(prefix + "====== UNTIL:\n").c_str());
    condition->print(prefix + "\t");
    printf(string(prefix + "====== END REPEAT\n").c_str());
}

void StmtRepeat::generate(AsmCode * code) {
}


void StmtBreak::print(string prefix) {
    string loopType = "ERROR";
    if (dynamic_cast<StmtFor*>(parent)) loopType = "FOR LOOP";
    if (dynamic_cast<StmtWhile*>(parent)) loopType = "WHILE LOOP";
    if (dynamic_cast<StmtRepeat*>(parent)) loopType = "REPEAT LOOP";
    printf(string(prefix + "====== BREAK " + loopType + "\n").c_str());
}

void StmtBreak::generate(AsmCode * code) {
}

void StmtContinue::print(string prefix) {
    string loopType = "ERROR";
    if (dynamic_cast<StmtFor*>(parent)) loopType = "FOR LOOP";
    if (dynamic_cast<StmtWhile*>(parent)) loopType = "WHILE LOOP";
    if (dynamic_cast<StmtRepeat*>(parent)) loopType = "REPEAT LOOP";
    printf(string(prefix + "====== CONTINUE " + loopType + "\n").c_str());
}

void StmtContinue::generate(AsmCode * code) {
}
