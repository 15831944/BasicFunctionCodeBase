#ifndef __LOG_NETWORK_H__
#define __LOG_NETWORK_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Network       "network"
#define ModuleVersion_Network    g_make_svn_version //"4343"  //
#define ModuleLogType_Network    LogType_SECURITY

/** 
* g_moduleNetworkNO： 记录当前模块在模块列表中的位置
* g_networkLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
int g_moduleNetworkNO;
static int g_networkLogLevel = LOG_LEVEL_Debug;

#define networkLogFatal(args...)     LogFatal(g_moduleNetworkNO, g_networkLogLevel, args)
#define networkLogError(args...)     LogError(g_moduleNetworkNO, g_networkLogLevel, args)
#define networkLogWarning(args...)   LogWarning(g_moduleNetworkNO, g_networkLogLevel, args)
#define networkLogInfo(args...)      LogInfo(g_moduleNetworkNO, g_networkLogLevel, args)
#define networkLogVerbose(args...)   LogVerbose(g_moduleNetworkNO, g_networkLogLevel, args)
#define networkLogDebug(args...)     LogDebug(g_moduleNetworkNO, g_networkLogLevel, args)


#endif  //__LOG_NETWORK_H__

