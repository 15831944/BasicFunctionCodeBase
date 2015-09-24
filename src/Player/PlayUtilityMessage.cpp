/**
 *  PlayCommandUtility.cpp文件
 *  负责初始化Play播放相关的结构体，主要是解码器跟上层控制相关的结构；
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "log/LogC.h"
#include "LogPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"

#include "PlayUtilityMessageType.h"
#include "PlayUtilityInit.h"
#ifdef __cplusplus
}
#endif

#include "PlayUtilityDataStructures.h"
#include "PlayUtilityMessage.h"



/**
 *  拼装命令，向解码循环发送控制消息
 *
 **/
int PlayCommandToSending(MessageQueue_Type messageQueue, int cmd, int subCMD, int arg1, int arg2, unsigned int microSecond )
{
    PlayerCommand_Type playCmd;
    int ret;

    if(cmd < PLAY_CMD_RESERVED || cmd > PLAY_CMD_MAX) {
        playerLogError("PlayCommandToSending: cmd[%d]\n", cmd);
        return -1;
    }
    playCmd.cmd = cmd;
    playCmd.subCMD = subCMD;
    playCmd.arg1 = arg1;
    playCmd.arg2 = arg2;
    playerLogInfo("playCmd.cmd[%d], subCMD[%d], arg1[%d], arg2[%d], microSecond[%d] \n", cmd, subCMD, arg1, arg2, microSecond);

    ret = MessageQueueEnqueue(messageQueue, (char *)&playCmd, microSecond );
    playerLogInfo(" ret = [%d]\n", ret);

    return 0;
}



int SendPlayMessageToHandler(int playMsg, int playerIndex)
{
    playerLogInfo(" playMsg = [%d]\n", playMsg);
    
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue =  streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, playMsg, 0, 0, 0, microSecond);

    
    return 0;
}














