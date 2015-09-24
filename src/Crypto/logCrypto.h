#ifndef __LOG_Crypto_H__
#define __LOG_Crypto_H__

#include "Log/LogC.h"
#include "build_info.h"

#define ModuleName_Crypto       "crypto"
#define ModuleVersion_Crypto    g_make_svn_version //"4343"  //
#define ModuleLogType_Crypto    LogType_SECURITY

/** 
* g_moduleCryptoNO： 记录当前模块在模块列表中的位置
* g_cryptoLogLevel： 设置当前模块的log输出等级，每个模块都可设置独立的log输出等级 
**/
extern int g_moduleCryptoNO;
static int g_cryptoLogLevel = LOG_LEVEL_Debug;

#define cryptoLogFatal(args...)     LogFatal(g_moduleCryptoNO, g_cryptoLogLevel, args)
#define cryptoLogError(args...)     LogError(g_moduleCryptoNO, g_cryptoLogLevel, args)
#define cryptoLogWarning(args...)   LogWarning(g_moduleCryptoNO, g_cryptoLogLevel, args)
#define cryptoLogInfo(args...)      LogInfo(g_moduleCryptoNO, g_cryptoLogLevel, args)
#define cryptoLogVerbose(args...)   LogVerbose(g_moduleCryptoNO, g_cryptoLogLevel, args)
#define cryptoLogDebug(args...)     LogDebug(g_moduleCryptoNO, g_cryptoLogLevel, args)


#endif

