/**
 *  InitPlayer.c文件
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 *
 **/



#include <stdio.h>

#include "log/LogC.h"
#include "../LogPlayer.h"
#include "Player.h"
#include "UtilityTypeConversion.h"







/**
 *  Name:: IntToString
 *          将各种进制的数字转换成字符串，注意进制的选择；
 *  Para::  number - 进制数字
 *          strBuf - 保存buffer
 *          radix - 进制
 **/
char *IntToString(int number, char *strBuf, int radix)
{
    char indexTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; /*索引表*/
    unsigned uMiddleNumber; /*中间变量*/
    int index = 0, j, k;
    char temp;

    printf("-IntToString--radix[%d] transform.\n", radix);

    /*确定unum的值*/
    if (radix == 10 && number < 0) { /*十进制负数*/
        uMiddleNumber = (unsigned)(-number);
        strBuf[index++] = '-';
    } else {
        uMiddleNumber = (unsigned)number; /*其他情况*/
    }

    /*转换*/
    do {
        strBuf[index++] = indexTable[uMiddleNumber % (unsigned)radix];
        uMiddleNumber /= radix;
    } while (uMiddleNumber);
    strBuf[index] = '\0';

    /*逆序*/
    if (strBuf[0] == '-') {
        k = 1; /*十进制负数*/
    } else {
        k = 0;
    }
    for (j = k; j <= (index - 1) / 2; j++) {
        temp = strBuf[j];
        strBuf[j] = strBuf[index - 1 + k - j];
        strBuf[index - 1 + k - j] = temp;
    }

    printf("--213--itoa--[%d] transformed into [%s].\n", number, strBuf);
    return strBuf;
}



















