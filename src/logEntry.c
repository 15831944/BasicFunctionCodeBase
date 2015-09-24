#include <stdio.h>

#include "LogC.h"
#include "LogEntry.h"
#include "Entry.h"


int g_moduleEntryNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_entry_init()
{
    //register to  module list
    g_moduleEntryNO = registerModule(ModuleName_Entry, ModuleVersion_Entry, ModuleLogType_Entry);

    printf("==================log_entry_init========================\n");
#if 1
    entryLogFatal("=====entryLogFatal==[%d]=--------------=\n", g_moduleEntryNO);
    entryLogError("=====entryLogError==[%d]=--------------=\n", g_moduleEntryNO);
    entryLogWarning("=====entryLogWarning==[%d]=--------------=\n", g_moduleEntryNO);
    entryLogInfo("=====entryLogInfo==[%d]=--------------=\n", g_moduleEntryNO);
    entryLogVerbose("=====entryLogVerbose==[%d]=--------------=\n", g_moduleEntryNO);
    entryLogDebug("=====entryLogDebug==[%d]=--------------=\n", g_moduleEntryNO);
#endif
    return 0;
}
