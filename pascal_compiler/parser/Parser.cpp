//
// Created by xtpvx on 07.10.2015.
//

#include "Parser.h"
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

NodeBlock *Parser::Parse() {
    checkEof();
    return ParseBlock();
}

NodeBlock *Parser::ParseBlock() {
    checkEof();
    Token beg = tokenizer.Get();
    if (tokenizer.Get().tokenType != TK_BEGIN)
        throw SyntaxException(beg.strNum, beg.strPos, "required 'begin' token");
    tokenizer.Next();
    std::list<Node *> lines;
    lines.push_back(ParseAssign());
    if (tokenizer.Get().tokenType != TK_END)
        throw SyntaxException(beg.strNum, beg.strPos, "required 'end' token");
    return new NodeBlock(lines);
}


Node *Parser::ParseAssign() {
    checkEof();
    Node *result = ParseBinary(0);
    if (tokenizer.Get().tokenType == TK_ASSIGN) {
        tokenizer.Next();
        result = new NodeAssign(result, ParseBinary(0));
    }
    return result;
}

Node *Parser::ParseBinary(int level) {
    checkEof();
    if (level >= ARRAY_SIZE(priority))
        return ParseUnary();
    Node *left = ParseBinary(level + 1);
    Token op = tokenizer.Get();
    while (checkToken(op.tokenType, priority[level])) {
        tokenizer.Next();
        left = new ExprBinary(op, left, ParseBinary(level + 1));
        op = tokenizer.Get();
    }
    return left;
}


Node *Parser::ParseUnary() {
    checkEof();
    Token op = tokenizer.Get();
    switch (op.tokenType) {
        case TK_ADD:
        case TK_SUB: {
            tokenizer.Next();
            Node *right = ParseFunction();
            return new ExprUnary(op, right);
        }
        default:
            return ParseFunction();
    }
}

Node *Parser::ParseFunction() {
    checkEof();
    Node *result = ParseRecordAccess();
    if (tokenizer.Get().tokenType == TK_LB) {
        tokenizer.Next();
        auto params = ParseParameters();
        result = new NodeFunc(result, params);
        if (tokenizer.Get().tokenType != TK_RB)
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required ')' token");
        tokenizer.Next();
    }
    return result;
}

Node *Parser::ParseRecordAccess() {
    checkEof();
    Node *result = ParseArrayIndex();
    while (tokenizer.Get().tokenType == TK_POINT) {
        tokenizer.Next();
        Node *right = ParseArrayIndex();
        result = new NodeRecordAccess(result, right);
    }
    return result;
}

Node *Parser::ParseArrayIndex() {
    checkEof();
    Node *result = ParseFactor();
    if (tokenizer.Get().tokenType == TK_LSQB) {
        tokenizer.Next();
        auto params = ParseParameters();
        if (params.size() > 0)
            for (auto i = params.begin(); i != params.end(); i++) {
                result = new NodeArrayIndex(result, *i);
            }
        else
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required array index");
        if (tokenizer.Get().tokenType != TK_RSQB)
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required ',' or ']' token");
        tokenizer.Next();
    }
    return result;
}


std::list<Node *> Parser::ParseParameters() {
    std::list<Node *> result;
    Node *item;
    try {
        item = ParseBinary(0);
    } catch (SyntaxException e) {
        if (e.getErrorText().compare("unexpected token") == 0)
            return result;
        else throw;
    }
    result.push_back(item);
    while (tokenizer.Get().tokenType == TK_COMMA) {
        tokenizer.Next();
        item = ParseBinary(0);
        result.push_back(item);
    }
    return result;
}

Node *Parser::ParseFactor() {
    checkEof();
    Token factor = tokenizer.Get();
    Node *result;
    if (factor.tokenType == TK_LB) {
        result = ParseBinary(0);
        if (tokenizer.Get().tokenType != TK_RB)
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required ',' or ')' token");
    } else {
        if (factor.tokenType == TK_ID)
            result = new NodeVariable(factor);
        else if (factor.tokenType == TK_INT)
            result = new NodeIntConst(factor);
        else if (factor.tokenType == TK_REAL)
            result = new NodeRealConst(factor);
        else {
            throw SyntaxException(factor.strNum, factor.strPos, "unexpected token");
        }
    }
    tokenizer.Next();
    return result;
}

void Parser::checkEof() {
    if(tokenizer.Eof())
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "unexpected end of file");
}
