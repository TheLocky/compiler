//
// Created by xtpvx on 28.09.2015.
//

#include "StateController.h"

States StateController::StatesTable[128][ST_COUNT];

void StateController::Build() {
    for (int i = 0; i < 128; ++i) {
        StatesTable[i][ST_BEGIN] = ST_BADCHAR;
        for (int j = 1; j < 100; ++j) {
            StatesTable[i][j] = ST_BEGIN;
        }
    }
    StatesTable[' '][ST_BEGIN] = ST_SKIP;
    StatesTable['\t'][ST_BEGIN] = ST_SKIP;
    StatesTable['\n'][ST_BEGIN] = ST_SKIP;
    for (int i = 0; i < 100; ++i)
        StatesTable['\r'][i] = ST_SKIP;
    BuildNumbers();
    BuildIdents();
    BuildStrings();
    BuildComments();
    BuildSpecial();
}

void StateController::BuildNumbers() {
    //для всех чисел
    for (int i = '0'; i <= '9'; ++i) {
        StatesTable[i][ST_BEGIN] = ST_NUMBER_ALL;
        StatesTable[i][ST_NUMBER_ALL] = ST_NUMBER_ALL;
        StatesTable[i][ST_NUMBER_DBL_FIRST] = ST_NUMBER_DBL;
        StatesTable[i][ST_NUMBER_DBL] = ST_NUMBER_DBL;
        StatesTable[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
        StatesTable[i][ST_NUMBER_EXP] = ST_NUMBER_EXP;
        StatesTable[i][ST_NUMBER_EXP] = ST_NUMBER_EXP_VAL;
        StatesTable[i][ST_NUMBER_EXP_VAL] = ST_NUMBER_EXP_VAL;
        StatesTable[i][ST_NUMBER_EXP_SIGN] = ST_NUMBER_EXP_VAL;
    }
    //число с плавающей точкой
    StatesTable['.'][ST_NUMBER_ALL] = ST_NUMBER_DBL_FIRST; //1.
    for (int i = 0; i < '0'; ++i)
        StatesTable[i][ST_NUMBER_DBL_FIRST] = ST_NOFRACT;
    for (int i = '9'+1; i < 128; ++i)
        StatesTable[i][ST_NUMBER_DBL_FIRST] = ST_NOFRACT;
    StatesTable['.'][ST_NUMBER_DBL_FIRST] = ST_DBLPOINT; //Исключение разделителя
    //число с экспонентой
    StatesTable['E'][ST_NUMBER_ALL] = ST_NUMBER_EXP;
    StatesTable['e'][ST_NUMBER_ALL] = ST_NUMBER_EXP;
    StatesTable['E'][ST_NUMBER_DBL] = ST_NUMBER_EXP;
    StatesTable['e'][ST_NUMBER_DBL] = ST_NUMBER_EXP;
    for (int i = 0; i < '0'; ++i) {
        StatesTable[i][ST_NUMBER_EXP] = ST_NOEXP;
        StatesTable[i][ST_NUMBER_EXP_SIGN] = ST_NOEXP;
    }
    for (int i = '9'+1; i < 128; ++i) {
        StatesTable[i][ST_NUMBER_EXP] = ST_NOEXP;
        StatesTable[i][ST_NUMBER_EXP_SIGN] = ST_NOEXP;
    }
    StatesTable['+'][ST_NUMBER_EXP] = ST_NUMBER_EXP_SIGN;
    StatesTable['-'][ST_NUMBER_EXP] = ST_NUMBER_EXP_SIGN;
    //HEX-числа
    StatesTable['$'][ST_BEGIN] = ST_NUMBER_HEX;
    for (int i = 'A'; i <= 'F'; ++i) {
        StatesTable[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
    }
    for (int i = 'G'; i <= 'Z'; ++i) {
        StatesTable[i][ST_NUMBER_HEX] = ST_NOHEX;
    }
    for (int i = 'a'; i <= 'f'; ++i) {
        StatesTable[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
    }
    for (int i = 'g'; i <= 'z'; ++i) {
        StatesTable[i][ST_NUMBER_HEX] = ST_NOHEX;
    }
}

void StateController::BuildIdents() {
    for (int i = 'A'; i < 'Z'; ++i) {
        StatesTable[i][ST_BEGIN] = ST_IDENT;
        StatesTable[i][ST_IDENT] = ST_IDENT;
    }
    for (int i = 'a'; i < 'z'; ++i) {
        StatesTable[i][ST_BEGIN] = ST_IDENT;
        StatesTable[i][ST_IDENT] = ST_IDENT;
    }
    for (int i = '0'; i < '9'; ++i) {
        StatesTable[i][ST_IDENT] = ST_IDENT;
    }
}

void StateController::BuildComments() {
    StatesTable['{'][ST_BEGIN] = ST_COMMENT1_OP;
    for (int i = 0; i < 128; ++i) {
        StatesTable[i][ST_COMMENT1_OP] = ST_COMMENT1_OP;
        StatesTable[i][ST_COMMENT2_OP] = ST_COMMENT2_OP;
    }
    StatesTable['}'][ST_COMMENT1_OP] = ST_COMMENT1_CL;
    StatesTable['\n'][ST_COMMENT2_OP] = ST_COMMENT2_CL;
}

void StateController::BuildStrings() {
    StatesTable['\''][ST_BEGIN] = ST_STRING_OP;
    for (int i = 0; i < 128; ++i) {
        StatesTable[i][ST_STRING_OP] = ST_STRING_OP;
        StatesTable[i][ST_STR_SCREEN] = ST_STRING_OP;
    }
    StatesTable['\n'][ST_STRING_OP] = ST_BADNL;
    StatesTable['\''][ST_STRING_OP] = ST_STRING_CL;
    StatesTable['\\'][ST_STRING_OP] = ST_STR_SCREEN;
    StatesTable['#'][ST_BEGIN] = ST_CHAR;
    for (int i = 0; i < '0'; ++i)
        StatesTable[i][ST_CHAR] = ST_NOCC;
    for (int i = '0'; i <= '9'; ++i) {
        StatesTable[i][ST_CHAR] = ST_CHAR_NUM;
        StatesTable[i][ST_CHAR_NUM] = ST_CHAR_NUM;
    }
    for (int i = '9'+1; i < 128; ++i)
        StatesTable[i][ST_CHAR] = ST_NOCC;
    StatesTable['$'][ST_CHAR] = ST_NUMBER_HEX;
}

void StateController::BuildSpecial() {
    StatesTable['+'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['-'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['*'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['/'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['^'][ST_BEGIN] = ST_SPECIAL;
    StatesTable['<'][ST_BEGIN] = ST_SPECIAL_EB;
    StatesTable['>'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['='][ST_BEGIN] = ST_SPECIAL;
    StatesTable['@'][ST_BEGIN] = ST_SPECIAL;
    StatesTable['('][ST_BEGIN] = ST_SPECIAL;
    StatesTable[')'][ST_BEGIN] = ST_SPECIAL;
    StatesTable['['][ST_BEGIN] = ST_SPECIAL;
    StatesTable[']'][ST_BEGIN] = ST_SPECIAL;
    StatesTable[';'][ST_BEGIN] = ST_SPECIAL;
    StatesTable[':'][ST_BEGIN] = ST_SPECIAL_E;
    StatesTable['.'][ST_BEGIN] = ST_SPECIAL_P;
    StatesTable[','][ST_BEGIN] = ST_SPECIAL;
    StatesTable['='][ST_SPECIAL_E] = ST_SPECIAL_DBL;
    StatesTable['='][ST_SPECIAL_EB] = ST_SPECIAL_DBL;
    StatesTable['>'][ST_SPECIAL_EB] = ST_SPECIAL_DBL;
    StatesTable['.'][ST_SPECIAL_P] = ST_SPECIAL_DBL;
    StatesTable['/'][ST_SPECIAL_E] = ST_COMMENT2_OP;
}


void StateController::Print() {
    for (int i = 0; i < 128; ++i) {
        printf("%c ", (char)i);
        for (int j = ST_BEGIN; j < ST_SPECIAL_DBL; ++j) {
            printf("%d ", StatesTable[i][j]);
        }
        printf("\n");
    }
}
