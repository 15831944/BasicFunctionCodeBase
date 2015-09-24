/**
 *  NetworkInterface.c文件
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
#include "logNetwork.h"
#include "Network.h"

#include "NetworkInterface.h"



static int NetworkBuildTime()
{
    networkLogInfo("Network Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



int InitNetwork(void)
{
    log_network_init();
    NetworkBuildTime();
    
    
    networkLogDebug("InitNetwork.\n");
    return 0;
}









