#ifndef __LOG_MEDIA_INFO_H__
#define __LOG_MEDIA_INFO_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_MediaInfo       "media_info"
#define ModuleVersion_MediaInfo    g_make_svn_version //"4343"  //
#define ModuleLogType_MediaInfo    LogType_SECURITY

/** 
* g_moduleMediaInfoNO： 记录当前模块在模块列表中的位置
* g_media_infoLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleMediaInfoNO;
static int g_media_infoLogLevel = LOG_LEVEL_Debug;

#define media_infoLogFatal(args...)     LogFatal(g_moduleMediaInfoNO, g_media_infoLogLevel, args)
#define media_infoLogError(args...)     LogError(g_moduleMediaInfoNO, g_media_infoLogLevel, args)
#define media_infoLogWarning(args...)   LogWarning(g_moduleMediaInfoNO, g_media_infoLogLevel, args)
#define media_infoLogInfo(args...)      LogInfo(g_moduleMediaInfoNO, g_media_infoLogLevel, args)
#define media_infoLogVerbose(args...)   LogVerbose(g_moduleMediaInfoNO, g_media_infoLogLevel, args)
#define media_infoLogDebug(args...)     LogDebug(g_moduleMediaInfoNO, g_media_infoLogLevel, args)


#endif   //__LOG_MEDIA_INFO_H__

