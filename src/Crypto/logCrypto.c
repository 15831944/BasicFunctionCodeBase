#include <stdio.h>

#include "Log/LogC.h"
#include "logCrypto.h"


int g_moduleCryptoNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_crypto_init()
{

    //register to  module list
    g_moduleCryptoNO = registerModule(ModuleName_Crypto, ModuleVersion_Crypto, ModuleLogType_Crypto);

    printf("==========================================\n");
#if 1
    cryptoLogFatal("=====cryptoLogFatal==[%d]=--------------=\n", g_moduleCryptoNO);
    cryptoLogError("=====cryptoLogError==[%d]=--------------=\n", g_moduleCryptoNO);
    cryptoLogWarning("=====cryptoLogWarning==[%d]=--------------=\n", g_moduleCryptoNO);
    cryptoLogInfo("=====cryptoLogInfo==[%d]=--------------=\n", g_moduleCryptoNO);
    cryptoLogVerbose("=====cryptoLogVerbose==[%d]=--------------=\n", g_moduleCryptoNO);
    cryptoLogDebug("=====cryptoLogDebug==[%d]=--------------=\n", g_moduleCryptoNO);
#endif
    return 0;
}

