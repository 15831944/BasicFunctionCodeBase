#include <stdio.h>

#include "Log/LogC.h"
#include "logXmppIM.h"
#include "XmppIM.h"


int g_moduleXmppIMNO;


/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 *
 **/
int log_xmpp_im_init()
{

    //register to  module list
    g_moduleXmppIMNO = registerModule(ModuleName_XmppIM, ModuleVersion_XmppIM, ModuleLogType_XmppIM);

    printf("=================log_xmpp_im_init=========================\n");
#if 1
    xmpp_imLogFatal("=====xmpp_imLogFatal==[%d]=--------------=\n", g_moduleXmppIMNO);
    xmpp_imLogError("=====xmpp_imLogError==[%d]=--------------=\n", g_moduleXmppIMNO);
    xmpp_imLogWarning("=====xmpp_imLogWarning==[%d]=--------------=\n", g_moduleXmppIMNO);
    xmpp_imLogInfo("=====xmpp_imLogInfo==[%d]=--------------=\n", g_moduleXmppIMNO);
    xmpp_imLogVerbose("=====xmpp_imLogVerbose==[%d]=--------------=\n", g_moduleXmppIMNO);
    xmpp_imLogDebug("=====xmpp_imLogDebug==[%d]=--------------=\n", g_moduleXmppIMNO);
#endif
    return 0;
}

