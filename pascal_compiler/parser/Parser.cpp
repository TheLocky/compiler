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

static Type simpleCastRules[TypeBool+1][TypeBool+1] = {
//        int         double        char       string      bool
        {TypeInt,   TypeDouble,   TypeChar,   TypeEnd,    TypeBool},    //int
        {TypeInt,   TypeDouble,   TypeEnd,    TypeEnd,    TypeEnd},     //double
        {TypeInt,   TypeEnd,      TypeChar,   TypeString, TypeEnd},     //char
        {TypeEnd,   TypeEnd,      TypeEnd,    TypeString, TypeEnd},     //string
        {TypeInt,   TypeEnd,      TypeEnd,    TypeEnd,    TypeBool}     //bool
};

static bool checkTypeAllow = true;

Node *Parser::ParseAssign(bool mustBe) {
    Node *result = ParseBinary(0);
    Token assign = tokenizer.Get();
    if (mustBe) requireToken(TK_ASSIGN, ":=");
    if (assign.tokenType == TK_ASSIGN) {
        tokenizer.Next();
        Node *right = ParseBinary(0);
        thExpectedExpr(right);
        Symbol *symb = result->getSymbol();
        if (checkTypeAllow) {
            NodeCast *cast = createCast(symb, right);
            if (cast != nullptr) right = cast;
        }
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
        if (checkToken(op.tokenType, lexList(TK_AND, TK_OR, 0))) { //bool
            Symbol *b = symTable.getTypeSymbol("Bool");
            NodeCast *leftCast = createCast(b, left);
            if (leftCast != nullptr) left = leftCast;
            NodeCast *rightCast = createCast(b, right);
            if (rightCast != nullptr) right = rightCast;
        } else {
            bool leftCasted;
            NodeCast *cast = createCast(left, right, &leftCasted);
            if (cast != nullptr) { if (leftCasted) left = cast; else right = cast; }
        }
        ExprBinary *l = new ExprBinary(op, left, right);
        if (level == 2) {
            l->setSymbol(symTable.getTypeSymbol("Bool"));
        }
        left = l;
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
            result = new NodeIntConst(factor, symTable.getTypeSymbol("Integer"));
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_REAL) {
            result = new NodeRealConst(factor, symTable.getTypeSymbol("Double"));
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_STRING) {
            result = new NodeStringConst(factor, symTable.getTypeSymbol("String"));
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_CHAR) {
            result = new NodeCharConst(factor, symTable.getTypeSymbol("Char"));
            tokenizer.Next();
        }
        else {
            return nullptr;
        }
    }
    return result;
}

Node *Parser::ParseDesignator() {
    Token ident = tokenizer.Get();
    Symbol *symbol = (Symbol*)symTable.getVarSymbol(ident.text);
    if (!symTable.containsVar(ident.text) && checkTypeAllow)
        throw SyntaxException(ident.strNum, ident.strPos, "undefined variable");
    Node *result = new NodeVariable(ident, symTable.getVarSymbol(ident.text));
    Token tk = tokenizer.Next();
    while (checkToken(tk.tokenType, lexList(TK_LB, TK_LSQB, TK_POINT, TK_POINTER, 0))) {
        if (tk.tokenType == TK_LB) {
            tokenizer.Next();
            auto params = ParseParameters();
            requireToken(TK_RB, ")");
            result = new NodeFunc(result, params);
        } else if (tk.tokenType == TK_LSQB) {
            tokenizer.Next();
            symbol = symbol->toVar()->type;
            auto params = ParseParameters();
            requireToken(TK_RSQB, "]");
            if (params.size() > 0)
                for (int i = 0; i < params.size(); i++) {
                    if (symbol->typeId != TypeArray)
                        throw SyntaxException(tk.strNum, tk.strPos, "operator [] required Array type variable");
                    auto indexCast = createCast(symbol->toArray()->indexType, params[i], "invalid index type");
                    if (indexCast != nullptr)
                        result = new NodeArrayIndex(result, indexCast);
                    else
                        result = new NodeArrayIndex(result, params[i]);
                    result->symbol = symbol->toArray()->arrayType;
                    symbol = symbol->toArray()->arrayType;
                }
            else
                throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "required array index");
        } else if (tk.tokenType == TK_POINT) {
            tokenizer.Next();
            requireToken(TK_ID, "<identificator>");
            //TODO: record type parse
            Node *right = new NodeVariable(tokenizer.Get(), nullptr);
            result = new NodeRecordAccess(result, right);
        } else {
            result = new ExprUnary(tk, result);
        }
        tk = tokenizer.Next();
    }
    return result;
}

