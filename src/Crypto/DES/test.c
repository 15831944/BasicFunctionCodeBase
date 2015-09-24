
#include <stdlib.h>
#include <stdio.h>
#include "bool.h"   // 位处理 
#include "tables.h"
#include "des.h"




int main()
{
    int i = 0;
    char MesHex[16] = {0};       // 16个字符数组用于存放 64位16进制的密文
    char MyKey[8] = {0};         // 初始密钥 8字节*8
    char YourKey[8] = {0};       // 输入的解密密钥 8字节*8
    char MyMessage[8] = {0};     // 初始明文

    /*-----------------------------------------------*/

    printf("Welcome! Please input your Message(64 bit):\n");
    gets(MyMessage);            // 明文
    printf("Please input your Secret Key:\n");
    gets(MyKey);                // 密钥

    while (MyKey[i] != '\0') {   // 计算密钥长度
        i++;
    }

    while (i != 8) {             // 不是8 提示错误
        printf("Please input a correct Secret Key!\n");
        gets(MyKey);
        i = 0;
        while (MyKey[i] != '\0') { // 再次检测
            i++;
        }
    }

    SetKey(MyKey);               // 设置密钥 得到子密钥Ki

    PlayDes(MesHex, MyMessage);  // 执行DES加密

    printf("Your Message is Encrypted!:\n");  // 信息已加密
    for (i = 0; i < 16; i++) {
        printf("%c ", MesHex[i]);
    }
    printf("\n");
    printf("\n");

    printf("Please input your Secret Key to Deciphering:\n");  // 请输入密钥以解密
    gets(YourKey);                                         // 得到密钥
    SetKey(YourKey);                                       // 设置密钥

    KickDes(MyMessage, MesHex);                    // 解密输出到MyMessage

    printf("Deciphering Over !!:\n");                     // 解密结束
    for (i = 0; i < 8; i++) {
        printf("%c ", MyMessage[i]);
    }
    printf("\n");
    system("pause");

    /*------------------------------------------------*/
}