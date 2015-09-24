/**
 *  PlayUtility4UpperInterfaces.c文件
 *  主要有以下功能：
 *          提供接口供BasicPlayer.c文件调用
 *
 **/

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json/json.h>



#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include "log/LogC.h"
#include "LogPlayer.h"
#include "InitPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"

#include "PlayUtilityInit.h"
#include "PlayUtilityMessageType.h"
#include "AudioCtrl/AudioUtilityInterfaces.h"
#include "Codecs/CodecUtilityInterfaces.h"

#ifdef __cplusplus
}
#endif

#include "PlayUtility4UpperInterfaces.h"
#include "PlayUtilityDataStructures.h"
#include "PlayUtilityMessage.h"
#include "PlayListUtility.h"


using namespace std;


//=================================================================================================
// 基本播放控制
//=================================================================================================

int StreamPlayerUtilityCtrlOpen(int playerIndex)
{
#if 0
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;

    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue =  streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_CMD_OPEN, 0, 0, 0, microSecond);
#else
    SendPlayMessageToHandler(PLAY_CMD_OPEN, playerIndex);
#endif
    return 0;
}


int StreamPlayerUtilityCtrlPlay(int playerIndex)
{
    
    SendPlayMessageToHandler(PLAY_CMD_PLAY, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlClose(int playerIndex)
{
    
    SendPlayMessageToHandler(PLAY_CMD_CLOSE, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlStop(int playerIndex)
{
    
    SendPlayMessageToHandler(PLAY_CMD_STOP, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlPause(int playerIndex)
{
    
    SendPlayMessageToHandler(PLAY_CMD_PAUSE, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlResume(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_RESUME, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlSeekto(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_SEEKTO, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlSeektoStart(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_SEEKTO_START, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlSeektoEnd(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_SEEKTO_END, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlFastForward(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_FASTFORWARD, playerIndex);
    
    return 0;
}


int StreamPlayerUtilityCtrlFastRewind(int playerIndex)
{

    SendPlayMessageToHandler(PLAY_CMD_FASTREWIND, playerIndex);
    
    return 0;
}





//=================================================================================================
//  播放方式
//=================================================================================================

int StreamPlayerUtilityModePlayFirst(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayFirst playerIndex[%d]\n", playerIndex);
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    streamCtrl = StreamDispatcher(playerIndex);

    //TODO:: GetPlayListFirst
    std::string fileUrl;
    GetPlayListFirst(fileUrl);
    
    //TODO:: modify message parameter
    (streamCtrl->playArgs).PlayUrl = (char *)malloc(sizeof(char) * fileUrl.length());
    sprintf((streamCtrl->playArgs).PlayUrl, fileUrl.c_str());

    //TODO:: send message
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);
    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYFIRST, 0, 0, 0, microSecond);

    return 0;
}


int StreamPlayerUtilityModePlayLast(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayLast playerIndex[%d]\n", playerIndex);
        
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    streamCtrl = StreamDispatcher(playerIndex);

    //TODO:: GetPlayListLast
    std::string fileUrl;
    GetPlayListLast(fileUrl);
    
    //TODO:: modify message parameter
    (streamCtrl->playArgs).PlayUrl = (char *)malloc(sizeof(char) * fileUrl.length());
    sprintf((streamCtrl->playArgs).PlayUrl, fileUrl.c_str());

    //TODO:: send message    
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);
    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYLAST, 0, 0, 0, microSecond);
    
    return 0;
}


int StreamPlayerUtilityModePlayNext(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayNext playerIndex[%d]\n", playerIndex);
        
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    streamCtrl = StreamDispatcher(playerIndex);

    //TODO:: GetPlayNext
    std::string fileUrl;
    std::string currentPlayFileName;
    GetPlayListCurrent(currentPlayFileName);
    GetPlayListNext(currentPlayFileName, fileUrl);
    
    //TODO:: modify message parameter
    (streamCtrl->playArgs).PlayUrl = (char *)malloc(sizeof(char) * fileUrl.length());
    sprintf((streamCtrl->playArgs).PlayUrl, fileUrl.c_str());

    //TODO:: send message    
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);
    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYNEXT, 0, 0, 0, microSecond);
    
    return 0;
}


int StreamPlayerUtilityModePlayPrevious(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayPrevious playerIndex[%d]\n", playerIndex);
        
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    streamCtrl = StreamDispatcher(playerIndex);

    //TODO:: GetPlayPrevious
    std::string fileUrl;
    std::string currentPlayFileName;
    GetPlayListCurrent(currentPlayFileName);
    GetPlayListPrevious(currentPlayFileName, fileUrl);
    
    //TODO:: modify message parameter
    (streamCtrl->playArgs).PlayUrl = (char *)malloc(sizeof(char) * fileUrl.length());
    sprintf((streamCtrl->playArgs).PlayUrl, fileUrl.c_str());

    //TODO:: send message    
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);
    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYPREVIOUS, 0, 0, 0, microSecond);
    
    return 0;
}


int StreamPlayerUtilityModePlayRepeatList(int playerIndex)    //循环播放 all repeat
{
    playerLogInfo("StreamPlayerUtilityModePlayRepeatList playerIndex[%d]\n", playerIndex);
    
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
   
    //TODO:: Set PlayRepeatList flag
    streamCtrl->isPlayRepeatList = 1;
    streamCtrl->isPlayRepeatItem = 0;
    
    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYREPEAT_LIST, 0, 0, 0, microSecond);
    return 0;
}


int StreamPlayerUtilityModePlayRepeatItem(int playerIndex)    //单曲循环 repeat once
{
    playerLogInfo("StreamPlayerUtilityModePlayRepeatItem playerIndex[%d]\n", playerIndex);
       
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
       
    //TODO:: Set PlayRepeatItem flag
    streamCtrl->isPlayRepeatList = 0;
    streamCtrl->isPlayRepeatItem = 1;
    
    //TODO:: modify message parameter
    std::string fileUrl;
    GetPlayRepeatItem(fileUrl);
    (streamCtrl->playArgs).PlayUrl = (char *)malloc(sizeof(char) * fileUrl.length());
    sprintf((streamCtrl->playArgs).PlayUrl, fileUrl.c_str());
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYREPEAT_ITEM, 0, 0, 0, microSecond);
    return 0;
}


int StreamPlayerUtilityModePlayShuffleByList(int playerIndex)   //随机播放 shuffle
{
    playerLogInfo("StreamPlayerUtilityModePlayShuffleByList playerIndex[%d]\n", playerIndex);
      
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type   *streamCtrl;
      
    //TODO:: GetPlayShuffleByList
    streamCtrl->isPlayRepeatList = 1;
    streamCtrl->isPlayRepeatItem = 0;
    
    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYSHUFFLE_BYLIST, 0, 0, 0, microSecond);
    return 0;
}


