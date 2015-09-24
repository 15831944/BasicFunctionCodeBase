#ifndef __UTILITY_TYPE_CONVERSION_H__
#define __UTILITY_TYPE_CONVERSION_H__



enum RadixTable{
    RADIX_RESERVED,         
    RADIX_HEXADECIMAL,      //16进制
    RADIX_DECIMALIST,       //10进制
    RADIX_OCTONARY,         //8进制
    RADIX_QUATERNARY,       //4进制
    RADIX_BINARY,           //2进制
    RADIX_MAX,
};


int TimeDelay(const int delayMode, int delaySecond, int delayMicroSecond);















#endif  //__UTILITY_TYPE_CONVERSION_H__

