//
// Created by xtpvx on 30.09.2015.
//

#ifndef PASCAL_COMPILER_TOKEN_H
#define PASCAL_COMPILER_TOKEN_H

#include <string>
#include "TokenTypes.h"
#include <math.h>

using std::string;

class Token {
private:
    int getTokenType(LEX_TYPE lt);
    string getTokenTypeStr(LEX_TYPE lt);

public:
    int strNum;
    int strPos;
    LEX_TYPE tokenType;
    string text;
    int intData;
    double realData;


    Token(int strNum, int strPos, LEX_TYPE tokenType,
          const string &text, int data1, double data2) :
            strNum(strNum), strPos(strPos), tokenType(tokenType),
            text(text), intData(data1), realData(data2) { }

    Token();

    Token(const Token& src);

    void print();
};


#endif //PASCAL_COMPILER_TOKEN_H
