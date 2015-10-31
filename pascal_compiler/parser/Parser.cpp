//
// Created by xtpvx on 07.10.2015.
//

#include "Parser.h"
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

static std::list<LEX_TYPE> priority[] = {
        lexList(TK_ADD, TK_SUB, 0),
        lexList(TK_MUL, TK_DIV, 0),
        lexList(TK_CGT, TK_CLT, TK_CGE, TK_CLE, TK_CNE, TK_CE, TK_AND, TK_OR, 0)
};

NodeBlock *Parser::Parse() {
    while (checkToken(tokenizer.Get().tokenType, lexList(TK_TYPE, 0))) {
        tokenizer.Next();
        ParseTypeBlock();
    }
    return ParseBlock();
}

NodeBlock *Parser::ParseBlock() {
    requireToken(TK_BEGIN, "begin");
    tokenizer.Next();
    std::list<Node *> lines;
    lines.push_back(ParseAssign());
    requireToken(TK_END, "end");
    return new NodeBlock(lines);
}

Node *Parser::ParseAssign() {
    Node *result = ParseBinary(0);
    if (tokenizer.Get().tokenType == TK_ASSIGN) {
        tokenizer.Next();
        Node *right = ParseBinary(0);
        thExpectedExpr(right);
        result = new NodeAssign(result, right);
    }
    return result;
}

Node *Parser::ParseBinary(int level) {
    if (level >= ARRAY_SIZE(priority))
        return ParseFactor();
    Node *left = ParseBinary(level + 1);
    Token op = tokenizer.Get();
    while (checkToken(op.tokenType, priority[level])) {
        tokenizer.Next();
        Node *right = ParseBinary(level + 1);
        thExpectedExpr(right);
        left = new ExprBinary(op, left, right);
        op = tokenizer.Get();
    }
    return left;
}

std::vector<Node *> Parser::ParseParameters() {
    std::vector<Node *> result;
    Node *item = ParseBinary(0);
    if (item == nullptr)
        return result;
    result.push_back(item);
    while (tokenizer.Get().tokenType == TK_COMMA) {
        tokenizer.Next();
        item = ParseBinary(0);
        thExpectedExpr(item);
        result.push_back(item);
    }
    return result;
}

Node *Parser::ParseFactor() {
    Token factor = tokenizer.Get();
    if (tokenizer.Eof())
        throw SyntaxException(factor.strNum, factor.strPos, "unexpected EOF");
    Node *result;
    if (factor.tokenType == TK_LB) {
        tokenizer.Next();
        result = ParseBinary(0);
        requireToken(TK_RB, ")");
        tokenizer.Next();
    } else {
        if (checkToken(factor.tokenType, lexList(TK_ADDR, TK_NOT, TK_ADD, TK_SUB, 0))) {
            tokenizer.Next();
            result = ParseFactor();
            result = new ExprUnary(factor, result);
        }
        else if (factor.tokenType == TK_ID) {
            result = ParseDesignator();
        }
        else if (factor.tokenType == TK_INT) {
            result = new NodeIntConst(factor);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_REAL) {
            result = new NodeRealConst(factor);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_STRING) {
            result = new NodeStringConst(factor);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_CHAR) {
            result = new NodeCharConst(factor);
            tokenizer.Next();
        }
        else {
            return nullptr;
        }
    }
    return result;
}

Node *Parser::ParseDesignator() {
    Node *result = new NodeVariable(tokenizer.Get());
    Token tk = tokenizer.Next();
    while (checkToken(tk.tokenType, lexList(TK_LB, TK_LSQB, TK_POINT, TK_POINTER, 0))) {
        if (tk.tokenType == TK_LB) {
            tokenizer.Next();
            auto params = ParseParameters();
            requireToken(TK_RB, ")");
            result = new NodeFunc(result, params);
        } else if (tk.tokenType == TK_LSQB) {
            tokenizer.Next();
            auto params = ParseParameters();
            requireToken(TK_RSQB, "]");
            if (params.size() > 0)
                for (int i = 0; i < params.size(); i++) {
                    result = new NodeArrayIndex(result, params[i]);
                }
            else
                throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required array index");
        } else if (tk.tokenType == TK_POINT) {
            tokenizer.Next();
            requireToken(TK_ID, "<identificator>");
            Node *right = new NodeVariable(tokenizer.Get());
            result = new NodeRecordAccess(result, right);
        } else {
            result = new ExprUnary(tk, result);
        }
        tk = tokenizer.Next();
    }
    return result;
}

