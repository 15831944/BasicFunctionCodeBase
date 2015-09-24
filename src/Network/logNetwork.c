#include <stdio.h>

#include "Log/LogC.h"
#include "logNetwork.h"


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 * 
 **/
int log_network_init()
{

    //register to  module list
    g_moduleNetworkNO = registerModule(ModuleName_Network, ModuleVersion_Network, ModuleLogType_Network);

    printf("==========================================\n");
#if 1
    networkLogFatal("=====networkLogFatal==[%d]=--------------=\n", g_moduleNetworkNO);
    networkLogError("=====networkLogError==[%d]=--------------=\n", g_moduleNetworkNO);
    networkLogWarning("=====networkLogWarning==[%d]=--------------=\n", g_moduleNetworkNO);
    networkLogInfo("=====networkLogInfo==[%d]=--------------=\n", g_moduleNetworkNO);
    networkLogVerbose("=====networkLogVerbose==[%d]=--------------=\n", g_moduleNetworkNO);
    networkLogDebug("=====networkLogDebug==[%d]=--------------=\n", g_moduleNetworkNO);
#endif
    return 0;
}

