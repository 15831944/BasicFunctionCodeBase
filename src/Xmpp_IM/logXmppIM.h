#ifndef __LOG_XMPP_IM_H__
#define __LOG_XMPP_IM_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_XmppIM       "xmpp_im"
#define ModuleVersion_XmppIM    g_make_svn_version //"4343"  //
#define ModuleLogType_XmppIM    LogType_SECURITY

/** 
* g_moduleXmppIMNO： 记录当前模块在模块列表中的位置
* g_xmpp_imLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleXmppIMNO;
static int g_xmpp_imLogLevel = LOG_LEVEL_Debug;

#define xmpp_imLogFatal(args...)     LogFatal(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)
#define xmpp_imLogError(args...)     LogError(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)
#define xmpp_imLogWarning(args...)   LogWarning(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)
#define xmpp_imLogInfo(args...)      LogInfo(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)
#define xmpp_imLogVerbose(args...)   LogVerbose(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)
#define xmpp_imLogDebug(args...)     LogDebug(g_moduleXmppIMNO, g_xmpp_imLogLevel, args)


#endif   //__LOG_XMPP_IM_H__

