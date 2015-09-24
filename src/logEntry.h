#ifndef __LOG_Entry_H__
#define __LOG_Entry_H__


#include "build_info.h"
#include "LogC.h"

#define ModuleName_Entry       "entry"
#define ModuleVersion_Entry    g_make_svn_version
#define ModuleLogType_Entry    LogType_SECURITY




/** 
* g_moduleEntryNO： 记录当前模块在模块列表中的位置
* g_entryLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleEntryNO;
static int g_entryLogLevel = LOG_LEVEL_Debug;

#define entryLogFatal(args...)     LogFatal(g_moduleEntryNO, g_entryLogLevel, args)
#define entryLogError(args...)     LogError(g_moduleEntryNO, g_entryLogLevel, args)
#define entryLogWarning(args...)   LogWarning(g_moduleEntryNO, g_entryLogLevel, args)
#define entryLogInfo(args...)      LogInfo(g_moduleEntryNO, g_entryLogLevel, args)
#define entryLogVerbose(args...)   LogVerbose(g_moduleEntryNO, g_entryLogLevel, args)
#define entryLogDebug(args...)     LogDebug(g_moduleEntryNO, g_entryLogLevel, args)





#endif  //__LOG_Entry_H__
