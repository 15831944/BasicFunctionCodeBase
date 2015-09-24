#include <stdio.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "Settings.h"


int g_moduleSettingsNO;


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_settings_init()
{

    //register to  module list
    g_moduleSettingsNO = registerModule(ModuleName_Settings, ModuleVersion_Settings, ModuleLogType_Settings);

    printf("==========================================\n");
#if 1
    settingsLogFatal("=====settingsLogFatal==[%d]=--------------=\n", g_moduleSettingsNO);
    settingsLogError("=====settingsLogError==[%d]=--------------=\n", g_moduleSettingsNO);
    settingsLogWarning("=====settingsLogWarning==[%d]=--------------=\n", g_moduleSettingsNO);
    settingsLogInfo("=====settingsLogInfo==[%d]=--------------=\n", g_moduleSettingsNO);
    settingsLogVerbose("=====settingsLogVerbose==[%d]=--------------=\n", g_moduleSettingsNO);
    settingsLogDebug("=====settingsLogDebug==[%d]=--------------=\n", g_moduleSettingsNO);
#endif
    return 0;
}

