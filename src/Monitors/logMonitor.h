#ifndef __LOG_Monitor_H__
#define __LOG_Monitor_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Monitor       "monitor"
#define ModuleVersion_Monitor    g_make_svn_version //"4343"  //
#define ModuleLogType_Monitor    LogType_SECURITY

/** 
* g_moduleMonitorNO： 记录当前模块在模块列表中的位置
* g_monitorLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleMonitorNO;
static int g_monitorLogLevel = LOG_LEVEL_Debug;

#define monitorLogFatal(args...)     LogFatal(g_moduleMonitorNO, g_monitorLogLevel, args)
#define monitorLogError(args...)     LogError(g_moduleMonitorNO, g_monitorLogLevel, args)
#define monitorLogWarning(args...)   LogWarning(g_moduleMonitorNO, g_monitorLogLevel, args)
#define monitorLogInfo(args...)      LogInfo(g_moduleMonitorNO, g_monitorLogLevel, args)
#define monitorLogVerbose(args...)   LogVerbose(g_moduleMonitorNO, g_monitorLogLevel, args)
#define monitorLogDebug(args...)     LogDebug(g_moduleMonitorNO, g_monitorLogLevel, args)


#endif

