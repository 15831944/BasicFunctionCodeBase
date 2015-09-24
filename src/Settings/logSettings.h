#ifndef __LOG_Settings_H__
#define __LOG_Settings_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Settings       "settings"
#define ModuleVersion_Settings    g_make_svn_version //"4343"  //
#define ModuleLogType_Settings    LogType_SECURITY

/** 
* g_moduleSettingsNO： 记录当前模块在模块列表中的位置
* g_settingsLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleSettingsNO;
static int g_settingsLogLevel = LOG_LEVEL_Debug;

#define settingsLogFatal(args...)     LogFatal(g_moduleSettingsNO, g_settingsLogLevel, args)
#define settingsLogError(args...)     LogError(g_moduleSettingsNO, g_settingsLogLevel, args)
#define settingsLogWarning(args...)   LogWarning(g_moduleSettingsNO, g_settingsLogLevel, args)
#define settingsLogInfo(args...)      LogInfo(g_moduleSettingsNO, g_settingsLogLevel, args)
#define settingsLogVerbose(args...)   LogVerbose(g_moduleSettingsNO, g_settingsLogLevel, args)
#define settingsLogDebug(args...)     LogDebug(g_moduleSettingsNO, g_settingsLogLevel, args)


#endif

