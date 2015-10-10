//
// Created by xtpvx on 07.10.2015.
//

#include "Parser.h"


Expression *Parser::ParseExpr() {
    if (!tokenizer.Eof()) {
        Expression *left = ParseTerm();
        Token op = tokenizer.Get();
        while (validateToken(op.tokenType, TK_ADD, TK_SUB, 0)) {
            tokenizer.Next();
            Expression *right = ParseTerm();
            ExprBinary *binary = new ExprBinary(op, left, right);
            left = (Expression*)binary;
            op = tokenizer.Get();
        }
        return left;
    } else {
        //TODO: eof error
        return nullptr;
    }
}

Expression *Parser::ParseTerm() {
    if (!tokenizer.Eof()) {
        Expression *left = ParseFactor();
        Token op = tokenizer.Next();
        while (validateToken(op.tokenType, TK_MUL, TK_DIV, 0)) {
            tokenizer.Next();
            Expression *right = ParseFactor();
            ExprBinary *binary = new ExprBinary(op, left, right);
            left = (Expression*)binary;
            op = tokenizer.Next();
        }
        return left;
    } else {
        //TODO: eof error
        return nullptr;
    }
}

Expression *Parser::ParseFactor() {
    if (!tokenizer.Eof()) {
        Token factor = tokenizer.Get();
        if (factor.tokenType == TK_LB) {
            tokenizer.Next();
            Expression *result = ParseExpr();
            Token reqLB = tokenizer.Get();
            if (reqLB.tokenType != TK_RB)
                throw SyntaxException(reqLB.strNum, reqLB.strPos, "required ')' token");
            return result;
        } else {
            if (factor.tokenType == TK_ID)
                return ((Expression*) new ExprVariable(factor));
            else if (factor.tokenType == TK_INT)
                return ((Expression*) new ExprIntConst(factor));
            else if (factor.tokenType == TK_REAL)
                return ((Expression*) new ExprRealConst(factor));
            else {
                //TODO: factor error
                return nullptr;
            }
        }
    } else {
        //TODO: eof error
        return nullptr;
    }
}
