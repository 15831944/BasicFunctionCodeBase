#include <stdio.h>
#include "LogC.h"

#include "test/upgrade.h"
#include "test/example.h"

extern struct _Version  g_version;


int log_init()
{
    //register to  module list
    g_moduleUpgradeNO = registerModule(ModuleName_UPGRADE, ModuleVersion_UPGRADE, ModuleLogType_UPGRADE);
    g_moduleBuildNO =  registerModule(ModuleName_BUILD, ModuleVersion_BUILD, ModuleLogType_BUILD);

    return 0;
}

int main()
{
    //init g_version
    g_version.Major = 9;
    g_version.Minor = 88;
    g_version.BuildVersion = 777;

    log_init();

    upgradeLogFatal("=====upgradeLogFatal==[%d]=--------------=\n", g_moduleUpgradeNO);
    upgradeLogError("=====upgradeLogError==[%d]=--------------=\n", g_moduleUpgradeNO);
    upgradeLogWarning("=====upgradeLogWarning==[%d]=--------------=\n", g_moduleUpgradeNO);
    upgradeLogInfo("=====upgradeLogInfo==[%d]=--------------=\n", g_moduleUpgradeNO);
    upgradeLogVerbose("=====upgradeLogVerbose==[%d]=--------------=\n", g_moduleUpgradeNO);
    upgradeLogDebug("=====upgradeLogDebug==[%d]=--------------=\n", g_moduleUpgradeNO);

    printf("==========================================\n");
    buildLogFatal("=====buildLogFatal==[%d]=--------------=\n", g_moduleBuildNO);
    buildLogError("=====buildLogError==[%d]=--------------=\n", g_moduleBuildNO);
    buildLogWarning("=====buildLogWarning==[%d]=--------------=\n", g_moduleBuildNO);
    buildLogInfo("=====buildLogInfo==[%d]=--------------=\n", g_moduleBuildNO);
    buildLogVerbose("=====buildLogVerbose==[%d]=--------------=\n", g_moduleBuildNO);
    buildLogDebug("=====buildLogDebug==[%d]=--------------=\n", g_moduleBuildNO);


    return 0;
}






