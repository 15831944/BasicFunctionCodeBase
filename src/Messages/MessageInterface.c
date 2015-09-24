/**
 *  MessageInterface.c文件
 *  负责初始化Play播放相关的结构体，主要是解码器跟上层控制相关的结构；
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "Log/LogC.h"
#include "logMessage.h"
#include "Message.h"

#include "MessageInterface.h"

 



static int MessageBuildTime()
{
    messageLogInfo("Message Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



int InitMessage(void)
{
    log_message_init();
    MessageBuildTime();
    
    
    messageLogDebug("InitMessage.\n");
    return 0;
}









