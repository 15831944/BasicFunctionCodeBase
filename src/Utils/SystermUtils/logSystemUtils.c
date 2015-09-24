#include <stdio.h>

#include "log/log_c.h"
#include "log_utils.h"


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 * 
 **/
int log_utils_init()
{

    //register to  module list
    g_moduleUtilsNO = registerModule(ModuleName_Utils, ModuleVersion_Utils, ModuleLogType_Utils);

    printf("==========================================\n");
#if 1
    utilsLogFatal("=====utilsLogFatal==[%d]=--------------=\n", g_moduleUtilsNO);
    utilsLogError("=====utilsLogError==[%d]=--------------=\n", g_moduleUtilsNO);
    utilsLogWarning("=====utilsLogWarning==[%d]=--------------=\n", g_moduleUtilsNO);
    utilsLogInfo("=====utilsLogInfo==[%d]=--------------=\n", g_moduleUtilsNO);
    utilsLogVerbose("=====utilsLogVerbose==[%d]=--------------=\n", g_moduleUtilsNO);
    utilsLogDebug("=====utilsLogDebug==[%d]=--------------=\n", g_moduleUtilsNO);
#endif
    return 0;
}

