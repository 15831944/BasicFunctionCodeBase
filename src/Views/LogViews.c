#include <stdio.h>

#include "LogC.h"
#include "LogViews.h"
#include "Views.h"



int g_moduleViewsNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_views_init()
{
    //register to  module list
    g_moduleViewsNO = registerModule(ModuleName_Views, ModuleVersion_Views, ModuleLogType_Views);

    printf("====================log_views_init======================\n");
#if 1
    viewsLogFatal("=====viewsLogFatal==[%d]=--------------=\n", g_moduleViewsNO);
    viewsLogError("=====viewsLogError==[%d]=--------------=\n", g_moduleViewsNO);
    viewsLogWarning("=====viewsLogWarning==[%d]=--------------=\n", g_moduleViewsNO);
    viewsLogInfo("=====viewsLogInfo==[%d]=--------------=\n", g_moduleViewsNO);
    viewsLogVerbose("=====viewsLogVerbose==[%d]=--------------=\n", g_moduleViewsNO);
    viewsLogDebug("=====viewsLogDebug==[%d]=--------------=\n", g_moduleViewsNO);
#endif
    return 0;
}

