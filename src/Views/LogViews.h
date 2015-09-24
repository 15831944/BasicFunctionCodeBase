#ifndef __LOG_UI_H__
#define __LOG_UI_H__

#include "log/LogC.h"
#include "build_info.h"

#define ModuleName_Views       "Views"
#define ModuleVersion_Views    g_make_svn_version
#define ModuleLogType_Views    LogType_SECURITY

/** 
* g_moduleViewsNO： 记录当前模块在模块列表中的位置
* g_viewsLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleViewsNO;
static int g_viewsLogLevel = LOG_LEVEL_Debug;

#define viewsLogFatal(args...)     LogFatal(g_moduleViewsNO, g_viewsLogLevel, args)
#define viewsLogError(args...)     LogError(g_moduleViewsNO, g_viewsLogLevel, args)
#define viewsLogWarning(args...)   LogWarning(g_moduleViewsNO, g_viewsLogLevel, args)
#define viewsLogInfo(args...)      LogInfo(g_moduleViewsNO, g_viewsLogLevel, args)
#define viewsLogVerbose(args...)   LogVerbose(g_moduleViewsNO, g_viewsLogLevel, args)
#define viewsLogDebug(args...)     LogDebug(g_moduleViewsNO, g_viewsLogLevel, args)



#endif

