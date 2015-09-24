
#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "LogC.h"

#define ModuleName_UPGRADE       "upgrade"
#define ModuleVersion_UPGRADE    "1111"
#define ModuleLogType_UPGRADE    LogType_SECURITY

/** 
* g_moduleUpgradeNO： 记录当前模块在模块列表中的位置
* g_upgradeLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
int g_moduleUpgradeNO = 0;
static int g_upgradeLogLevel = LOG_LEVEL_Debug;


#define upgradeLogFatal(args...)     LogFatal(g_moduleUpgradeNO, g_upgradeLogLevel, args)
#define upgradeLogError(args...)     LogError(g_moduleUpgradeNO, g_upgradeLogLevel, args)
#define upgradeLogWarning(args...)   LogWarning(g_moduleUpgradeNO, g_upgradeLogLevel, args)
#define upgradeLogInfo(args...)      LogInfo(g_moduleUpgradeNO, g_upgradeLogLevel, args)
#define upgradeLogVerbose(args...)   LogVerbose(g_moduleUpgradeNO, g_upgradeLogLevel, args)
#define upgradeLogDebug(args...)     LogDebug(g_moduleUpgradeNO, g_upgradeLogLevel, args)


#endif