int StreamPlayerUtilityModePlayOnceByList(int playerIndex)      //顺序播放 order   
{
    playerLogInfo("StreamPlayerUtilityModePlayOnceByList playerIndex[%d]\n", playerIndex);
        
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;

    //TODO:: GetPlayOnceByList
    streamCtrl->isPlayRepeatList = 0;
    streamCtrl->isPlayRepeatItem = 0;

    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYONCE_BYLIST, 0, 0, 0, microSecond);
    return 0;
}

   
int StreamPlayerUtilityModePlayOnceByItem(int playerIndex)      //单曲播放 once
{
    playerLogInfo("StreamPlayerUtilityModePlayOnceByItem playerIndex[%d]\n", playerIndex);
        
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;

    //TODO:: GetPlayOnceByItem
    streamCtrl->isPlayRepeatList = 0;
    streamCtrl->isPlayRepeatItem = 0;

    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYONCE_BYITEM, 0, 0, 0, microSecond);
    return 0;
}



int StreamPlayerUtilityModePlayByListIndex(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayByListIndex playerIndex[%d]\n", playerIndex);
      
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
      
    //TODO:: GetPlayByListIndex
    
    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYBYLISTINDEX, 0, 0, 0, microSecond);
    return 0;
}


int StreamPlayerUtilityModePlayByFileOffset(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayByFileOffset playerIndex[%d]\n", playerIndex);
    
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
    
    //TODO:: GetPlayByFileOffset
    
    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYBYFILEOFFSET, 0, 0, 0, microSecond);
    return 0;
}


