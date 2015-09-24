#include <stdio.h>

#include "Log/LogC.h"
#include "logMessage.h"
#include "Message.h"


int g_moduleMessageNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_message_init()
{

    //register to  module list
    g_moduleMessageNO = registerModule(ModuleName_Message, ModuleVersion_Message, ModuleLogType_Message);

    printf("==================log_message_init========================\n");
#if 1
    messageLogFatal("=====messageLogFatal==[%d]=--------------=\n", g_moduleMessageNO);
    messageLogError("=====messageLogError==[%d]=--------------=\n", g_moduleMessageNO);
    messageLogWarning("=====messageLogWarning==[%d]=--------------=\n", g_moduleMessageNO);
    messageLogInfo("=====messageLogInfo==[%d]=--------------=\n", g_moduleMessageNO);
    messageLogVerbose("=====messageLogVerbose==[%d]=--------------=\n", g_moduleMessageNO);
    messageLogDebug("=====messageLogDebug==[%d]=--------------=\n", g_moduleMessageNO);
#endif
    return 0;
}

