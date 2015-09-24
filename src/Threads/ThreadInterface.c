/**
 *  ThreadInterface.c文件
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

#include "log/LogC.h"
#include "LogThreads.h"
#include "Threads.h"

#include "ThreadTask.h"
#include "ThreadInterface.h"

 


static int ThreadBuildTime()
{
    threadsLogInfo("Thread Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}


extern void ThreadTaskInit(void);
int InitThread(void)
{
    log_threads_init();
    ThreadBuildTime();
    
    ThreadTaskInit();
    
    
    threadsLogDebug("InitThread.\n");
    return 0;
}