void Parser::ParseTypeBlock() {
    Token name = tokenizer.Get();
    while (name.tokenType == TK_ID) {
        tokenizer.Next();
        requireToken(TK_CE, "=");
        if (symTable.containsType(name.text))
            throw SyntaxException(name.strNum, name.strPos, string("type'") + name.text + "'is already defined");
        tokenizer.Next();
        symTable.addTypeSymbol(ParseType(name.text, true));
        requireToken(TK_SEMICOLON, ";");
        name = tokenizer.Next();
    }
}

SymType *Parser::ParseType(string name, bool createAlias) {
    Token typeName = tokenizer.Get();
    if (typeName.tokenType == TK_ID) {
        tokenizer.Next();
        if (symTable.containsType(typeName.text)) {
            if (createAlias)
                return new SymAlias(name, symTable.getTypeSymbol(typeName.text));
            else
                return symTable.getTypeSymbol(typeName.text);
        } else
            throw SyntaxException(typeName.strNum, typeName.strPos, string("unknown type '") + typeName.text + "'");
    } else if(typeName.tokenType == TK_ARRAY) {
        return ParseTypeArray(name);
    } else if (checkToken(typeName.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
        return ParseTypeSubRange(name);
    } else if (typeName.tokenType == TK_POINTER) {
        return new SymTypePointer(name, ParseType("anon", false));
    } else
        throw SyntaxException(typeName.strNum, typeName.strPos, "expected type declaration");
}

SymArray *Parser::ParseTypeArray(string name) {
    requireToken(TK_ARRAY, "Array");
    tokenizer.Next();
    bool dynamic = true;
    SymType *indexType = symTable.getTypeSymbol("Integer");
    if (tokenizer.Get().tokenType == TK_LSQB) {
        tokenizer.Next();
        indexType = ParseType("anon", false);
        if (!SymTable::compareTypes(indexType, TypeInt) &&
                !SymTable::compareTypes(indexType, TypeChar))
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "invalid array index type");
        dynamic = false;
        requireToken(TK_RSQB, "]");
        tokenizer.Next();
    }
    requireToken(TK_OF, "of");
    tokenizer.Next();
    if (tokenizer.Get().tokenType == TK_ARRAY)
        return new SymArray(name, ParseTypeArray("anon"), indexType, dynamic);
    return new SymArray(name, ParseType("anon", false), indexType, dynamic);
}

SymTypeSubRange *Parser::ParseTypeSubRange(string name) {
    SymConst *left = ParseConstantExpr("anon", lexList(TK_INT, TK_CHAR, 0));
    requireToken(TK_DBLPOINT, "..");
    tokenizer.Next();
    SymConst *right = ParseConstantExpr("anon", lexList(TK_INT, TK_CHAR, 0));
    if (left->type->typeId != right->type->typeId)
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                              "const expression in range must be equal type");
    if (left->value.intData >= right->value.intData)
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                              "right value must be greather then left value");
    return new SymTypeSubRange(name, left, right);
}

SymConst *Parser::ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes) {
    Token an = tokenizer.Get();
    tokenizer.Next();
    if (allowTypes.size() > 0 && !checkToken(an.tokenType, allowTypes))
        throw SyntaxException(an.strNum, an.strPos, "unexpected const type");
    switch (an.tokenType) {
        case TK_INT:
            return new SymConst(name, symTable.getTypeSymbol("Integer"), an);
        case TK_REAL:
            return new SymConst(name, symTable.getTypeSymbol("Double"), an);
        case TK_CHAR:
            return new SymConst(name, symTable.getTypeSymbol("Char"), an);
        case TK_STRING:
            return new SymConst(name, symTable.getTypeSymbol("String"), an);
        default:
            throw SyntaxException(an.strNum, an.strPos, "required const expression");
    }
}

NodeCast *Parser::createCast(Node *left, Node *right, bool *leftUsed) {
    return createCast(left, right, leftUsed, "invalid type cast");
}

NodeCast *Parser::createCast(Node *left, Node *right, bool *leftUsed, string err) {
    NodeCast *result = createCast(left->getSymbol(), right, err);
    if (result) {
        *leftUsed = false;
        return result;
    }
    result = createCast(right->getSymbol(), left, err);
    if (result) {
        *leftUsed = true;
        return result;
    }
    return nullptr;
}

