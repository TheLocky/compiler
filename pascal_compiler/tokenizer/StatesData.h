//
// Created by Locky on 28.09.2015.
//

#ifndef PASCAL_COMPILER_STATECONTROLLER_H
#define PASCAL_COMPILER_STATECONTROLLER_H

#define CHAR_COUNT (128)

enum __States {
    ST_SKIP = -1,
    ST_BEGIN = 0,
    ST_IDENT,
    ST_NUMBER_ALL,
    ST_NUMBER_DBL_FIRST, ST_NUMBER_DBL,
    ST_NUMBER_EXP, ST_NUMBER_EXP_SIGN, ST_NUMBER_EXP_VAL,
    ST_NUMBER_HEX,
    ST_STRING_OP, ST_STRING_CL, ST_STR_SCREEN,
    ST_CHAR, ST_CHAR_NUM,
    ST_COMMENT1_OP, ST_COMMENT1_CL,
    ST_COMMENT2_OP, ST_COMMENT2_CL,
    ST_SPECIAL, //Введен специальный символ (односимвольные лексемы)
    ST_SPECIAL_E, //= после спецсимвола
    ST_SPECIAL_EB, //> либо = после спецсимвола
    ST_SPECIAL_P, //. после спецсимвола
    ST_SPECIAL_DBL, //Двусимвольные лексемы
    ST_DBLPOINT,
    ST_BADCHAR,
    ST_NOFRACT,
    ST_NOEXP,
    ST_NOHEX,
    ST_BADNL,
    ST_NOCC,
    ST_LAST
};

class States {
public:
    static __States Table[CHAR_COUNT][ST_LAST];

    static void Build();
    static void BuildNumbers();
    static void BuildIdents();
    static void BuildComments();
    static void BuildStrings();
    static void BuildSpecial();
};

#endif //PASCAL_COMPILER_STATECONTROLLER_H
