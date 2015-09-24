#ifndef __DES_H__
#define __DES_H__

#include "bool.h"   // 位处理 
#include "tables.h"

void BitsCopy(bool *DatOut,bool *DatIn,int Len);  // 数组复制 

void ByteToBit(bool *DatOut,char *DatIn,int Num); // 字节到位 
void BitToByte(char *DatOut,bool *DatIn,int Num); // 位到字节

void BitToHex(char *DatOut,bool *DatIn,int Num);  // 二进制到十六进制 64位 to 4*16字符
void HexToBit(bool *DatOut,char *DatIn,int Num);  // 十六进制到二进制 

void TablePermute(bool *DatOut,bool *DatIn,const char *Table,int Num); // 位表置换函数 
void LoopMove(bool *DatIn,int Len,int Num);     // 循环左移 Len长度 Num移动位数 
void Xor(bool *DatA,bool *DatB,int Num);         // 异或函数 

void S_Change(bool DatOut[32],bool DatIn[48]);   // S盒变换 
void F_Change(bool DatIn[32],bool DatKi[48]);    // F函数                                  

void SetKey(char KeyIn[8]);                         // 设置密钥
void PlayDes(char MesOut[8],char MesIn[8]);       // 执行DES加密
void KickDes(char MesOut[8],char MesIn[8]);             // 执行DES解密 


#endif