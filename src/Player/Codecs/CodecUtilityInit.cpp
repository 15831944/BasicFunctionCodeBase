

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log/LogC.h"
#include "../LogPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"
#include "ThreadTask.h"

#include "PlayUtilityDataStructures.h"
#include "SoftwareCodec.h"
#include "CodecUtilityInit.h"

#ifdef __cplusplus
};
#endif



#define SOFTWARE_CODEC_FLAG         1
#define SOFTWARE_CODEC_FLAG         1


//解码时的结构体codecCtrl    主要跟Codecs功能类对应，并由其初始化，是Codecs功能类与PlayCodec的接口类型
static ThreadMutex_Type     gCodecCtrlMutex = NULL;
static CodecCtrl_type       *gCodecCtrl = NULL;

static int gArgc;
static char** gArgv = NULL;


static int SoftwareCodecTask(void *arg)
{
    playerLogDebug("SoftwareCodecTask::[%s]\n", arg);
    
    SoftwareCodecInit(gArgc, gArgv, gCodecCtrl);
    return 0;
}


static int SoftwareCodecThreadCreate()
{
    playerLogDebug("SoftwareCodecThreadCreate\n");

    ThreadTaskCreateAdvanced("SoftwareCodecTask", SoftwareCodecTask, (void *)"SoftwareCodecTask");
    sleep(8);
    playerLogDebug("SoftwareCodecThreadCreate ==sleep(8)==end==========\n");

    return 0;
}


static int InitCodecsCtrl()
{
    CodecCtrl_type *codecCtrl;
    
    
    codecCtrl = (CodecCtrl_type *)malloc(sizeof(CodecCtrl_type));
    if (codecCtrl == NULL)
        playerLogError("codecCtrl malloc error.\n");
    memset(codecCtrl, 0, sizeof(CodecCtrl_type));
    
    ///initial gCodecCtrl structure
    
    if (ThreadMutexLock(gCodecCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");
    gCodecCtrl = codecCtrl;
    playerLogVerbose("gCodecCtrl address[%p]\n", gCodecCtrl);
    if (ThreadMutexUnLock(gCodecCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");
    
    return 0;
}


/**
 *  name::  InitPlayCodecs
 *          初始化各个解码器, 主要是初始化ffmpeg解码器
 *
 **/
int InitPlayCodecs(int argc, char *argv[])
{
    playerLogDebug("InitPlayCodecs.\n");
    
    gCodecCtrlMutex = ThreadMutexCreate();
    InitCodecsCtrl();
    
    gArgc = argc;
    gArgv = argv;
    SoftwareCodecThreadCreate();
    
    playerLogDebug("InitPlayCodecs END. \n");
    return 0;
}














