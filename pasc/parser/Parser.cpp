//
// Created by xtpvx on 07.10.2015.
//

#include "Parser.h"
#include "Statement.h"

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

NodeAssign *Parser::ParseAssign(NodeExpr *left) {
    requireToken(TK_ASSIGN, ":=");
    Token tk = tokenizer.Next();
    NodeExpr *right = ParseBinary(0);
    thExpectedExpr(right);
	if (left->type->typeId() == TypeProcedure)
		throw SyntaxException(tk, "left operand is not assignable");
    createCast(left->type, &right);
    return new NodeAssign(left, right);
}

NodeExpr *Parser::ParseBinary(int level) {
    if (level >= ARRAY_SIZE(priority))
        return ParseFactor();
    NodeExpr *left = ParseBinary(level + 1);
    Token op = tokenizer.Get();
    while (checkToken(op.tokenType, priority[level])) {
        tokenizer.Next();
        NodeExpr *right = ParseBinary(level + 1);
        thExpectedExpr(right);
        if (checkToken(op.tokenType, lexList(TK_AND, TK_OR, 0))) { //bool
            createCast(GLOBAL_BOOLEAN, &left);
            createCast(GLOBAL_BOOLEAN, &right);
        } else
            createCast(&left, &right);
        left = new ExprBinary(op, left, right);
        if (level == 2) left->type = GLOBAL_BOOLEAN;
        op = tokenizer.Get();
    }
    return left;
}

std::vector<NodeExpr *> Parser::ParseParameters() {
    std::vector<NodeExpr *> result;
    NodeExpr *item = ParseBinary(0);
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

NodeExpr *Parser::ParseFactor() {
    Token factor = tokenizer.Get();
    if (tokenizer.Eof())
        throw SyntaxException(factor.strNum, factor.strPos, "unexpected EOF");

    NodeExpr *result;
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
            result = new ExprIntConst(factor, GLOBAL_INT);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_REAL) {
            result = new ExprRealConst(factor, GLOBAL_DOUBLE);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_STRING) {
            result = new ExprStringConst(factor, GLOBAL_STRING);
            tokenizer.Next();
        }
        else if (factor.tokenType == TK_CHAR) {
            result = new ExprCharConst(factor, GLOBAL_CHAR);
            tokenizer.Next();
        }
        else if (checkToken(factor.tokenType, lexList(TK_TRUE, TK_FALSE, 0))) {
            result = new ExprBooleanConst(factor, GLOBAL_BOOLEAN);
            tokenizer.Next();
        }
        else {
            return nullptr;
        }
    }
    return result;
}

