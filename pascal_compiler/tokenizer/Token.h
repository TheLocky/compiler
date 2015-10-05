//
// Created by xtpvx on 30.09.2015.
//

#ifndef PASCAL_COMPILER_TOKEN_H
#define PASCAL_COMPILER_TOKEN_H

#include <string>
#include "lexems.h"
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
    int data1;
    int data2;
    int data3;


    Token(int strNum, int strPos, LEX_TYPE tokenType,
          const string &text, int data1, int data2, int data3) :
            strNum(strNum), strPos(strPos), tokenType(tokenType),
            text(text), data1(data1), data2(data2), data3(data3) { }

    Token();

    Token(const Token& src);

    void print();
};


#endif //PASCAL_COMPILER_TOKEN_H
