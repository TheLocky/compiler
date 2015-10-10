//
// Created by xtpvx on 01.10.2015.
//

#include "Exception.h"

void TokenException::print() {
    printf("%d\t%d\t%s\n", strNum, strPos, errorText.c_str());
}

TokenException::TokenException(int str, int pos, const string &src, const string &error) {
    strNum = str;
    strPos = pos;
    srcText = src;
    errorText = error;
}

SyntaxException::SyntaxException(int str, int pos, const string &error) {
    strNum = str;
    strPos = pos;
    errorText = error;
}

void SyntaxException::print() {
    printf("(%d:%d) syntax error: %s", strNum, strPos, errorText.c_str());
}
