
#ifndef __BUILD_H__
#define __BUILD_H__

#include "LogC.h"

#define ModuleName_BUILD       "build"
#define ModuleVersion_BUILD    "45343"
#define ModuleLogType_BUILD    LogType_ALL

/** 
* g_moduleBuildNO： 记录当前模块在模块列表中的位置
* g_buildLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
int g_moduleBuildNO = 0;
static int g_buildLogLevel = LOG_LEVEL_Debug;


#define buildLogFatal(args...)     LogFatal(g_moduleBuildNO, g_buildLogLevel, args)
#define buildLogError(args...)     LogError(g_moduleBuildNO, g_buildLogLevel, args)
#define buildLogWarning(args...)   LogWarning(g_moduleBuildNO, g_buildLogLevel, args)
#define buildLogInfo(args...)      LogInfo(g_moduleBuildNO, g_buildLogLevel, args)
#define buildLogVerbose(args...)   LogVerbose(g_moduleBuildNO, g_buildLogLevel, args)
#define buildLogDebug(args...)     LogDebug(g_moduleBuildNO, g_buildLogLevel, args)


#endif



