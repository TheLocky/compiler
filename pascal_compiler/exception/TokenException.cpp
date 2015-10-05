//
// Created by xtpvx on 01.10.2015.
//

#include "TokenException.h"

TokenException::TokenException() {
    strNum = 0;
    strPos = 0;
    srcText = "";
    errorText = "";
}

void TokenException::print() {
    printf("%d\t%d\t%s\n", strNum, strPos, errorText.c_str());
}
