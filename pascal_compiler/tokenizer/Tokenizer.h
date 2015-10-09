//
// Created by xtpvx on 30.09.2015.
//

#ifndef PASCAL_COMPILER_TOEKNIZER_H
#define PASCAL_COMPILER_TOEKNIZER_H

#include <fstream>
#include <algorithm>
#include "Token.h"
#include "StatesData.h"
#include "exception/TokenException.h"

using std::ifstream;

class Tokenizer {
private:
    ifstream *file = NULL;
    int currentStr;
    int currentPos;
    Token currentTok;
    bool eof;
public:
    Tokenizer(ifstream *file);
    Token Get();
    Token Next();
    bool Eof();
};


#endif //PASCAL_COMPILER_TOEKNIZER_H
