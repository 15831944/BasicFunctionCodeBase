/**
 *  PlayUtility4LowerInterfaces.c文件
 *  主要有以下功能：
 *          提供接口供codec的调用接口
 *
 **/

#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "log/LogC.h"
#include "LogPlayer.h"
#include "ThreadMutex.h"
#include "MessageQueue.h"

#include "AudioCtrl/AudioUtilityInterfaces.h"
#include "Codecs/CodecUtilityInterfaces.h"
#include "PlayUtilityHandler.h"
#include "PlayUtilityInit.h"


static ThreadMutex_Type     gPlayCtrlMutex = NULL;
static PlayCtrl_type       *gPlayerCtrl = NULL;

static ThreadMutex_Type     gStreamCtrlMutex = NULL;
static StreamCtrl_Type     *gStreamCtrl = NULL;

static MessageQueue_Type gMsgQueue = NULL;


/**
 *  name::  PlayCtrlInit
 *          初始化PlayCtrl控制结构体（for PlayUtility4UpperInterfaces.cpp）
 **/
static int PlayCtrlInit(void)
{
    PlayCtrl_type *playCtrl;

    playerLogDebug("PlayCtrlInit.\n");


    //TODO:: init play structure gPlayerCtrl
    playCtrl = (PlayCtrl_type *)malloc(sizeof(PlayCtrl_type));
    if (playCtrl == NULL)
        playerLogError("playCtrl malloc error.\n");
    memset(playCtrl, 0, sizeof(PlayCtrl_type));

    if (ThreadMutexLock(gPlayCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");
    gPlayerCtrl = playCtrl;
    playerLogVerbose("gPlayerCtrl address[%p]\n", gPlayerCtrl);
    if (ThreadMutexUnLock(gPlayCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");

    playerLogDebug("PlayCtrlInit END.\n");
    return 0;
}

/**
 *  name::  StreamCtrlInit
 *          初始化StreamCtrl控制结构体（for PlayUtilityHandler.c）
 **/
static int StreamCtrlInit(void)
{
    StreamCtrl_Type *streamCtrl;

    playerLogDebug("StreamCtrlInit.\n");

    //TODO:: init message queue
    gMsgQueue = MessageQueueCreate(sizeof(PlayerCommand_Type));

    //TODO:: init gStreamCtrl && gPlayerCtrl
    streamCtrl = (StreamCtrl_Type *)malloc(sizeof(StreamCtrl_Type));
    if (streamCtrl == NULL)
        playerLogError("streamCtrl malloc error.\n");
    memset(streamCtrl, 0, sizeof(StreamCtrl_Type));
    streamCtrl->msgQueue = gMsgQueue;

    if (ThreadMutexLock(gStreamCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");
    gStreamCtrl = streamCtrl;
    playerLogInfo("gStreamCtrl[%p],streamCtrl->msgQueue[%p]\n", gStreamCtrl, streamCtrl->msgQueue);
    if (ThreadMutexUnLock(gStreamCtrlMutex))
        playerLogWarning("thread mutex locking error.\n");

    return 0;
}


// Two Thread functions
extern int StreamCtrlThreadCreate(StreamCtrl_Type *streamCtrl);

/**
 *  name:: InitStreamPlay
 *          线程：  用于基本的控制命令的处理；
 *
 **/
int InitStreamPlay()
{
    playerLogDebug("InitStreamPlay.\n");
    
    gPlayCtrlMutex = ThreadMutexCreate();
    gStreamCtrlMutex = ThreadMutexCreate();
    PlayCtrlInit();
    StreamCtrlInit();
    StreamCtrlThreadCreate(gStreamCtrl);
    
    return 0;
}




//===========================================================================================================================
//===========================================================================================================================



/**
 *  name::  StreamDispatcher
 *          获取到所需的StreamCtrl结构体,
 **/
StreamCtrl_Type *StreamDispatcher(int index)
{
    StreamCtrl_Type *streamCtrl;

    playerLogDebug("StreamDispatcher. index[%d]\n", index);

    switch (index) {
    case 0:
        //获取到所需的stream结构体
        streamCtrl = gStreamCtrl; // gPlayer->streamCtrl;
        playerLogVerbose("Get streamCtrl from gStreamCtrl, address[%p]\n", streamCtrl);
        break;
    case 1:
        playerLogVerbose(" 1111111111 \n");
        break;
    default:
        playerLogError("StreamDispatcher index[%d] is error.\n", index);
        break;
    }

    playerLogDebug("Index[%d] get streamCtrl[%p] \n", index, streamCtrl);
    return streamCtrl;
}




















