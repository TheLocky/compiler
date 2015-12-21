//
// Created by xtpvx on 28.09.2015.
//

#include "StatesData.h"

StatesEnum States::Table[CHAR_COUNT][ST_LAST];

void States::Build() {
    for (int i = 0; i < CHAR_COUNT; ++i) {
        Table[i][ST_BEGIN] = ST_BADCHAR;
        for (int j = 1; j < ST_LAST; ++j) {
            Table[i][j] = ST_BEGIN;
        }
    }
    Table[' '][ST_BEGIN] = ST_SKIP;
    Table['\t'][ST_BEGIN] = ST_SKIP;
    Table['\n'][ST_BEGIN] = ST_SKIP;
    for (int i = 0; i < ST_LAST; ++i)
        Table['\r'][i] = ST_SKIP;
    BuildNumbers();
    BuildIdents();
    BuildStrings();
    BuildComments();
    BuildSpecial();
}

void States::BuildNumbers() {
    //All number
    for (int i = '0'; i <= '9'; ++i) {
        Table[i][ST_BEGIN] = ST_NUMBER_ALL;
        Table[i][ST_NUMBER_ALL] = ST_NUMBER_ALL;
        Table[i][ST_NUMBER_DBL_FIRST] = ST_NUMBER_DBL;
        Table[i][ST_NUMBER_DBL] = ST_NUMBER_DBL;
        Table[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
        Table[i][ST_NUMBER_EXP] = ST_NUMBER_EXP;
        Table[i][ST_NUMBER_EXP] = ST_NUMBER_EXP_VAL;
        Table[i][ST_NUMBER_EXP_VAL] = ST_NUMBER_EXP_VAL;
        Table[i][ST_NUMBER_EXP_SIGN] = ST_NUMBER_EXP_VAL;
    }
    //Double number
    Table['.'][ST_NUMBER_ALL] = ST_NUMBER_DBL_FIRST; //1.
    for (int i = 0; i < '0'; ++i)
        Table[i][ST_NUMBER_DBL_FIRST] = ST_NOFRACT;
    for (int i = '9'+1; i < CHAR_COUNT; ++i)
        Table[i][ST_NUMBER_DBL_FIRST] = ST_NOFRACT;
    Table['.'][ST_NUMBER_DBL_FIRST] = ST_DBLPOINT; //separator ..
    //Exp number
    Table['E'][ST_NUMBER_ALL] = ST_NUMBER_EXP;
    Table['e'][ST_NUMBER_ALL] = ST_NUMBER_EXP;
    Table['E'][ST_NUMBER_DBL] = ST_NUMBER_EXP;
    Table['e'][ST_NUMBER_DBL] = ST_NUMBER_EXP;
    for (int i = 0; i < '0'; ++i) {
        Table[i][ST_NUMBER_EXP] = ST_NOEXP;
        Table[i][ST_NUMBER_EXP_SIGN] = ST_NOEXP;
    }
    for (int i = '9'+1; i < CHAR_COUNT; ++i) {
        Table[i][ST_NUMBER_EXP] = ST_NOEXP;
        Table[i][ST_NUMBER_EXP_SIGN] = ST_NOEXP;
    }
    Table['+'][ST_NUMBER_EXP] = ST_NUMBER_EXP_SIGN;
    Table['-'][ST_NUMBER_EXP] = ST_NUMBER_EXP_SIGN;
    //HEX-numbers
    Table['$'][ST_BEGIN] = ST_NUMBER_HEX;
    for (int i = 'A'; i <= 'F'; ++i) {
        Table[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
    }
    for (int i = 'G'; i <= 'Z'; ++i) {
        Table[i][ST_NUMBER_HEX] = ST_NOHEX;
    }
    for (int i = 'a'; i <= 'f'; ++i) {
        Table[i][ST_NUMBER_HEX] = ST_NUMBER_HEX;
    }
    for (int i = 'g'; i <= 'z'; ++i) {
        Table[i][ST_NUMBER_HEX] = ST_NOHEX;
    }
}

void States::BuildIdents() {
    for (int i = 'A'; i <= 'Z'; ++i) {
        Table[i][ST_BEGIN] = ST_IDENT;
        Table[i][ST_IDENT] = ST_IDENT;
    }
    for (int i = 'a'; i <= 'z'; ++i) {
        Table[i][ST_BEGIN] = ST_IDENT;
        Table[i][ST_IDENT] = ST_IDENT;
    }
    for (int i = '0'; i <= '9'; ++i) {
        Table[i][ST_IDENT] = ST_IDENT;
    }
}

void States::BuildComments() {
    Table['{'][ST_BEGIN] = ST_COMMENT1_OP;
    for (int i = 0; i < CHAR_COUNT; ++i) {
        Table[i][ST_COMMENT1_OP] = ST_COMMENT1_OP;
        Table[i][ST_COMMENT2_OP] = ST_COMMENT2_OP;
    }
    Table['}'][ST_COMMENT1_OP] = ST_COMMENT1_CL;
    Table['\n'][ST_COMMENT2_OP] = ST_COMMENT2_CL;
}

void States::BuildStrings() {
    Table['\''][ST_BEGIN] = ST_STRING_OP;
    for (int i = 0; i < CHAR_COUNT; ++i) {
        Table[i][ST_STRING_OP] = ST_STRING_OP;
        Table[i][ST_STR_SCREEN] = ST_STRING_OP;
    }
    Table['\n'][ST_STRING_OP] = ST_BADNL;
    Table['\''][ST_STRING_OP] = ST_STRING_CL;
    Table['\\'][ST_STRING_OP] = ST_STR_SCREEN;
    Table['#'][ST_BEGIN] = ST_CHAR;
    for (int i = 0; i < '0'; ++i)
        Table[i][ST_CHAR] = ST_NOCC;
    for (int i = '0'; i <= '9'; ++i) {
        Table[i][ST_CHAR] = ST_CHAR_NUM;
        Table[i][ST_CHAR_NUM] = ST_CHAR_NUM;
    }
    for (int i = '9'+1; i < CHAR_COUNT; ++i)
        Table[i][ST_CHAR] = ST_NOCC;
    Table['$'][ST_CHAR] = ST_NUMBER_HEX;
}

void States::BuildSpecial() {
    Table['+'][ST_BEGIN] = ST_SPECIAL_E;
    Table['-'][ST_BEGIN] = ST_SPECIAL_E;
    Table['*'][ST_BEGIN] = ST_SPECIAL_E;
    Table['/'][ST_BEGIN] = ST_SPECIAL_E;
    Table['^'][ST_BEGIN] = ST_SPECIAL;
    Table['<'][ST_BEGIN] = ST_SPECIAL_EB;
    Table['>'][ST_BEGIN] = ST_SPECIAL_E;
    Table['='][ST_BEGIN] = ST_SPECIAL;
    Table['@'][ST_BEGIN] = ST_SPECIAL;
    Table['('][ST_BEGIN] = ST_SPECIAL;
    Table[')'][ST_BEGIN] = ST_SPECIAL;
    Table['['][ST_BEGIN] = ST_SPECIAL;
    Table[']'][ST_BEGIN] = ST_SPECIAL;
    Table[';'][ST_BEGIN] = ST_SPECIAL;
    Table[':'][ST_BEGIN] = ST_SPECIAL_E;
    Table['.'][ST_BEGIN] = ST_SPECIAL_P;
    Table[','][ST_BEGIN] = ST_SPECIAL;
    Table['='][ST_SPECIAL_E] = ST_SPECIAL_DBL;
    Table['='][ST_SPECIAL_EB] = ST_SPECIAL_DBL;
    Table['>'][ST_SPECIAL_EB] = ST_SPECIAL_DBL;
    Table['.'][ST_SPECIAL_P] = ST_SPECIAL_DBL;
    Table['/'][ST_SPECIAL_E] = ST_COMMENT2_OP;
}
