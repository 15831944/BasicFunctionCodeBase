/**
 *  AudioUtilityInterfaces.h.c文件
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
#include "AudioUtilityInterfaces.h"

#include "SDL2/SDL.h"






//=================================================================================================
// 播放器音量调节
//=================================================================================================
int AudioUtilityGetVolume(int playerIndex)
{
    playerLogInfo("AudioUtilityGetVolume playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilitySetVolume(int playerIndex)
{
    playerLogInfo("AudioUtilitySetVolume playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilitySetMute(int playerIndex)
{
    playerLogInfo("AudioUtilitySetMute playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetMute(int playerIndex)
{
    playerLogInfo("AudioUtilityGetMute playerIndex[%d]\n", playerIndex);

    
    return 0;
}



//=================================================================================================
// 声道调节
//=================================================================================================
int AudioUtilitySetChannel(int playerIndex)
{
    playerLogInfo("AudioUtilitySetChannel playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetChannel(int playerIndex)
{
    playerLogInfo("AudioUtilityGetChannel playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetCurrentAudioChannel(int playerIndex)
{
    playerLogInfo("AudioUtilityGetCurrentAudioChannel playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilitySwitchAudioChannel(int playerIndex)
{
    playerLogInfo("AudioUtilitySwitchAudioChannel playerIndex[%d]\n", playerIndex);

    
    return 0;
}



//=================================================================================================
// 音轨调节
//=================================================================================================
int AudioUtilitySetTrack(int playerIndex)
{
    playerLogInfo("AudioUtilitySetTrack playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetTrack(int playerIndex)
{
    playerLogInfo("AudioUtilityGetTrack playerIndex[%d]\n", playerIndex);

    
    return 0;
}
int AudioUtilityGetTrackInfo(int playerIndex)
{
    playerLogInfo("AudioUtilityGetTrackInfo playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetAllAudioTrackInfo(int playerIndex)
{
    playerLogInfo("AudioUtilityGetAllAudioTrackInfo playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilityGetCurrentAudioTrackInfo(int playerIndex)
{
    playerLogInfo("AudioUtilityGetCurrentAudioTrackInfo playerIndex[%d]\n", playerIndex);

    
    return 0;
}

int AudioUtilitySelectAudioTrack(int playerIndex)
{
    playerLogInfo("AudioUtilitySelectAudioTrack playerIndex[%d]\n", playerIndex);

    
    return 0;
}


int AudioUtilitySwitchAudioTrack(int playerIndex)
{
    playerLogInfo("AudioUtilitySwitchAudioTrack playerIndex[%d]\n", playerIndex);

    
    return 0;
}















