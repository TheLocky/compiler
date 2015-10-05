//
// Created by xtpvx on 28.09.2015.
//

#ifndef PASCAL_COMPILER_LEXEMS_H
#define PASCAL_COMPILER_LEXEMS_H

#include <string>

using std::string;
/*
Хцифра  Ч [0-9]
Хбуква  Ч [A-Za-z_]
Хсимвол  Ч [^'\n]|''
 - символ из двух кавычек обозначает одну кавычку
Хмногострочный_комментарий  Ч {[^}]*}
Хмногострочный_комментарий  Ч \(\*[^(\*\))]*\*\)
Хстрочный_комментарий  Ч //[^EOF\n]*[EOF\n]
Хпробельные символы  Ч [ \t\n\r]+
Хидентификатор  Ч {буква}({буква}|{цифра})*
Хчисло  Ч {цифра}+
Хшестнадцатеричное_число  Ч $[0-9A-Fa-f]+
Хэкспонента  Ч [Ee][+-]?{число}
Хвещественное_число  Ч {число}\.{число}
Хвещественное_число  Ч {число}(\.{число})?{экспонента}
Хсимвольна€_литера  Ч '{символ}'
Хсимвольна€_литера  Ч #[{число}{шестнадцатеричное_число}]
Хстрока  Ч '{символ}*'

Х begin, forward, do, else, end, for, function, if, array, of, procedure, program, record, then, to, type, var, while, break, continue, downto, exit, repeat, until
Х and, div, mod, not, or, xor, +, -, *, /, ^, +=, -=, *=, /=, <, >, <=, >=, =, <>, :=, @, .
Х  (, ), [, ], ;, :, .., (символ зап€та€ ',')

 */

enum LEX_TYPE {
        //global
/*1*/   TK_ID = 1, TK_INT, TK_REAL, TK_HEX, TK_CHAR, TK_STRING,
        //reserved words
/*7*/   TK_PROGRAM, TK_BEGIN, TK_END, TK_FWD, TK_FUNCTION, TK_PROCEDURE, TK_RECORD, TK_EXIT,
/*15*/  TK_IF, TK_THEN, TK_ELSE, TK_FOR, TK_TO, TK_DOWNTO, TK_WHILE, TK_DO, TK_REPEAT, TK_UNTIL, TK_BREAK, TK_CONTINUE,
/*27*/  TK_VAR, TK_TYPE, TK_ARRAY, TK_OF,
        //operations               compare greater/less_than
/*31*/  TK_AND, TK_NOT, TK_OR, TK_XOR, TK_CGT, TK_CLT, TK_CGE, TK_CLE, TK_CE, TK_CNE,
/*41*/  TK_DIV, TK_MOD, TK_ADD, TK_SUB, TK_MUL, TK_DIV_S, TK_ADDE, TK_SUBE, TK_MULE, TK_DIVE, TK_ASGN,
/*52*/  TK_POINTER, TK_ADDR,
        //symbols
/*54*/  TK_RB, TK_LB, TK_RSQB, TK_LSQB, TK_SEMICOLON, TK_COLON, TK_DBLPOINT, TK_POINT, TK_COMMA
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

static keyWord keyWords[30] {
        {"begin", TK_BEGIN}, {"forward", TK_FWD}, {"do", TK_DO}, {"else", TK_ELSE}, {"end", TK_END}, {"for", TK_FOR},
        {"function", TK_FUNCTION}, {"if", TK_IF}, {"array", TK_ARRAY}, {"of", TK_OF}, {"procedure", TK_PROCEDURE},
        {"program", TK_PROGRAM}, {"record", TK_RECORD}, {"then", TK_THEN}, {"to", TK_TO}, {"type", TK_TYPE},
        {"var", TK_VAR}, {"while", TK_WHILE}, {"break", TK_BREAK}, {"continue", TK_CONTINUE}, {"downto", TK_DOWNTO},
        {"exit", TK_EXIT}, {"repeat", TK_REPEAT}, {"until", TK_UNTIL}, {"and", TK_AND}, {"div", TK_DIV},
        {"mod", TK_MOD}, {"not", TK_NOT}, {"or", TK_OR}, {"xor", TK_XOR}
};

static specialDbl specialDbls [9] {
        {"+=", TK_ADDE}, {"-=", TK_SUBE}, {"*=", TK_MULE}, {"/=", TK_DIVE}, {">=", TK_CGE},
        {"<=", TK_CLE}, {"<>", TK_CNE}, {":=", TK_ASGN}, {"..", TK_DBLPOINT}
};

static special specials [16] {
        {"+", TK_ADD}, {"-", TK_SUB}, {"*", TK_MUL}, {"/", TK_DIV_S}, {"^", TK_POINTER},
        {">", TK_CGT}, {"<", TK_CLT}, {"@", TK_ADDR}, {".", TK_POINT}, {"(", TK_LB}, {")", TK_RB},
        {"[", TK_LSQB}, {"]", TK_RSQB}, {";", TK_SEMICOLON}, {":", TK_COLON}, {",", TK_COMMA}
};

#endif //PASCAL_COMPILER_LEXEMS_H
