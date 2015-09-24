#include <stdio.h>

#include "Log/LogC.h"
#include "logMonitor.h"
#include "Monitors.h"


int g_moduleMonitorNO;


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_monitor_init()
{

    //register to  module list
    g_moduleMonitorNO = registerModule(ModuleName_Monitor, ModuleVersion_Monitor, ModuleLogType_Monitor);

    printf("==========================================\n");
#if 1
    monitorLogFatal("=====monitorLogFatal==[%d]=--------------=\n", g_moduleMonitorNO);
    monitorLogError("=====monitorLogError==[%d]=--------------=\n", g_moduleMonitorNO);
    monitorLogWarning("=====monitorLogWarning==[%d]=--------------=\n", g_moduleMonitorNO);
    monitorLogInfo("=====monitorLogInfo==[%d]=--------------=\n", g_moduleMonitorNO);
    monitorLogVerbose("=====monitorLogVerbose==[%d]=--------------=\n", g_moduleMonitorNO);
    monitorLogDebug("=====monitorLogDebug==[%d]=--------------=\n", g_moduleMonitorNO);
#endif
    return 0;
}