NodeCast *Parser::createCast(Symbol *type, Node *node) {
    return createCast(type, node, "invalid type cast");
}

NodeCast *Parser::createCast(Symbol *type, Node *node, string err) {
    if (!checkTypeAllow)
        return nullptr;

    Symbol *castType = type;
    Symbol *nodeType = node->getSymbol();

    if (castType->isVar())
        castType = castType->toVar()->type;
    if (nodeType->isVar())
        nodeType = nodeType->toVar()->type;
    if (castType->isConst())
        castType = castType->toConst()->type;
    if (nodeType->isConst())
        nodeType = nodeType->toConst()->type;

    while (castType->isAlias())
        castType = castType->toAlias()->type;
    while (nodeType->isAlias())
        nodeType = nodeType->toAlias()->type;

    if (castType->isArray() && nodeType->isArray()) {
        if (SymTable::compareTypes(castType, nodeType))
            return nullptr;
        else
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                                  err + ": different Array types");
    }
    else if (castType->isPointer() && nodeType->isPointer()) {
        if (SymTable::compareTypes(castType, nodeType))
            return nullptr;
        else
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                                  err + ": different Pointer types");
    }
    else if (castType->isSubRange() && nodeType->isSubRange()) {
        if (SymTable::compareTypes(castType, nodeType))
            return nullptr;
        else
            throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos,
                                  err + ": different SubRange types");
    } else {
        if (castType->isSubRange())
            castType = castType->toSubRange()->left->type;

        if (castType->typeId <= TypeBool && nodeType->typeId <= TypeBool) {
            if (nodeType->typeId == castType->typeId)
                return nullptr;
            else if (simpleCastRules[nodeType->typeId][castType->typeId] != TypeEnd)
                return new NodeCast(castType, node);
        }
    }
    throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, err);
}

SymTable Parser::getSymbolsTable() const {
    return symTable;
}

void Parser::ParseConstBlock() {
    Token name = tokenizer.Get();
    while (name.tokenType == TK_ID) {
        tokenizer.Next();
        requireToken(TK_CE, "=");
        tokenizer.Next();
        symTable.addVarSymbol(ParseConstantExpr(name.text, {}));
        requireToken(TK_SEMICOLON, ";");
        name = tokenizer.Next();
    }
}

void Parser::ParseVarBlock() {
    auto namesList = ParseIdentList();
    while (namesList.size() > 0) {
        requireToken(TK_COLON, ":");
        Token typeName = tokenizer.Next();
        SymType *type = ParseType("anon", false);
        for (auto name : namesList)
            symTable.addVarSymbol(new SymVar(name.text, type));
        requireToken(TK_SEMICOLON, ";");
        tokenizer.Next();
        namesList = ParseIdentList();
    }
}

std::vector<Token> Parser::ParseIdentList() {
    std::vector<Token> result;
    Token name = tokenizer.Get();
    while (name.tokenType == TK_ID) {
        result.push_back(name);
        if (tokenizer.Next().tokenType != TK_COMMA)
            return result;
        name = tokenizer.Next();
    }
}

void Parser::ParseDeclSection() {
    Token declSection = tokenizer.Get();
    if (declSection.text == "skipTypeCheck") {
        checkTypeAllow = false;
        declSection = tokenizer.Next();
    }
    while (checkToken(declSection.tokenType, lexList(TK_TYPE, TK_CONST, TK_VAR, 0))) {
        tokenizer.Next();
        if (declSection.tokenType == TK_TYPE)
            ParseTypeBlock();
        else if (declSection.tokenType == TK_CONST)
            ParseConstBlock();
        else if (declSection.tokenType == TK_VAR)
            ParseVarBlock();
        declSection = tokenizer.Get();
    }
}

