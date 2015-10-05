//
// Created by Locky on 28.09.2015.
//

#ifndef PASCAL_COMPILER_STATECONTROLLER_H
#define PASCAL_COMPILER_STATECONTROLLER_H

#include <cstdio>
#define ST_COUNT 100

enum States {
    ST_SKIP = -1, // space & \t
    ST_BEGIN = 0, //��������� ���������
    ST_IDENT, //�������������
    ST_NUMBER_ALL, //����� �����
    ST_NUMBER_DBL_FIRST, ST_NUMBER_DBL,  //���� ����� � ��������� ������
    ST_NUMBER_EXP, ST_NUMBER_EXP_SIGN, ST_NUMBER_EXP_VAL, //���� ����������
    ST_NUMBER_HEX, //HEX-�����
    ST_STRING_OP, ST_STRING_CL, ST_STR_SCREEN, //���� ������
    ST_CHAR, ST_CHAR_NUM, //���������� ������ #123
    ST_COMMENT1_OP, ST_COMMENT1_CL, //����������� { }
    ST_COMMENT2_OP, ST_COMMENT2_CL, //����������� //
    ST_SPECIAL, //������ ����������� ������ (�������������� �������)
    ST_SPECIAL_E, //= ����� �����������
    ST_SPECIAL_EB, //> ���� = ����� �����������
    ST_SPECIAL_P, //. ����� �����������
    ST_SPECIAL_DBL, //������������� �������
    ST_DBLPOINT, //������� �����
    ST_BADCHAR = 50, //����������� ������
    ST_NOFRACT, //�� ����� ����� ����� � double
    ST_NOEXP, //�� ����� ���� ����������
    ST_NOHEX, //�� ����������������� ������� � �����
    ST_BADNL, //������ �������� ������ � �������� �������
    ST_NOCC //�� ����� ����� #
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