int StreamPlayerUtilityModePlayByEntryId(int playerIndex)
{
    playerLogInfo("StreamPlayerUtilityModePlayByEntryId playerIndex[%d]\n", playerIndex);
      
    int microSecond = PLAYER_CMD_SENDING_DELAY_TIME;
    MessageQueue_Type messageQueue;
    StreamCtrl_Type *streamCtrl;
      
    //TODO:: GetPlayByEntryId
    
    //TODO:: modify message parameter
    
    //TODO:: send message
    streamCtrl = StreamDispatcher(playerIndex);
    messageQueue = streamCtrl->msgQueue;    //GlobalMessageQueueGet(0);

    PlayCommandToSending( messageQueue, PLAY_MODE_PLAYBYENTRYID, 0, 0, 0, microSecond);
    return 0;
}







//=================================================================================================
// 播放器时间
//=================================================================================================
int StreamPlayerGetTotalTime(int playerIndex)
{
    playerLogInfo("StreamPlayerGetTotalTime playerIndex[%d]\n", playerIndex);

    //TODO:: get time info

    //TODO:: 拼装生成json串

    //TODO::

    return 0;
}


int StreamPlayerGetCurrentTime(int playerIndex)
{
    playerLogInfo("StreamPlayerGetCurrentTime playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetCurrentTimeString(int playerIndex)
{
    playerLogInfo("StreamPlayerGetCurrentTimeString playerIndex[%d]\n", playerIndex);

    return 0;
}






//=================================================================================================
// 播放字幕
//=================================================================================================
/**========================================================
{
   "SubtitleListCount" : 5,
   "SubtitleList" : [
      {
         "PID" : 34548,
         "LanguageCode" : "zh",
         "LangusgeNameEnglish" : "Chinese"
      }
   ]
}
========================================================**/
int StreamPlayerGetAllSubtitleInfo(int playerIndex)
{
    playerLogInfo("GetAllSubtitleInfo playerIndex[%d]\n", playerIndex);

    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;

    int subtitleNum;
    int subtitlePid;
    char languageCode[4] = {0};
    char languageName[32] = {0};
    int tIndex;


    //TODO:: get Subtitle Number;
    CodecSubtitleNumberGet(&subtitleNum);
    root["SubtitleListCount"] = subtitleNum;

    for ( tIndex = 0; tIndex < subtitleNum; tIndex++) {
        CodecSubtitlePidGet(tIndex, &subtitlePid);
        item["PID"] = subtitlePid;

        CodecSubtitleLangGet(tIndex, languageCode);
        item["LanguageCode"] = languageCode;

        CodecSubtitleLangFullNameGet(tIndex, languageName);
        item["LangusgeNameEnglish"] = languageName;

        arrayObj.append(item);

        root["SubtitleList"] = arrayObj;
    }
    root.toStyledString();
    std::string strValue = root.toStyledString();
    std::cout << strValue << std::endl;

    return 0;
}


int StreamPlayerGetCurrentSubtitleInfo(int playerIndex)
{
    playerLogInfo("StreamPlayerGetCurrentSubtitleInfo playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerSelectSubtitle(int playerIndex)
{
    playerLogInfo("StreamPlayerSelectSubtitle playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerSwitchSubtitle(int playerIndex)
{
    playerLogInfo("StreamPlayerSwitchSubtitle playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetSubtitileFlag(int playerIndex)
{
    playerLogInfo("StreamPlayerGetSubtitileFlag playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerSetSubtitileFlag(int playerIndex)
{
    playerLogInfo("StreamPlayerSetSubtitileFlag playerIndex[%d]\n", playerIndex);

    return 0;
}







//=================================================================================================
// 播放器音量调节
//=================================================================================================


int StreamPlayerGetVolume(int playerIndex)
{
    playerLogInfo("StreamPlayerGetVolume playerIndex[%d]\n", playerIndex);
    Json::Reader reader;
    Json::Value root;

    int volume;
    /**
    ifstream cfgFile ("player.cfg");
    if ( !cfgFile.is_open()) {
        cout << "Error opening file";
        exit (1);
    }
    while ( !cfgFile.eof()) {
        cfgFile.getline (buffer, 100);
        cout << buffer << endl;

    }



    ifstream ifs;
    ifs.open("testjson.json");
    assert(ifs.is_open());

    if (!reader.parse(ifs, root, false)) {
        playerLogError("reader parse file error.\n");
        return -1;
    }
    std::string name;
    int age;
    int size = root.size();
    for (int i=0; i<size; ++i)
    {
        name = root[i]["name"].asString();
        volume = root[i]["volume"].asInt();

        std::cout<<name<<" "<<age<<std::endl;
    }
    **/


    //TODO:: get volume;

    //CodecVolumeGet(&volume);
    root["volume"] = volume;
    root.toStyledString();
    std::string strValue = root.toStyledString();
    std::cout << strValue << std::endl;

    return 0;
}


int StreamPlayerSetVolume(int playerIndex)
{
    playerLogInfo("StreamPlayerSetVolume playerIndex[%d]\n", playerIndex);
    ofstream cfgFile ("player.cfg");

    if (cfgFile.is_open()) {
        cfgFile << "This is a line.\n";
        cfgFile << "This is another line.\n";
        cfgFile.close();
    }
    return 0;
}


int StreamPlayerSetMute(int playerIndex)
{
    playerLogInfo("StreamPlayerSetMute playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetMute(int playerIndex)
{
    playerLogInfo("StreamPlayerGetMute playerIndex[%d]\n", playerIndex);

    return 0;
}



//=================================================================================================
// 声道调节
//=================================================================================================
int StreamPlayerSetChannel(int playerIndex)
{
    playerLogInfo("StreamPlayerSetChannel playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetChannel(int playerIndex)
{
    playerLogInfo("StreamPlayerGetChannel playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetCurrentAudioChannel(int playerIndex)
{
    playerLogInfo("StreamPlayerGetCurrentAudioChannel playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerSwitchAudioChannel(int playerIndex)
{
    playerLogInfo("StreamPlayerSwitchAudioChannel playerIndex[%d]\n", playerIndex);

    return 0;
}



//=================================================================================================
// 音轨调节
//=================================================================================================
int StreamPlayerSetTrack(int playerIndex)
{
    playerLogInfo("StreamPlayerSetTrack playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetTrack(int playerIndex)
{
    playerLogInfo("StreamPlayerGetTrack playerIndex[%d]\n", playerIndex);

    return 0;
}
int StreamPlayerGetTrackInfo(int playerIndex)
{
    playerLogInfo("StreamPlayerGetTrackInfo playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetAllAudioTrackInfo(int playerIndex)
{
    playerLogInfo("StreamPlayerGetAllAudioTrackInfo playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerGetCurrentAudioTrackInfo(int playerIndex)
{
    playerLogInfo("StreamPlayerGetCurrentAudioTrackInfo playerIndex[%d]\n", playerIndex);

    return 0;
}

int StreamPlayerSelectAudioTrack(int playerIndex)
{
    playerLogInfo("StreamPlayerSelectAudioTrack playerIndex[%d]\n", playerIndex);

    return 0;
}


int StreamPlayerSwitchAudioTrack(int playerIndex)
{
    playerLogInfo("StreamPlayerSwitchAudioTrack playerIndex[%d]\n", playerIndex);

    return 0;
}





















