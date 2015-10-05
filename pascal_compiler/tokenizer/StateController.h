//
// Created by Locky on 28.09.2015.
//

#ifndef PASCAL_COMPILER_STATECONTROLLER_H
#define PASCAL_COMPILER_STATECONTROLLER_H

#include <cstdio>
#define ST_COUNT 100

enum States {
    ST_SKIP = -1, // space & \t
    ST_BEGIN = 0, //Начальное состояние
    ST_IDENT, //Идентификатор
    ST_NUMBER_ALL, //Любое число
    ST_NUMBER_DBL_FIRST, ST_NUMBER_DBL,  //Ввод числа с плавающей точкой
    ST_NUMBER_EXP, ST_NUMBER_EXP_SIGN, ST_NUMBER_EXP_VAL, //Ввод экспоненты
    ST_NUMBER_HEX, //HEX-число
    ST_STRING_OP, ST_STRING_CL, ST_STR_SCREEN, //Ввод строки
    ST_CHAR, ST_CHAR_NUM, //Символьная литера #123
    ST_COMMENT1_OP, ST_COMMENT1_CL, //Комментарий { }
    ST_COMMENT2_OP, ST_COMMENT2_CL, //Комментарий //
    ST_SPECIAL, //Введен специальный символ (односимвольные лексемы)
    ST_SPECIAL_E, //= после спецсимвола
    ST_SPECIAL_EB, //> либо = после спецсимвола
    ST_SPECIAL_P, //. после спецсимвола
    ST_SPECIAL_DBL, //Двусимвольные лексемы
    ST_DBLPOINT, //Двойная точка
    ST_BADCHAR = 50, //Неизвестный символ
    ST_NOFRACT, //Не число после точки в double
    ST_NOEXP, //Не число поле экспоненты
    ST_NOHEX, //Не шестнадцатеричные символы в числе
    ST_BADNL, //Символ перевода строки в строчной лексеме
    ST_NOCC //Не число после #
};

class StateController {
public:
    static States StatesTable[128][ST_COUNT];
    static void Build();
    static void BuildNumbers();
    static void BuildIdents();
    static void BuildComments();
    static void BuildStrings();
    static void BuildSpecial();
    static void Print();
};


#endif //PASCAL_COMPILER_STATECONTROLLER_H
