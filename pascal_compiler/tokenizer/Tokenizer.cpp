//
// Created by Locky on 30.09.2015.
//

#include "Tokenizer.h"
#include "StatesData.h"
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

Tokenizer::Tokenizer(ifstream *file): file(file),
                                      currentStr(1),
                                      currentPos(0),
                                      currentTok(Token()),
                                      eof(false) { Next(); }

Token Tokenizer::Next() {
    if (eof)
        return Token();
    if (!file->is_open()) {
        eof = true;
        return Token();
    }
    StatesEnum currentState = ST_BEGIN;
    StatesEnum additionalState = ST_BEGIN;

    string real_src = "";
    string lex_data = "";
    int fract = 0;
    int exp = 0;
    int exp_sign = 1;
    bool __eof = false;

    Token result(currentStr, currentPos + 1, TK_ERROR, "", 0, 0);
    while (true) {
        char symbol = (char)file->get();
        __eof = file->eof();
        currentPos++;
        StatesEnum newState = States::Table[symbol][currentState];
        if (additionalState == ST_DBLPOINT) {
            newState = ST_BEGIN;
            additionalState = ST_BEGIN;
        }
        if (__eof) {
            if (currentState == ST_BEGIN)
                eof = true;
            if (currentState == ST_STRING_OP || currentState == ST_COMMENT2_OP)
                throw TokenException(result.strNum, result.strPos, result.text, "BadEOF");
            newState = ST_BEGIN;
        }
        switch (newState) {
            case ST_SKIP:
                if (symbol != ' ')
                    currentPos--;
                break;
            case ST_BEGIN:
                switch (result.tokenType) {
                    case TK_ID: {
                        string tmp = lex_data;
                        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
                        for (int i = 0; i < ARRAY_SIZE(keyWords); ++i)
                            if (tmp.compare(keyWords[i].text) == 0) {
                                result.tokenType = keyWords[i].type;
                                break;
                            }
                        result.text = lex_data;
                        break;
                    }
                    case TK_INT:
                        result.text = lex_data;
                        result.intData = std::stoi(lex_data);
                        break;
                    case TK_REAL:
                        result.text = real_src;
                        if (fract != 0)
                            fract -= (int)lex_data.length();
                        exp *= exp_sign;
                        result.realData = std::stoi(lex_data) * pow(10, fract + exp);
                        break;
                    case TK_HEX:
                        result.text = real_src;
                        for (int i = 0; i < lex_data.length(); ++i) {
                            char ch = lex_data[(int)lex_data.length() - i - 1];
                            if (ch > 'F')
                                ch -= 32;
                            if (ch < 'A')
                                ch -= 48;
                            else
                                ch -= 55;
                            result.intData += ch * pow(16, i);
                        }
                        break;
                    case TK_CHAR:
                        result.text = real_src;
                        if (!lex_data.empty()) {
                            for (int i = 0; i < lex_data.length(); ++i) {
                                char ch = lex_data[(int) lex_data.length() - i - 1];
                                if (ch > 'F')
                                    ch -= 32;
                                result.intData += (ch - 55) * pow(16, i);
                            }
                        }
                        break;
                    case TK_MUL:
                        result.text = lex_data;
                        for (int i = 0; i < ARRAY_SIZE(specials); ++i)
                            if (result.text.compare(specials[i].s) == 0) {
                                result.tokenType = specials[i].type;
                                break;
                            }
                        break;
                    case TK_MUL_E:
                        result.text = lex_data;
                        for (int i = 0; i < ARRAY_SIZE(specialDbls); ++i)
                            if (result.text.compare(specialDbls[i].sD) == 0) {
                                result.tokenType = specialDbls[i].type;
                                break;
                            }
                        break;
                }
                if (!__eof)
                    file->unget();
                currentPos--;
                currentTok = Token(result);
                return result;
            case ST_IDENT:
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_ID;
                lex_data += symbol;
                break;
            case ST_NUMBER_ALL:
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_INT;
                real_src += symbol;
                lex_data += symbol;
                break;
            case ST_NUMBER_DBL_FIRST:
            case ST_NUMBER_DBL:
                result.tokenType = TK_REAL;
                real_src += symbol;
                if (symbol == '.')
                    fract = (int)lex_data.length();
                else
                    lex_data += symbol;
                break;
            case ST_DBLPOINT:
                result.tokenType = TK_INT;
                additionalState = ST_DBLPOINT;
                real_src = real_src.substr(0, real_src.length()-1);
                file->unget();
                file->unget();
                currentPos-=2;
                break;
            case ST_NUMBER_EXP:
                result.tokenType = TK_REAL;
                real_src += symbol;
                break;
            case ST_NUMBER_EXP_SIGN:
                real_src += symbol;
                if (symbol == '-')
                    exp_sign = -1;
                break;
            case ST_NUMBER_EXP_VAL:
                real_src += symbol;
                exp = exp * 10 + (int)symbol - 48;
                break;
            case ST_NUMBER_HEX:
                if (result.strPos == 0)
                    result.strPos = currentPos;
                if (additionalState == ST_CHAR)
                    result.tokenType = TK_CHAR;
                else
                    result.tokenType = TK_HEX;
                real_src += symbol;
                if (symbol != '$')
                    lex_data += symbol;
                break;
            case ST_CHAR:
                additionalState = ST_CHAR;
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_CHAR;
                real_src += symbol;
                break;
            case ST_CHAR_NUM:
                real_src += symbol;
                result.intData = result.intData * 10 + (int)symbol - 48;
                break;
            case ST_STRING_OP:
                additionalState = ST_STRING_OP;
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_STRING;
                real_src += symbol;
                break;
            case ST_STR_SCREEN:
                real_src += symbol;
                break;
            case ST_STRING_CL:
                real_src += symbol;
                result.text = real_src;
                if (result.text.length() == 3) {
                    result.tokenType = TK_CHAR;
                    result.intData = (int)result.text[1];
                }
                currentTok = Token(result);
                return result;
            //comments
            case ST_COMMENT1_OP:break;
            case ST_COMMENT1_CL:
                return Next();
            case ST_COMMENT2_OP:break;
            case ST_COMMENT2_CL:
                file->unget();
                return Next();
            //specials
            case ST_SPECIAL:
            case ST_SPECIAL_E:
            case ST_SPECIAL_EB:
            case ST_SPECIAL_P:
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_MUL;
                lex_data += symbol;
                break;
            case ST_SPECIAL_DBL:
                result.tokenType = TK_MUL_E;
                lex_data += symbol;
                break;

            case ST_BADCHAR:
                throw TokenException(result.strNum, result.strPos, result.text, "BadChar");
            case ST_NOFRACT:
                throw TokenException(result.strNum, result.strPos, result.text, "NoFract");
            case ST_NOEXP:
                throw TokenException(result.strNum, result.strPos, result.text, "NoExp");
            case ST_NOHEX:
                throw TokenException(result.strNum, result.strPos, result.text, "NoHex");
            case ST_BADNL:
                throw TokenException(result.strNum, result.strPos, result.text, "BadNL");
            case ST_NOCC:
                throw TokenException(result.strNum, result.strPos, result.text, "NoCC");
        }
        if (newState != ST_SKIP)
            currentState = newState;
        if (symbol == '\n') {
            currentStr++;
            currentPos = 0;
            result.strNum = currentStr;
            result.strPos = currentPos;
        }
    }
}

Token Tokenizer::Get() {
    return currentTok;
}

bool Tokenizer::Eof() {
    return eof;
}
