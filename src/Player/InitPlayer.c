/**
 *  InitPlayer.c文件
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
#include "LogPlayer.h"
#include "Player.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"

#include "PlayUtilityMessageType.h"
#include "InitPlayer.h"
#include "AudioCtrl/AudioUtilityInterfaces.h"
#include "Codecs/CodecUtilityInterfaces.h"
#include "PlayUtilityInit.h"
#include "PlayListUtilityInit.h"
#include "PlayUtilityDataStructures.h"

#include "Codecs/CodecUtilityInit.h"

extern int InitStreamPlay();
extern int InitPlayCodecs(int argc, char *argv[]);

 

static int PlayerBuildTime()
{
    playerLogInfo("Message Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



/**
 *  //三个结构体：
 *      播放控制的结构体playCtrl   跟BasicUtility功能类对应，并由其初始化；并包含streamCtrl结构
 *  
 *      播放时的结构体streamCtrl   主要是跟PlayUtility对应，并由其初始化，主要负责消息传递和根据媒体类型选择合适的控制播放类型
 *  
 *      解码时的结构体codecCtrl    主要跟Codecs功能类对应，并由其初始化，是Codecs功能类与PlayCodec的接口类型
 *  
 **/


/**
 *  name::  InitPlayer
 *          播放部分的入口；
 *          初始化基本的数据结构，创建线程，陷入循环，等待接收命令；
 *
 **/
int InitPlayer(int argc, char *argv[])
{
    log_player_init();
    
    playerLogDebug("Player Init.\n");
    PlayerBuildTime();

    //TODO:: 初始化解码器，初始化控制
    //TODO:: 另一个是循环播放线程；
    InitPlayCodecs(argc, argv);

    //TODO:: 初始化PlayList
    InitPlayList(argc, argv);
    
    
    //TODO: 线程：用于基本的控制命令的处理；
    InitStreamPlay();


    playerLogDebug("Player Init END.\n");
    return 0;
}

























