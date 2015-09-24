#ifndef __LOG_Utils_H__
#define __LOG_Utils_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Utils       "utils"
#define ModuleVersion_Utils    g_make_svn_version //"4343"  //
#define ModuleLogType_Utils    LogType_SECURITY

/** 
* g_moduleUtilsNO： 记录当前模块在模块列表中的位置
* g_utilsLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleUtilsNO;
static int g_utilsLogLevel = LOG_LEVEL_Debug;

#define utilsLogFatal(args...)     LogFatal(g_moduleUtilsNO, g_utilsLogLevel, args)
#define utilsLogError(args...)     LogError(g_moduleUtilsNO, g_utilsLogLevel, args)
#define utilsLogWarning(args...)   LogWarning(g_moduleUtilsNO, g_utilsLogLevel, args)
#define utilsLogInfo(args...)      LogInfo(g_moduleUtilsNO, g_utilsLogLevel, args)
#define utilsLogVerbose(args...)   LogVerbose(g_moduleUtilsNO, g_utilsLogLevel, args)
#define utilsLogDebug(args...)     LogDebug(g_moduleUtilsNO, g_utilsLogLevel, args)


#endif

