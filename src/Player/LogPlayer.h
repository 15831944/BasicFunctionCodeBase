#ifndef __LOG_PLAYER_H__
#define __LOG_PLAYER_H__

#include "log/LogC.h"
#include "build_info.h"

#define ModuleName_PLAYER       "player"
#define ModuleVersion_PLAYER    g_make_svn_version
#define ModuleLogType_PLAYER    LogType_SECURITY

/** 
* g_modulePlayerNO： 记录当前模块在模块列表中的位置
* g_playerLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_modulePlayerNO;
static int g_playerLogLevel = LOG_LEVEL_Warning;

#define playerLogFatal(args...)     LogFatal(g_modulePlayerNO, g_playerLogLevel, args)
#define playerLogError(args...)     LogError(g_modulePlayerNO, g_playerLogLevel, args)
#define playerLogWarning(args...)   LogWarning(g_modulePlayerNO, g_playerLogLevel, args)
#define playerLogInfo(args...)      LogInfo(g_modulePlayerNO, g_playerLogLevel, args)
#define playerLogVerbose(args...)   LogVerbose(g_modulePlayerNO, g_playerLogLevel, args)
#define playerLogDebug(args...)     LogDebug(g_modulePlayerNO, g_playerLogLevel, args)




#endif

