//
// Created by xtpvx on 01.10.2015.
//

#ifndef PASCAL_COMPILER_TOKENEXCEPTION_H
#define PASCAL_COMPILER_TOKENEXCEPTION_H

#include <string>
using std::string;

class TokenException {
private:
    int strNum;
    int strPos;
    string srcText;
    string errorText;
public:
    TokenException();
    TokenException(int strNum, int strPos, const string &srcText, const string &errorText) : strNum(strNum),
                                                                                             strPos(strPos),
                                                                                             srcText(srcText),
                                                                                             errorText(errorText) { }

    void print();
};


#endif //PASCAL_COMPILER_TOKENEXCEPTION_H
