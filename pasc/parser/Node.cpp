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

NodeWrite::NodeWrite(std::vector<NodeExpr*> params) {
	operatorToken.tokenType = TK_WRITE;
	this->params = params;
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

void ExprBinary::generate(AsmCode * code) {
	left->generate(code);
	right->generate(code);
	code->add(pop, ebx);
	code->add(pop, eax);
	switch (operatorToken.tokenType) {
	case TK_ADD:
		code->add(add, eax, ebx);
		break;
	case TK_SUB:
		code->add(sub, eax, ebx);
		break;
	case TK_MUL:
		code->add(mul, ebx);
		break;
	case TK_DIV:
		code->add(_div, ebx);
		break;
	default:
		break;
	}
	code->add(push, eax);
}

void ExprUnary::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.text.c_str());
    if (right != NULL)
        right->print(prefix + "\t");
}

void ExprUnary::generate(AsmCode * code) {
	right->generate(code);
	switch (operatorToken.tokenType) {
	case TK_SUB:
		code->add(pop, eax);
		code->add(neg, eax);
		code->add(push, eax);
		break;
	default:
		break;
	}
}

void ExprIntConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprIntConst::generate(AsmCode * code) {
	code->add(push, operatorToken.intData);
}

void ExprRealConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprRealConst::generate(AsmCode * code) {
}

void ExprStringConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprStringConst::generate(AsmCode * code) {
}

void ExprCharConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprCharConst::generate(AsmCode * code) {
	code->add(push, operatorToken.intData);
}

void ExprBooleanConst::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.getStr().c_str());
}

void ExprBooleanConst::generate(AsmCode * code) {
}

void ExprVariable::print(string prefix) {
    printf("%s%s\n", prefix.c_str(), operatorToken.text.c_str());
}

void ExprVariable::generate(AsmCode * code) {
}

void NodeArrayIndex::print(string prefix) {
    if (parent != NULL)
        parent->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), "[ ]");
    if (child != NULL)
        child->print(prefix + "\t");
}

void NodeArrayIndex::generate(AsmCode * code) {
}

void NodeRecordAccess::print(string prefix) {
    if (left != NULL)
        left->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), ".");
    if (right != NULL)
        right->print(prefix + "\t");
}

void NodeRecordAccess::generate(AsmCode * code) {
}

void NodeFunc::print(string prefix) {
    if (parent != NULL)
        parent->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), "( )");
    for (auto i = params.begin(); i != params.end(); i++)
        (*i)->print(prefix + "\t");
}

void NodeFunc::generate(AsmCode * code) {
}

void NodeWrite::print(string prefix) {
	printf("%sWRITE ( )\n", prefix.c_str());
	for (auto i = params.begin(); i != params.end(); i++)
		(*i)->print(prefix + "\t");
}

void NodeWrite::generate(AsmCode * code) {
	string format = "";
	int size = 0;
	for (int i = params.size() - 1; i >= 0; i--) {
		switch (params[i]->type->typeId()) {
		case Symbols::TypeInt:
			format = "%d" + format;
			params[i]->generate(code);
			size += 4;
			break;
		case Symbols::TypeChar:
			format = "%c" + format;
			params[i]->generate(code);
			size += 4;
		default:
			break;
		}
	}
	auto fmt = code->add("fmt" +  std::to_string(code->counter()), db, format, 0);
	code->add(push, fmt);
	code->add(call, new AsmIdentificator("crt_printf"));
	code->add(add, esp, size + 4);
}

void NodeCast::print(string prefix) {
    if (parameter != NULL) {
        printf((prefix + "(" + type->name.c_str() + ")\n").c_str());
        parameter->print(prefix + "\t");
    }
}

void NodeCast::generate(AsmCode * code) {
}

void NodeAssign::print(string prefix) {
    if (left != NULL)
        left->print(prefix + "\t");
    printf("%s%s\n", prefix.c_str(), ":=");
    if (right != NULL)
        right->print(prefix + "\t");
}

void NodeAssign::generate(AsmCode * code) {
}