void Parser::requireToken(LEX_TYPE tok, string tokStr) {
    Token current = tokenizer.Get();
    if (current.tokenType != tok) {
        if (tokenizer.Eof())
            throw SyntaxException(current.strNum, current.strPos, "unexpected EOF");
        throw SyntaxException(current.strNum, current.strPos,
                                  "unexpected token '" + current.text + "' but required '" + tokStr + "'");
    }
}

void Parser::thExpectedExpr(Node *node) {
    if (node == nullptr)
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "expected expression");
}

void Parser::ParseTypeBlock() {
    Token name = tokenizer.Get();
    while (name.tokenType == TK_ID) {
        tokenizer.Next();
        requireToken(TK_CE, "=");
        Symbol *type = ParseType(name.text);
        symTable.addSymbol(type);
        requireToken(TK_SEMICOLON, ";");
        name = tokenizer.Next();
    }
}

SymType *Parser::ParseType(string name) {
    Token typeName = tokenizer.Next();
    if (typeName.tokenType == TK_ID) {
        tokenizer.Next();
        if (symTable.contains(typeName.text)) {
            return new SymAlias(name, (SymType*)symTable.getSymbol(typeName.text));
        } else
            throw SyntaxException(typeName.strNum, typeName.strPos, string("unknown type '") + typeName.text + "'");
    } else if(typeName.tokenType == TK_ARRAY) {
        tokenizer.Next();
        bool dynamic = true;
        SymType *indexType = (SymType*)symTable.getSymbol("Integer");
        if (tokenizer.Get().tokenType == TK_LSQB) {
            tokenizer.Next();
            indexType = ParseTypeSubRange("anon");
            dynamic = false;
            requireToken(TK_RSQB, "]");
            tokenizer.Next();
        }
        SymType *arrayType = ParseType("anon");
        return new SymArray(name, arrayType, indexType, dynamic);
    } else if (checkToken(typeName.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
        return ParseTypeSubRange(name);
    } else if (typeName.tokenType == TK_POINTER) {
        return new SymTypePointer(name, ParseType("anon"));
    }
    else
        throw SyntaxException(typeName.strNum, typeName.strPos, "expected type declaration");
}

SymTypeSubRange *Parser::ParseTypeSubRange(string name) {
    SymConst *left = ParseConstantExpr("anon", lexList(TK_INT, TK_CHAR, 0));
    requireToken(TK_DBLPOINT, "..");
    tokenizer.Next();
    SymConst *right = ParseConstantExpr("anon", lexList(TK_INT, TK_CHAR, 0));
    if (left->type != right->type)
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                              "const expression in range must be equal type");
    return new SymTypeSubRange(name, left, right);
}

SymConst *Parser::ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes) {
    Token an = tokenizer.Get();
    tokenizer.Next();
    if (allowTypes.size() > 0 && !checkToken(an.tokenType, allowTypes))
        throw SyntaxException(an.strNum, an.strPos, "unexpected const type");
    switch (an.tokenType) {
        case TK_INT:
            return new SymConst(name, (SymType*)symTable.getSymbol("Integer"), an);
        case TK_REAL:
            return new SymConst(name, (SymType*)symTable.getSymbol("Double"), an);
        case TK_CHAR:
            return new SymConst(name, (SymType*)symTable.getSymbol("Char"), an);
        case TK_STRING:
            return new SymConst(name, (SymType*)symTable.getSymbol("String"), an);
        default:
            throw SyntaxException(an.strNum, an.strPos, "required const expression");
    }
}

SymTable Parser::getSymbolsTable() const {
    return symTable;
}
