#include <stdio.h>

#include "Log/LogC.h"
#include "logMediaInfo.h"
#include "MediaInfo.h"


int g_moduleMediaInfoNO;


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_media_info_init()
{

    //register to  module list
    g_moduleMediaInfoNO = registerModule(ModuleName_MediaInfo, ModuleVersion_MediaInfo, ModuleLogType_MediaInfo);

    printf("=================log_media_info_init=========================\n");
#if 1
    media_infoLogFatal("=====media_infoLogFatal==[%d]=--------------=\n", g_moduleMediaInfoNO);
    media_infoLogError("=====media_infoLogError==[%d]=--------------=\n", g_moduleMediaInfoNO);
    media_infoLogWarning("=====media_infoLogWarning==[%d]=--------------=\n", g_moduleMediaInfoNO);
    media_infoLogInfo("=====media_infoLogInfo==[%d]=--------------=\n", g_moduleMediaInfoNO);
    media_infoLogVerbose("=====media_infoLogVerbose==[%d]=--------------=\n", g_moduleMediaInfoNO);
    media_infoLogDebug("=====media_infoLogDebug==[%d]=--------------=\n", g_moduleMediaInfoNO);
#endif
    return 0;
}