NodeExpr *Parser::ParseDesignator() {
    Token ident = tokenizer.Get();
    Symbol *symbol = currentTable->getSymbol(ident.text);
    if (symbol == nullptr && checkTypeAllow)
        throw SyntaxException(ident, string("undefined symbol '") + ident.text + "'");
    SymType *type = symbol ? ((SymVar*)symbol)->type : nullptr;
    NodeExpr *result = new ExprVariable(ident, type);
    Token tk = tokenizer.Next();
    while (checkToken(tk.tokenType, lexList(TK_LB, TK_LSQB, TK_POINT, TK_POINTER, 0))) {
        if (tk.tokenType == TK_LB) {
            tokenizer.Next();
            auto params = ParseParameters();
			requireToken(TK_RB, ")");
			auto proc = currentTable->getCastedSymbol<SymProcedure *>(ident);
			if (!proc) throw SyntaxException(ident, string("symbol '") + ident.text + "' is not callable");
			if (params.size() != proc->parameters.size())
				throw SyntaxException(ident, string("undefined symbol '") + ident.text + "'");
			for (int i = 0; i < (int)params.size(); i++) {
				auto parVar = proc->localTable->getCastedSymbol<SymVar *>(proc->parameters[i].name);
				createCast(parVar->type, &params[i]);
			}
			auto func = dynamic_cast<SymFunction *>(proc);
            result = new NodeFunc(result, params, func ? func->retType : nullptr);
        } else if (tk.tokenType == TK_LSQB) {
            tokenizer.Next();
            //TODO fix symbol
            if (checkTypeAllow)
                symbol = ((SymVar*)symbol)->type;
            auto params = ParseParameters();
            requireToken(TK_RSQB, "]");
            if (params.size() > 0)
                for (int i = 0; i < (int)params.size(); i++) {
                    auto parameter = params[i];
                    auto tmpType = result->type;
                    if (checkTypeAllow) {
                        if (symbol->typeId() != TypeArray)
                            throw SyntaxException(tk, "operator [] required Array type variable");
                        createCast(((SymArray*)symbol)->indexType, &parameter, "invalid index type");
                        result->type = ((SymArray*)symbol);
                        symbol = tmpType =  ((SymArray*)symbol)->arrayType;
                    }
                    result = new NodeArrayIndex(result, parameter, tmpType);
                }
            else
                throw SyntaxException(tokenizer.Get(), "required array index");
        } else if (tk.tokenType == TK_POINT) {
            tokenizer.Next();
            requireToken(TK_ID, "<identificator>");
			Token field = tokenizer.Get();
			auto recType = currentTable->getCastedSymbol<SymVar *>(ident, TypeVar)->type;
			if (recType->typeId() != TypeRecord)
				throw SyntaxException(field, string("symbol '") + field.text + "is not Record");
			auto record = (SymRecord *)recType;
			auto var = record->localTable->getCastedSymbolNoThrow<SymVar *>(field);
			if (!var) throw SyntaxException(ident, string("record '") + ident.text + 
				"' not contains symbol '" + field.text + "'");
            NodeExpr *right = new ExprVariable(field, var->type);
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
        if (currentTable->containsType(name.text))
            throw SyntaxException(name.strNum, name.strPos, string("type'") + name.text + "'is already defined");
        tokenizer.Next();
        currentTable->addType(ParseType(name.text, true));
        requireToken(TK_SEMICOLON, ";");
        name = tokenizer.Next();
    }
}

SymType *Parser::ParseType(string name, bool createAlias) {
    Token typeName = tokenizer.Get();
    if (typeName.tokenType == TK_ID) {
        tokenizer.Next();
        if (createAlias)
            return new SymAlias(name, currentTable->getType(typeName));
        else
            return currentTable->getType(typeName);
    } else if(typeName.tokenType == TK_ARRAY) {
        return ParseTypeArray(name);
    } else if (checkToken(typeName.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
        return ParseTypeSubRange(name);
    } else if (typeName.tokenType == TK_POINTER) {
        return new SymTypePointer(name, ParseType("anon", false));
    } else if (typeName.tokenType == TK_RECORD) {
        return ParseRecord(name);
    }
    else
        throw SyntaxException(typeName.strNum, typeName.strPos, "expected type declaration");
}

SymArray *Parser::ParseTypeArray(string name) {
    requireToken(TK_ARRAY, "Array");
    tokenizer.Next();
    bool dynamic = true;
    SymType *indexType = GLOBAL_INT;
    if (tokenizer.Get().tokenType == TK_LSQB) {
        tokenizer.Next();
        indexType = ParseType("anon", false);
        if (indexType->peelAlias()->typeId() != TypeInt && indexType->peelAlias()->typeId() != TypeChar)
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
    if (left->type->typeId() != right->type->typeId())
        throw SyntaxException(tokenizer.Get(), "const expression in range must be equal type");
    if (left->value.intData >= right->value.intData)
        throw SyntaxException(tokenizer.Get(), "right value must be greater then left value");
    return new SymTypeSubRange(name, left, right);
}

SymConst *Parser::ParseConstantExpr(string name, std::list<LEX_TYPE> allowTypes) {
    Token an = tokenizer.Get();
    tokenizer.Next();
    if (allowTypes.size() > 0 && !checkToken(an.tokenType, allowTypes))
        throw SyntaxException(an, "unexpected const type");
    switch (an.tokenType) {
        case TK_INT:
            return new SymConst(name, GLOBAL_INT, an);
        case TK_REAL:
            return new SymConst(name, GLOBAL_DOUBLE, an);
        case TK_CHAR:
            return new SymConst(name, GLOBAL_CHAR, an);
        case TK_STRING:
            return new SymConst(name, GLOBAL_STRING, an);
        case TK_TRUE:
        case TK_FALSE:
            return new SymConst(name, GLOBAL_BOOLEAN, an);
        default:
            throw SyntaxException(an, "required const expression");
    }
}

int Parser::createCast(NodeExpr **left, NodeExpr **right) {
    return createCast(left, right, "");
}

int Parser::createCast(NodeExpr **left, NodeExpr **right, string err) {
    int code = createCast((*left)->type, right, err);
    return code == 0 ? createCast((*right)->type, left, err) : code;
}

int Parser::createCast(SymType *type, NodeExpr **node) {
    return createCast(type, node, "");
}

int Parser::createCast(SymType *type, NodeExpr **node, string err) {
    //TODO CLEAR nullptr check
    if (!checkTypeAllow || type == nullptr || (*node)->type == nullptr)
        return -1;

    if (err.empty()) err = string("can't cast ") + (*node)->type->name + " to " + type->name;

    type = type->peelAlias();
    SymType *nodeType = (*node)->type->peelAlias();

    if (type->typeId() <= TypeBool && nodeType->typeId() <= TypeBool) {
        if (nodeType->typeId() == type->typeId())
            return 0;
        else if (simpleCastRules[nodeType->typeId()][type->typeId()] != TypeEnd) {
            auto tmp = new NodeCast(*node, type);
            *node = tmp;
            return 1;
        }
    } else if (type == nodeType) {
        return 0;
    }
    throw SyntaxException(tokenizer.Get().strNum, tokenizer.Get().strPos, err);
}

void Parser::ParseConstBlock() {
    Token name = tokenizer.Get();
    while (name.tokenType == TK_ID) {
        tokenizer.Next();
        requireToken(TK_CE, "=");
        tokenizer.Next();
        currentTable->addSymbol(ParseConstantExpr(name.text, {}));
        requireToken(TK_SEMICOLON, ";");
        name = tokenizer.Next();
    }
}

void Parser::ParseVarBlock() {
    do {
		auto namesList = ParseIdentificatorsList();
        requireToken(TK_COLON, ":");
        tokenizer.Next();
        SymType *type = ParseType("anon", false);
        for (auto name : namesList)
            currentTable->addSymbol(new SymVar(name.text, type));
        requireToken(TK_SEMICOLON, ";");
        tokenizer.Next();
	} while (tokenizer.Get().tokenType == TK_ID);
}

std::vector<Token> Parser::ParseIdentificatorsList() {
    std::vector<Token> result;
    Token name = tokenizer.Get();
	requireToken(TK_ID, "<identificator>");
    while (name.tokenType == TK_ID) {
        result.push_back(name);
        if (tokenizer.Next().tokenType != TK_COMMA)
            return result;
        name = tokenizer.Next();
    }
    return result;
}

void Parser::ParseCallDeclaration() {
	bool isFuntion = tokenizer.Get().tokenType == TK_FUNCTION;
	Token callName = tokenizer.Next();
	requireToken(TK_ID, "<identificator>");
	tokenizer.Next();
	SymTable *newTable = new SymTable(currentTable);
	currentTable = newTable;
	auto call = isFuntion ? new SymFunction(callName.text, newTable) : new SymProcedure(callName.text, newTable);
	if (tokenizer.Get().tokenType == TK_LB) {
		tokenizer.Next();
		call->parameters = ParseCallDeclParameters();
		requireToken(TK_RB, ")");
		tokenizer.Next();
	}
	if (isFuntion) {
		requireToken(TK_COLON, ":");
		Token typeName = tokenizer.Next();
		requireToken(TK_ID, "<identificator>");
		((SymFunction *)call)->retType = currentTable->getType(typeName);
		newTable->addSymbol(new SymVar("Result", ((SymFunction *)call)->retType));
		tokenizer.Next();
	}
	requireToken(TK_SEMICOLON, ";");
	tokenizer.Next();
	currentTable->parent->addSymbol(call);
	ParseDeclarationSection();
	call->def = ParseStmtCompound(nullptr);
	currentTable = currentTable->parent;
	requireToken(TK_SEMICOLON, ";");
	tokenizer.Next();
}

std::vector<SymProcedure::Parameter> Parser::ParseCallDeclParameters() {
	std::vector<SymProcedure::Parameter> params;
	if (tokenizer.Get().tokenType != TK_RB) {
		do {
			auto mode = SymProcedure::Modificator::MODE_DEFAULT;
			if (tokenizer.Get().tokenType == TK_VAR) {
				mode = SymProcedure::Modificator::MODE_VAR;
				tokenizer.Next();
			}
			auto namesList = ParseIdentificatorsList();
			if (namesList.size() < 1)
				requireToken(TK_ID, "<identificator>");
			requireToken(TK_COLON, ":");
			Token typeTok = tokenizer.Next();
			SymType *type;
			if (typeTok.tokenType == TK_ARRAY) {
				tokenizer.Next();
				requireToken(TK_OF, "of");
				typeTok = tokenizer.Next();
				requireToken(TK_ID, "<identificator>");
				SymType *arrType = currentTable->getType(typeTok);
				type = new SymArray("anon", arrType, GLOBAL_INT, true);
			}
			else if (typeTok.tokenType == TK_ID) {
				type = currentTable->getType(typeTok);
			}
			else requireToken(TK_ID, "<identificator>");
			for (auto name : namesList) {
				currentTable->addSymbol(new SymVar(name.text, type));
				SymProcedure::Parameter param(name.text, mode);
				params.push_back(param);
			}
			tokenizer.Next();
		} while (tokenizer.CheckAndNext(TK_SEMICOLON));
	}
	return params;
}

SymRecord * Parser::ParseRecord(string name)
{
	auto newTable = new SymTable(currentTable);
	auto rec = new SymRecord(name, newTable);
	Token tok = tokenizer.Next();
	do {
		if (tokenizer.Get().tokenType == TK_END) {
			tokenizer.Next();
			return rec;
		}
		requireToken(TK_ID, "<identificator>");
		auto varList = ParseIdentificatorsList();
		requireToken(TK_COLON, ":");
		tokenizer.Next();
		SymType *type = ParseType("anon", false);
		for (auto var : varList)
			newTable->addSymbol(new SymVar(var.text, type));
		if (tokenizer.Get().tokenType != TK_END) {
			requireToken(TK_SEMICOLON, ";");
			tokenizer.Next();
		}
	} while (true);
}

void Parser::ParseDeclarationSection() {
    Token declarationSection = tokenizer.Get();
    if (declarationSection.text == "skipTypeCheck") {
        checkTypeAllow = false;
        declarationSection = tokenizer.Next();
    }
    while (true) {
		switch (declarationSection.tokenType)
		{
		case TK_TYPE:
			tokenizer.Next();
			ParseTypeBlock();
			break;
		case TK_CONST:
			tokenizer.Next();
			ParseConstBlock();
			break;
		case TK_VAR:
			tokenizer.Next();
			ParseVarBlock();
			break;
		case TK_PROCEDURE:
		case TK_FUNCTION:
			ParseCallDeclaration();
			break;
		default:
			return;
		}
		declarationSection = tokenizer.Get();
    }
}

Statement *Parser::ParseStatement(Statement *parent) {
    Statement *result;
    Token tk = tokenizer.Get();
    if (tk.tokenType == TK_IF) {
        tokenizer.Next();
        result = ParseStmtIf(parent);
    }
    else if (tk.tokenType == TK_CASE) {
        tokenizer.Next();
        result = ParseStmtCase(parent);
    }
    else if (tk.tokenType == TK_REPEAT) {
        tokenizer.Next();
        result = ParseStmtRepeat(parent);
    }
    else if (tk.tokenType == TK_WHILE) {
        tokenizer.Next();
        result = ParseStmtWhile(parent);
    }
    else if (tk.tokenType == TK_FOR) {
        tokenizer.Next();
        result = ParseStmtFor(parent);
    }
    else if (tk.tokenType == TK_BREAK) {
        Statement *loop = parent;
        while (loop != nullptr) {
            if (dynamic_cast<StmtLoop*>(loop))
                break;
            loop = loop->parent;
        }
        if (!loop)
            throw SyntaxException(tk.strPos, tk.strNum, "operator 'break' must be inside the loop");
        tokenizer.Next();
        if (tokenizer.Get().tokenType != TK_END) {
            requireToken(TK_SEMICOLON, ";");
            tokenizer.Next();
        }
        result = new StmtBreak(loop);
    }
    else if (tk.tokenType == TK_CONTINUE) {
        Statement *loop = parent;
        while (loop != nullptr) {
            if (dynamic_cast<StmtLoop*>(loop))
                break;
            loop = loop->parent;
        }
        if (!loop)
            throw SyntaxException(tk.strPos, tk.strNum, "operator 'continue' must be inside the loop");
        tokenizer.Next();
        if (tokenizer.Get().tokenType != TK_END) {
            requireToken(TK_SEMICOLON, ";");
            tokenizer.Next();
        }
        result = new StmtContinue(loop);
    }
    else if (tk.tokenType == TK_BEGIN) {
        result = ParseStmtCompound(parent);
    }
    else {
        Node *node;
        NodeExpr *expr = ParseBinary(0);
        thExpectedExpr(expr);
        if (tokenizer.Get().tokenType == TK_ASSIGN)
            node = ParseAssign(expr);
        else
            node = expr;
        auto tmp = new StmtExpression(parent);
        tmp->expression = node;
        if (tokenizer.Get().tokenType != TK_END) {
            requireToken(TK_SEMICOLON, ";");
            tokenizer.Next();
        }
        result = tmp;
    }
    if (tokenizer.Get().tokenType == TK_SEMICOLON) {
        tokenizer.Next();
    }
    return result;
}

Statement *Parser::ParseStmtCompound(Statement *parent) {
    auto stmt = new StmtCompound(parent);
    requireToken(TK_BEGIN, "begin");
    Token tk = tokenizer.Next();
    while (tk.tokenType != TK_END) {
        stmt->add(ParseStatement());
        tk = tokenizer.Get();
    }
    tokenizer.Next();
    return stmt;
}

Statement *Parser::ParseStmtIf(Statement *parent) {
    auto condition = ParseBinary(0);
    thExpectedExpr(condition);
    createCast(GLOBAL_BOOLEAN, &condition, "condition must be Boolean type");
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
    }
    return tmp;
}

Statement *Parser::ParseStmtCase(Statement *parent) {
    auto expression = ParseBinary(0);
    createCast(GLOBAL_INT, &expression, "expression must be Int or Char type");
    requireToken(TK_OF, "of");
    auto tmp = new StmtCase(parent);
    tmp->condition = (ExprBinary*)expression;
    Token caseToken = tokenizer.Next();
    while (!checkToken(caseToken.tokenType, lexList(TK_END, TK_ELSE, 0))) {
        std::vector<StmtCase::Case *> labels;
        do {
            auto label = new StmtCase::Case;
            caseToken = tokenizer.Get();
            if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                label->left = Token(caseToken);
            } else
                throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
            if (caseToken.tokenType == TK_DBLPOINT) {
                caseToken = tokenizer.Next();
                if (checkToken(caseToken.tokenType, lexList(TK_INT, TK_CHAR, 0))) {
                    label->right = Token(caseToken);
                } else
                    throw SyntaxException(caseToken.strNum, caseToken.strPos, "case label must be Int or Char type");
            }
            labels.push_back(label);
            if (tokenizer.Next().tokenType != TK_COMMA)
                break;
            else
                tokenizer.Next();
        } while (true);
        requireToken(TK_COLON, ":");
        tokenizer.Next();
        Statement *labelStmt = ParseStatement(tmp);
        for (auto label : labels) {
            tmp->add(label->left, label->right, labelStmt);
        }
        caseToken = tokenizer.Get();
    }
    if (caseToken.tokenType == TK_ELSE) {
        tokenizer.Next();
        tmp->otherwise = ParseStatement(tmp);
    }
    requireToken(TK_END, "end");
    tokenizer.Next();
    return tmp;
}

Statement *Parser::ParseStmtFor(Statement *parent) {
    Token ident = tokenizer.Get();
    requireToken(TK_ID, "<identificator>");
    SymVar *var = currentTable->getCastedSymbol<SymVar *>(ident.text);
    if (checkTypeAllow) {
        if (!var)
            throw SyntaxException(ident, "undefined variable");
        if (var->type->peelAlias()->typeId() != TypeInt)
            throw SyntaxException(ident, "variable must be Integer type");
    }
    auto tmp = new StmtFor(parent);
    tmp->variable = var;
    tokenizer.Next();
    requireToken(TK_ASSIGN, ":=");
    tokenizer.Next();
    auto initExpr = ParseBinary(0);
    createCast(GLOBAL_INT, &initExpr);
    Token direction = tokenizer.Get();
    if (direction.tokenType == TK_TO)
        tmp->downto = false;
    else if (direction.tokenType == TK_DOWNTO)
        tmp->downto = true;
    else
        requireToken(TK_TO, "to' or 'downto");
    tokenizer.Next();
    auto resExpr = ParseBinary(0);
    createCast(GLOBAL_INT, &resExpr);
    requireToken(TK_DO, "do");
    tokenizer.Next();
    tmp->statement = ParseStatement(tmp);
    tmp->initExpr = initExpr;
    tmp->resExpr = resExpr;
    return tmp;
}

Statement *Parser::ParseStmtWhile(Statement *parent) {
    auto tmp = new StmtWhile(parent);
    auto cond = ParseBinary(0);
    createCast(GLOBAL_BOOLEAN, &cond, "expression must be Boolean type");
    tmp->condition = (ExprBinary*)cond;
    requireToken(TK_DO, "do");
    tokenizer.Next();
    tmp->statement = ParseStatement(tmp);
    return tmp;
}

Statement *Parser::ParseStmtRepeat(Statement *parent) {
    auto tmp = new StmtRepeat(parent);
    auto stmtList = new StmtCompound(tmp);
    Token tk = tokenizer.Get();
    while (tk.tokenType != TK_UNTIL) {
        stmtList->add(ParseStatement());
        tk = tokenizer.Get();
    }
    tmp->statement = stmtList;
    requireToken(TK_UNTIL, "until");
    tokenizer.Next();
    auto cond = ParseBinary(0);
    createCast(GLOBAL_BOOLEAN, &cond, "expression must be Boolean type");
    tmp->condition = (ExprBinary*)cond;
    requireToken(TK_SEMICOLON, ";");
    tokenizer.Next();
    return tmp;
}

SymProcedure *Parser::Parse() {
    currentTable = new SymTable();
    currentTable->addSystemTypes();
    auto mainProc = new SymProcedure("main", currentTable);
    ParseDeclarationSection();
    mainProc->def = ParseStmtCompound();
    requireToken(TK_POINT, ".");
    return mainProc;
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
