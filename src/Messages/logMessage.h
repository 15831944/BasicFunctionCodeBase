#ifndef __LOG_Message_H__
#define __LOG_Message_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Message       "message"
#define ModuleVersion_Message    g_make_svn_version //"4343"  //
#define ModuleLogType_Message    LogType_SECURITY

/** 
* g_moduleMessageNO： 记录当前模块在模块列表中的位置
* g_messageLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleMessageNO;
static int g_messageLogLevel = LOG_LEVEL_Warning;

#define messageLogFatal(args...)     LogFatal(g_moduleMessageNO, g_messageLogLevel, args)
#define messageLogError(args...)     LogError(g_moduleMessageNO, g_messageLogLevel, args)
#define messageLogWarning(args...)   LogWarning(g_moduleMessageNO, g_messageLogLevel, args)
#define messageLogInfo(args...)      LogInfo(g_moduleMessageNO, g_messageLogLevel, args)
#define messageLogVerbose(args...)   LogVerbose(g_moduleMessageNO, g_messageLogLevel, args)
#define messageLogDebug(args...)     LogDebug(g_moduleMessageNO, g_messageLogLevel, args)


#endif

