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

#include "PlayUtilityMessageType.h"

#ifdef __cplusplus
};
#endif

#include "PlayListUtilityInterfaces.h"
#include "PlayListUtility.h"
#include "PlayUtilityMessage.h"




int PlayListUtilityPlayFirst(int playerIndex)
{
    playerLogInfo("PlayListUtilityPlayFirst, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayLast(int playerIndex)
{
    playerLogInfo("PlayListUtilityPlayLast, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayPrevious(int playerIndex)
{
    playerLogInfo("PlayListUtilityPlayPrevious, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayNext(int playerIndex)
{
    playerLogInfo("PlayListUtilityPlayNext, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}




/////// 
int PlayListUtilityPlayRepeatList(int playerIndex)    //循环播放 all repeat
{
    playerLogInfo("PlayListUtilityPlayRepeatList, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayRepeatItem(int playerIndex)   //单曲循环 repeat once
{
    playerLogInfo("PlayListUtilityPlayRepeatItem, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayShuffleByList(int playerIndex)   //随机播放 shuffle
{
    playerLogInfo("PlayListUtilityPlayShuffleByList, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayOnceByList(int playerIndex)    //顺序播放 order     
{
    playerLogInfo("PlayListUtilityPlayOnceByList, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}

 
int PlayListUtilityPlayOnceByItem(int playerIndex)     //单曲播放 once
{
    playerLogInfo("PlayListUtilityPlayOnceByItem, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}




///// 
int PlayListUtilityPlayListByIndex(int playerIndex)    //须记录上次的播放文件
{
    playerLogInfo("PlayListUtilityPlayListByIndex, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilityPlayFileByOffset(int playerIndex)   //按照上次退出时的播放位置开始播放
{
    playerLogInfo("PlayListUtilityPlayFileByOffset, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}




////sort
int PlayListUtilitySortByFileName(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortByFileName, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilitySortByFilePath(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortByFilePath, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilitySortByFileType(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortByFileType, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilitySortByFileSize(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortByFileSize, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilitySortByPlayDuration(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortByPlayDuration, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int PlayListUtilitySortBySinger(int playerIndex)
{
    playerLogInfo("PlayListUtilitySortBySinger, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}























