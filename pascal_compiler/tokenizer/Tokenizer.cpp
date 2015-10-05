//
// Created by Locky on 30.09.2015.
//

#include "Tokenizer.h"

Tokenizer::Tokenizer(ifstream *file): file(file),
                                      currentStr(1),
                                      currentPos(0),
                                      globalState(ST_BEGIN),
                                      currentTok(Token()),
                                      eof(false) {}

Token Tokenizer::Next() {
    if (eof)
        return Token();
    if (!file->is_open()) {
        eof = true;
        return Token();
    }
    States currentState = ST_BEGIN;
    string real_src = "";
    string lex_data = "";
    int exp_sign = 1;
    Token result(currentStr, 0, TK_ERROR, "", 0, 0, 0);
    while (true) {
        char symbol = (char)file->get();
        currentPos++;
        States newState = StateController::StatesTable[symbol][currentState];
        if (globalState == ST_DBLPOINT) {
            newState = ST_BEGIN;
            globalState = ST_BEGIN;
        }
        if (file->eof()) {
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
                        for (int i = 0; i < 30; ++i)
                            if (tmp.compare(keyWords[i].text) == 0) {
                                result.tokenType = keyWords[i].type;
                                break;
                            }
                        result.text = lex_data;
                        break;
                    }
                    case TK_INT:
                        result.text = lex_data;
                        result.data1 = std::stoi(lex_data);
                        break;
                    case TK_REAL:
                        result.text = real_src;
                        result.data1 = std::stoi(lex_data);
                        if (result.data2 != 0)
                            result.data2 = result.data2 - (int)lex_data.length();
                        result.data3 *= exp_sign;
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
                            result.data1 += ch * pow(16, i);
                        }
                        break;
                    case TK_CHAR:
                        result.text = real_src;
                        if (!lex_data.empty()) {
                            for (int i = 0; i < lex_data.length(); ++i) {
                                char ch = lex_data[(int) lex_data.length() - i - 1];
                                if (ch > 'F')
                                    ch -= 32;
                                result.data1 += (ch - 55) * pow(16, i);
                            }
                        }
                        globalState = ST_BEGIN;
                        break;
                    case TK_MUL:
                        result.text = lex_data;
                        for (int i = 0; i < 16; ++i)
                            if (result.text.compare(specials[i].s) == 0) {
                                result.tokenType = specials[i].type;
                                break;
                            }
                        break;
                    case TK_MULE:
                        result.text = lex_data;
                        for (int i = 0; i < 9; ++i)
                            if (result.text.compare(specialDbls[i].sD) == 0) {
                                result.tokenType = specialDbls[i].type;
                                break;
                            }
                        break;
                }
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
                    result.data2 = (int)lex_data.length();
                else
                    lex_data += symbol;
                break;
            case ST_DBLPOINT:
                result.tokenType = TK_INT;
                globalState = ST_DBLPOINT;
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
                result.data3 = result.data3 * 10 + (int)symbol - 48;
                break;
            case ST_NUMBER_HEX:
                if (result.strPos == 0)
                    result.strPos = currentPos;
                if (globalState == ST_CHAR)
                    result.tokenType = TK_CHAR;
                else
                    result.tokenType = TK_HEX;
                real_src += symbol;
                if (symbol != '$')
                    lex_data += symbol;
                break;
            case ST_CHAR:
                globalState = ST_CHAR;
                if (result.strPos == 0)
                    result.strPos = currentPos;
                result.tokenType = TK_CHAR;
                real_src += symbol;
                break;
            case ST_CHAR_NUM:
                real_src += symbol;
                result.data1 = result.data1 * 10 + (int)symbol - 48;
                break;
            case ST_STRING_OP:
                globalState = ST_STRING_OP;
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
                    result.data1 = (int)result.text[1];
                }
                currentTok = Token(result);
                globalState = ST_BEGIN;
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
                result.tokenType = TK_MULE;
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