Statement *Parser::ParseStatement(Statement *parent) {
    Statement *result;
    Token tk = tokenizer.Get();
    if (tk.tokenType == TK_IF) {
        tokenizer.Next();
        auto condition = ParseBinary(0);
        auto cast = createCast(symTable.getTypeSymbol("Bool"), condition, "condition must be Bool type");
        if (cast != nullptr) condition = cast;
        requireToken(TK_THEN, "Then");
        tokenizer.Next();
        auto tmp = new StmtIf(parent);
        tmp->condition = (ExprBinary*)condition;
        Statement *thenBody = ParseStatement(tmp);
        tmp->then = thenBody;
        if (tokenizer.Get().tokenType == TK_ELSE) {
            tokenizer.Next();
            Statement *elseBody = ParseStatement(tmp);
            tmp->otherwise = elseBody;
        } else
            tmp->otherwise = nullptr;
        result = tmp;
    } else if (tk.tokenType == TK_CASE) {
        tokenizer.Next();
        auto expression = ParseBinary(0);
        auto intCast = createCast(symTable.getTypeSymbol("Int"), expression, "expression must be Int or Char type");
        if (intCast != nullptr) expression = intCast;
        requireToken(TK_OF, "of");
        auto tmp = new StmtCase(parent);
        Token caseToken = tokenizer.Next();
        while (checkToken(caseToken.tokenType, lexList(TK_END, TK_ELSE, 0))) {
            std::vector<StmtCase::Case *> labels;
            do {
                auto label = new StmtCase::Case;
                caseToken = tokenizer.Next();
                if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                    label->left = Token(caseToken);
                } else
                    throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
                caseToken = tokenizer.Next();
                if (caseToken.tokenType == TK_DBLPOINT) {
                    caseToken = tokenizer.Next();
                    if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                        label->right = Token(caseToken);
                    } else
                        throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
                    caseToken = tokenizer.Next();
                }
                labels.push_back(label);
            } while (caseToken.tokenType == TK_COMMA);
            requireToken(TK_COLON, ":");
            tokenizer.Next();
            Statement *labelStmt = ParseStatement(tmp);
            for (auto label : labels) {
                tmp->add(label->left, label->right, labelStmt);
            }
            caseToken = tokenizer.Get();
        }
        if (caseToken.tokenType == TK_ELSE) {
            caseToken = tokenizer.Next();
            auto label = new StmtCase::Case;
            if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                label->left = Token(caseToken);
            } else
                throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
            caseToken = tokenizer.Next();
            if (caseToken.tokenType == TK_DBLPOINT) {
                caseToken = tokenizer.Next();
                if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                    label->right = Token(caseToken);
                } else
                    throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
                caseToken = tokenizer.Next();
            }
            requireToken(TK_COLON, ":");
            tokenizer.Next();
            Statement *labelStmt = ParseStatement(tmp);
            tmp->add(label->left, label->right, labelStmt);
            tokenizer.Next();
        }
        requireToken(TK_END, "end");
        tokenizer.Next();
        result = tmp;
    } else if (tk.tokenType == TK_REPEAT) {
        tokenizer.Next();
    } else if (tk.tokenType == TK_WHILE) {
        tokenizer.Next();
    } else if (tk.tokenType == TK_FOR) {
        tokenizer.Next();
    } else if (tk.tokenType == TK_BREAK) {

    } else if (tk.tokenType == TK_CONTINUE) {

    } else if (tk.tokenType == TK_BEGIN) {
        result = ParseStmtCompound(parent);
    } else {
        Node *expr = ParseAssign();
        thExpectedExpr(expr);
        auto tmp = new StmtExpression(parent);
        tmp->expression = expr;
        result = tmp;
    }
    return result;
}

Statement *Parser::ParseStmtCompound(Statement *parent) {
    auto stmt = new StmtCompound(parent);
    requireToken(TK_BEGIN, "begin");
    Token tk = tokenizer.Next();
    while (tk.tokenType != TK_END) {
        stmt->add(ParseStatement());
        requireToken(TK_SEMICOLON, ";");
        tk = tokenizer.Next();
    }
    tokenizer.Next();
    return stmt;
}

Statement *Parser::Parse() {
    ParseDeclSection();
    auto result = ParseStmtCompound();
    requireToken(TK_POINT, ".");
    return result;
}

void Parser::requireToken(LEX_TYPE tok, string tokStr) {
    Token current = tokenizer.Get();
    if (current.tokenType != tok) {
        string tmp = "token '";
        tmp += current.text + "'";
        if (tokenizer.Eof())
            tmp = "EOF";
        throw SyntaxException(current.strNum, current.strPos,
                              "unexpected " + tmp + " but required '" + tokStr + "'");
    }
}

void Parser::thExpectedExpr(Node *node) {
    if (node == nullptr)
        throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, "expected expression");
}