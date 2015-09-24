#include <stdio.h>

#include "log/LogC.h"
#include "LogThreads.h"
#include "Threads.h"

int g_moduleThreadsNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 **/
int log_threads_init()
{
    //register to  module list
    g_moduleThreadsNO = registerModule(ModuleName_Threads, ModuleVersion_Threads, ModuleLogType_Threads);

    printf("===================log_threads_init=======================\n");
#if 1
    threadsLogFatal("=====threadsLogFatal==[%d]=--------------=\n", g_moduleThreadsNO);
    threadsLogError("=====threadsLogError==[%d]=--------------=\n", g_moduleThreadsNO);
    threadsLogWarning("=====threadsLogWarning==[%d]=--------------=\n", g_moduleThreadsNO);
    threadsLogInfo("=====threadsLogInfo==[%d]=--------------=\n", g_moduleThreadsNO);
    threadsLogVerbose("=====threadsLogVerbose==[%d]=--------------=\n", g_moduleThreadsNO);
    threadsLogDebug("=====threadsLogDebug==[%d]=--------------=\n", g_moduleThreadsNO);
#endif
    return 0;
}

