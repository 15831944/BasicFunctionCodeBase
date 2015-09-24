#ifndef __LOG_THREADS_H__
#define __LOG_THREADS_H__


#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Threads       "threads"
#define ModuleVersion_Threads    g_make_svn_version
#define ModuleLogType_Threads    LogType_SECURITY

/** 
* g_moduleThreadsNO： 记录当前模块在模块列表中的位置
* g_threadsLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleThreadsNO;
static int g_threadsLogLevel = LOG_LEVEL_Warning;

#define threadsLogFatal(args...)     LogFatal(g_moduleThreadsNO, g_threadsLogLevel, args)
#define threadsLogError(args...)     LogError(g_moduleThreadsNO, g_threadsLogLevel, args)
#define threadsLogWarning(args...)   LogWarning(g_moduleThreadsNO, g_threadsLogLevel, args)
#define threadsLogInfo(args...)      LogInfo(g_moduleThreadsNO, g_threadsLogLevel, args)
#define threadsLogVerbose(args...)   LogVerbose(g_moduleThreadsNO, g_threadsLogLevel, args)
#define threadsLogDebug(args...)     LogDebug(g_moduleThreadsNO, g_threadsLogLevel, args)



#endif //__LOG_THREADS_H__

