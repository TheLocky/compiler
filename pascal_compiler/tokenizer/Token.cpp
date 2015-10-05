//
// Created by xtpvx on 30.09.2015.
//

#include "Token.h"

Token::Token() {
    strNum = 0;
    strPos = 0;
    tokenType = (LEX_TYPE)0;
    text = "";
    data1 = 0;
    data2 = 0;
    data3 = 0;
}

Token::Token(const Token &src) {
    strNum = src.strNum;
    strPos = src.strPos;
    tokenType = src.tokenType;
    text = src.text;
    data1 = src.data1;
    data2 = src.data2;
    data3 = src.data3;
}

int Token::getTokenType(LEX_TYPE lt) {
    switch (lt) {
        case TK_ID: return 1;//"ident"
        case TK_INT: return 2;//"integer"
        case TK_REAL: return 3;//"real"
        case TK_HEX: return 4;//"hex"
        case TK_CHAR: return 5;//"char"
        case TK_STRING: return 6;//"string"
        case TK_PROGRAM:
        case TK_BEGIN:
        case TK_END:
        case TK_FWD:
        case TK_FUNCTION:
        case TK_PROCEDURE:
        case TK_RECORD:
        case TK_EXIT:
        case TK_IF:
        case TK_THEN:
        case TK_ELSE:
        case TK_FOR:
        case TK_TO:
        case TK_DOWNTO:
        case TK_WHILE:
        case TK_DO:
        case TK_REPEAT:
        case TK_UNTIL:
        case TK_BREAK:
        case TK_CONTINUE:
        case TK_VAR:
        case TK_TYPE:
        case TK_ARRAY:
        case TK_OF:
        case TK_AND:
        case TK_NOT:
        case TK_OR:
        case TK_XOR:
        case TK_DIV:
        case TK_MOD: return 7;//"keyword"
        case TK_CGT:
        case TK_CLT:
        case TK_CGE:
        case TK_CLE:
        case TK_CE:
        case TK_CNE:
        case TK_ADD:
        case TK_SUB:
        case TK_MUL:
        case TK_DIV_S:
        case TK_ADDE:
        case TK_SUBE:
        case TK_MULE:
        case TK_DIVE:
        case TK_ASGN:
        case TK_POINTER:
        case TK_ADDR: return 8;//"op"
        case TK_RB:
        case TK_LB:
        case TK_RSQB:
        case TK_LSQB:
        case TK_SEMICOLON:
        case TK_COLON:
        case TK_DBLPOINT:
        case TK_POINT:
        case TK_COMMA: return 9;//"sep"
    }
}

string Token::getTokenTypeStr(LEX_TYPE lt) {
    int code = getTokenType(lt);
    switch (code) {
        case 1: return "ident";
        case 2: return "integer";
        case 3: return "real";
        case 4: return "hex";
        case 5: return "char";
        case 6: return "string";
        case 7: return "keyword";
        case 8: return "op";
        case 9: return "sep";
        default: return "";
    }
}

void Token::print() {
    int tt = getTokenType(tokenType);
    printf("%d\t%d\t%s\t%s\t", strNum, strPos, getTokenTypeStr(tokenType).c_str(), text.c_str());
    switch (tt) {
        case 2:
        case 4:
            printf("%d\n", data1);
            break;
        case 3: {
            double real = data1 * pow(10, data2 + data3);
            printf("%.4E\n", real);
            break;
        }
        case 5:
            printf("%c\n", (char)data1);
            break;
        case 6:
            printf("%s\n", text.substr(1, text.length() - 2).c_str());
            break;
        default:
            printf("\n");
            break;
    }
}
