/**
 *  PlayCodec4UpperInterfaces.c文件
 *  主要有以下功能：
 *          codec提供接口供上层调用
 *
 **/


#include <stdio.h>

#include "log/LogC.h"
#include "LogPlayer.h"
#include "Player.h"
#include "InitPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"

#include "../PlayUtilityMessageType.h"
#include "../AudioCtrl/AudioUtilityInterfaces.h"
#include "CodecUtilityInterfaces.h"
#include "CodecUtilityMessageType.h"

#include "SDL2/SDL.h"





int SendMessageToCodec(int msgNo)
{
    if(msgNo < SOFTWARE_CODEC_EVENT_RESERVED || msgNo > SOFTWARE_CODEC_EVENT_MAX) {
        playerLogError("SendMessageToCodec: msgNo[%d]\n", msgNo);
        return -1;
    }
    
    //TODO:: create SDL Event
    SDL_Event event;
    event.type = msgNo;
    SDL_Delay(33);      //33ms
    SDL_PushEvent(&event);
    
	return 0;
}


/**
 *  播放控制与解码器的接口
 *
 **/

//================================================================================================
//
//================================================================================================
int PlayerOptionsOpen()
{
    playerLogVerbose("PlayerOptionsOpen\n");
    
    //TODO:: 先设置打开谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_OPEN);
    
    return 0;
}


int PlayerOptionsPlay()
{
    playerLogVerbose("PlayerOptionsPlay\n");
    
    //TODO:: 先设置播放谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_PLAY);
    
    return 0;
}


int PlayerOptionsClose()
{
    playerLogVerbose("PlayerOptionsClose\n");
    
    //TODO:: 先设置关闭谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_CLOSE);
    
    return 0;
}


int PlayerOptionsStop()
{
    playerLogVerbose("PlayerOptionsStop\n");
    
    //TODO:: 先设置停止谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_STOP);
    
    return 0;
}


int PlayerOptionsPause()
{
    playerLogVerbose("PlayerOptionsPause\n");
    
    //TODO:: 先设置暂停谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_PAUSE);

    return 0;
}


int PlayerOptionsResume()
{
    playerLogVerbose("PlayerOptionsResume\n");
    
    //TODO:: 先设置恢复谁，保存在全局变量里，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_RESUME);

    return 0;
}


int PlayerOptionsSeekto()
{
    playerLogVerbose("PlayerOptionsSeekto\n");
    
    //TODO:: 先设置seek谁，保存在全局变量里，seek多少，seek倍速，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_SEEKTO);

    return 0;
}


int PlayerOptionsSeektoStart()
{
    playerLogVerbose("PlayerOptionsSeektoStart\n");
    
    //TODO:: 先设置seek谁，保存在全局变量里，seek多少，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_SEEKTO);

    return 0;
}


int PlayerOptionsSeektoEnd()
{
    playerLogVerbose("PlayerOptionsSeektoEnd\n");
    
    //TODO:: 先设置seek谁，保存在全局变量里，seek多少，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_SEEKTO);

    return 0;
}


int PlayerOptionsFastForward()
{
    playerLogVerbose("PlayerOptionsFastForward\n");
    
    //TODO:: 先设置seek谁，保存在全局变量里，seek倍速，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_SEEKTO);

    return 0;
}


int PlayerOptionsFastRewind()
{
    playerLogVerbose("PlayerOptionsFastRewind\n");
    
    //TODO:: 先设置seek谁，保存在全局变量里，seek倍速，然后发送消息
    SendMessageToCodec(SOFTWARE_CODEC_EVENT_SEEKTO);

    return 0;
}



//=================================================================================================
//=================================================================================================


//=================================================================================================
// 播放字幕
//=================================================================================================
int CodecSubtitleNumberGet(int *subtitleNum)
{
    int tmp = 5;

    //TODO::  get subtitleNum
    *subtitleNum = tmp;
    playerLogInfo("CodecUtilityGetAllSubtitleInfo subtitleNum[%d]\n", *subtitleNum);
    return *subtitleNum;
}


int CodecSubtitlePidGet(int index, int *subtitlePid)
{
    playerLogInfo("CodecUtilityGetAllSubtitleInfo index[%d]\n", index);

    switch (index) {
    case CODEC_MODE_NORMAL:
        //TODO::  get subtitlePid
        *subtitlePid = 1;
        break;
    case CODEC_MODE_ZEBRA:
        //TODO::  get subtitlePid
        *subtitlePid = 2;
        break;
    case CODEC_MODE_NANO:
        //TODO::  get subtitlePid
        *subtitlePid = 3;
        break;
    case CODEC_MODE_JUDGE:
        //TODO::  get subtitlePid
        *subtitlePid = 4;
        break;
    default:
        break;
    }

    return 0;
}



int CodecSubtitleLangGet(int index, char *languageCode)
{
    playerLogInfo("CodecUtilityGetAllSubtitleInfo index[%d]\n", index);

    switch (index) {
    case CODEC_MODE_NORMAL:
        //TODO::  get languageCode
        sprintf(languageCode, "English");
        break;
    case CODEC_MODE_ZEBRA:
        //TODO::  get languageCode
        sprintf(languageCode, "Chinese");
        break;
    case CODEC_MODE_NANO:
        //TODO::  get languageCode
        sprintf(languageCode, "Japanese");
        break;
    case CODEC_MODE_JUDGE:
        //TODO::  get languageCode
        sprintf(languageCode, "Zulu");
        break;
    default:
        break;
    }

    return 0;
}
int CodecSubtitleLangFullNameGet(int index, char *languageName)
{
    playerLogInfo("CodecSubtitleLangFullNameGet index[%d]\n", index);


    return 0;

}


int CodecUtilityGetAllSubtitleInfo(int playerIndex)
{
    playerLogInfo("CodecUtilityGetAllSubtitleInfo playerIndex[%d]\n", playerIndex);


    return 0;
}


int CodecUtilityGetCurrentSubtitleInfo(int playerIndex)
{
    playerLogInfo("CodecUtilityGetCurrentSubtitleInfo playerIndex[%d]\n", playerIndex);

    return 0;
}


int CodecUtilitySelectSubtitle(int playerIndex)
{
    playerLogInfo("CodecUtilitySelectSubtitle playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int CodecUtilitySwitchSubtitle(int playerIndex)
{
    playerLogInfo("CodecUtilitySwitchSubtitle playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int CodecUtilityGetSubtitileFlag(int playerIndex)
{
    playerLogInfo("CodecUtilityGetSubtitileFlag playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int CodecUtilitySetSubtitileFlag(int playerIndex)
{
    playerLogInfo("CodecUtilitySetSubtitileFlag playerIndex[%d]\n", playerIndex);

    
    return 0;
}

















