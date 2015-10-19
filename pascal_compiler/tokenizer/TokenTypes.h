//
// Created by xtpvx on 28.09.2015.
//

#ifndef PASCAL_COMPILER_LEXEMS_H
#define PASCAL_COMPILER_LEXEMS_H

#include <list>
#include <string>
#include <cstdarg>

using std::string;

enum LEX_TYPE {
    TK_ERROR = -1,
    TK_ID = 1, TK_INT, TK_REAL, TK_HEX, TK_CHAR, TK_STRING,
    TK_PROGRAM, TK_BEGIN, TK_END, TK_FWD, TK_FUNCTION, TK_PROCEDURE, TK_RECORD, TK_EXIT,
    TK_IF, TK_THEN, TK_ELSE, TK_FOR, TK_TO, TK_DOWNTO, TK_WHILE, TK_DO, TK_REPEAT, TK_UNTIL, TK_BREAK, TK_CONTINUE,
    TK_VAR, TK_TYPE, TK_ARRAY, TK_OF,
    TK_AND, TK_NOT, TK_OR, TK_XOR, TK_CGT, TK_CLT, TK_CGE, TK_CLE, TK_CE, TK_CNE,
    TK_DIV, TK_MOD, TK_ADD, TK_SUB, TK_MUL, TK_DIV_S, TK_ADD_E, TK_SUB_E, TK_MUL_E, TK_DIV_E, TK_ASSIGN,
    TK_POINTER, TK_ADDR,
    TK_RB, TK_LB, TK_RSQB, TK_LSQB, TK_SEMICOLON, TK_COLON, TK_DBLPOINT, TK_POINT, TK_COMMA
};

struct keyWord {
    string text;
    LEX_TYPE type;
};

struct special {
    string s;
    LEX_TYPE type;
};

struct specialDbl {
    string sD;
    LEX_TYPE type;
};

static keyWord keyWords[] {
        {"begin", TK_BEGIN}, {"forward", TK_FWD}, {"do", TK_DO}, {"else", TK_ELSE}, {"end", TK_END}, {"for", TK_FOR},
        {"function", TK_FUNCTION}, {"if", TK_IF}, {"array", TK_ARRAY}, {"of", TK_OF}, {"procedure", TK_PROCEDURE},
        {"program", TK_PROGRAM}, {"record", TK_RECORD}, {"then", TK_THEN}, {"to", TK_TO}, {"type", TK_TYPE},
        {"var", TK_VAR}, {"while", TK_WHILE}, {"break", TK_BREAK}, {"continue", TK_CONTINUE}, {"downto", TK_DOWNTO},
        {"exit", TK_EXIT}, {"repeat", TK_REPEAT}, {"until", TK_UNTIL}, {"and", TK_AND}, {"div", TK_DIV},
        {"mod", TK_MOD}, {"not", TK_NOT}, {"or", TK_OR}, {"xor", TK_XOR}
};

static specialDbl specialDbls [] {
        {"+=", TK_ADD_E}, {"-=", TK_SUB_E}, {"*=", TK_MUL_E}, {"/=", TK_DIV_E}, {">=", TK_CGE},
        {"<=", TK_CLE}, {"<>", TK_CNE}, {":=", TK_ASSIGN}, {"..", TK_DBLPOINT}
};

static special specials [] {
        {"+", TK_ADD}, {"-", TK_SUB}, {"*", TK_MUL}, {"/", TK_DIV_S}, {"^", TK_POINTER},
        {">", TK_CGT}, {"<", TK_CLT}, {"@", TK_ADDR}, {".", TK_POINT}, {"(", TK_LB}, {")", TK_RB},
        {"[", TK_LSQB}, {"]", TK_RSQB}, {";", TK_SEMICOLON}, {":", TK_COLON}, {",", TK_COMMA}, {"=", TK_CE}
};

static bool checkToken(LEX_TYPE token, std::list<LEX_TYPE> list) {
    for (std::list<LEX_TYPE>::iterator i = list.begin(); i != list.end(); i++) {
        if (*i == token)
            return true;
    }
    return false;
}

static std::list<LEX_TYPE> lexList(LEX_TYPE first, ...) {
    va_list args;
    va_start(args, first);
    LEX_TYPE tmp;
    std::list<LEX_TYPE> result;
    result.push_back(first);
    while ((tmp = (LEX_TYPE)va_arg(args, int)) != 0){
        result.push_back(tmp);
    }
    va_end(args);
    return result;
}

#endif //PASCAL_COMPILER_LEXEMS_H
