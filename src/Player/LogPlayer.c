#include <stdio.h>

#include "LogC.h"
#include "LogPlayer.h"
#include "Player.h"


int g_modulePlayerNO;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 **/
int log_player_init()
{
    //register to  module list
    g_modulePlayerNO = registerModule(ModuleName_PLAYER, ModuleVersion_PLAYER, ModuleLogType_PLAYER);

    printf("================log_player_init==========================\n");
#if 1
    playerLogFatal("=====playerLogFatal==[%d]=--------------=\n", g_modulePlayerNO);
    playerLogError("=====playerLogError==[%d]=--------------=\n", g_modulePlayerNO);
    playerLogWarning("=====playerLogWarning==[%d]=--------------=\n", g_modulePlayerNO);
    playerLogInfo("=====playerLogInfo==[%d]=--------------=\n", g_modulePlayerNO);
    playerLogVerbose("=====playerLogVerbose==[%d]=--------------=\n", g_modulePlayerNO);
    playerLogDebug("=====playerLogDebug==[%d]=--------------=\n", g_modulePlayerNO);
#endif
    return 0;
}

