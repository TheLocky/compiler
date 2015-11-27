//
// Created by xtpvx on 01.10.2015.
//

#ifndef PASCAL_COMPILER_TOKENEXCEPTION_H
#define PASCAL_COMPILER_TOKENEXCEPTION_H

#include <string>
#include "tokenizer\Token.h"
using std::string;

class TokenException {
private:
    int strNum;
    int strPos;
    string srcText;
    string errorText;
public:
    TokenException(int str, int pos, const string &src, const string &error);
    void print();
};

class SyntaxException {
private:
    int strNum;
    int strPos;
    string errorText;
public:
    SyntaxException(int str, int pos, const string &error);
	SyntaxException(Token &tok, const string &error);
    void print();
    string getErrorText() const;
};




#endif //PASCAL_COMPILER_TOKENEXCEPTION_H
