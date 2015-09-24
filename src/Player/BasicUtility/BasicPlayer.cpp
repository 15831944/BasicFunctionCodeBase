/**
 *  BasicPlayer.c文件
 *  主要有以下功能：
 *          播放器基本功能的抽象
 *          基本功能接口
 *          需要把各个接口获取到的数据封装（封装成json格式 或ini或xml文件）
 *
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include "LogC.h"
#include "LogPlayer.h"

#include "PlayUtilityMessageType.h"


#ifdef __cplusplus
}
#endif

#include "BasicPlayer.h"
#include "PlayUtility4UpperInterfaces.h"






namespace Player
{

using namespace std;

BasicPlayer :: BasicPlayer()
{


}



BasicPlayer :: ~BasicPlayer()
{


}




#define MaxPlayerNo  4
BasicPlayer *gPlayers[MaxPlayerNo] = { 0 };



//============================================================================================================


/** 基本的播放控制 **/
// 播放控制
int BasicPlayer :: Open()
{
    playerLogDebug("BasicPlayer :: Open\n");
    StreamPlayerUtilityCtrlOpen(0);   // 0 表示索引值


    return 0;
}


int BasicPlayer :: Close()
{
    playerLogDebug("BasicPlayer :: Close\n");
    StreamPlayerUtilityCtrlClose(0);

    return 0;
}


int BasicPlayer :: Play()
{
    playerLogDebug("BasicPlayer :: Play\n");
    StreamPlayerUtilityCtrlPlay(0);

    return 0;
}


int BasicPlayer :: Stop()
{
    playerLogDebug("BasicPlayer :: Stop\n");
    StreamPlayerUtilityCtrlStop(0);

    return 0;
}


int BasicPlayer :: Pause()
{
    playerLogDebug("BasicPlayer :: Pause\n");
    StreamPlayerUtilityCtrlPause(0);

    return 0;
}


int BasicPlayer :: Resume()
{
    playerLogDebug("BasicPlayer :: Resume\n");
    StreamPlayerUtilityCtrlResume(0);

    return 0;
}


int BasicPlayer :: FastForward(int)
{
    playerLogDebug("BasicPlayer :: FastForward\n");
    StreamPlayerUtilityCtrlFastForward(0);

    return 0;
}


int BasicPlayer :: FastRewind(int)
{
    playerLogDebug("BasicPlayer :: FastRewind\n");
    StreamPlayerUtilityCtrlFastRewind(0);

    return 0;
}


int BasicPlayer :: SeekTo()
{
    playerLogDebug("BasicPlayer :: SeekTo\n");
    StreamPlayerUtilityCtrlSeekto(0);

    return 0;
}


int BasicPlayer :: SeekToStart(void)
{
    playerLogDebug("BasicPlayer :: SeekToStart\n");
    StreamPlayerUtilityCtrlSeektoStart(0);

    return 0;
}


int BasicPlayer :: SeekEnd()
{
    playerLogDebug("BasicPlayer :: SeekEnd\n");
    StreamPlayerUtilityCtrlSeektoEnd(0);

    return 0;
}





//============================================================================================================

// 播放方式
int BasicPlayer :: PlayFirst(void)
{
    playerLogDebug("BasicPlayer :: PlayFirst\n");
    StreamPlayerUtilityModePlayFirst(0);

    return 0;
}


int BasicPlayer :: PlayLast(void)
{
    playerLogDebug("BasicPlayer :: PlayLast\n");
    StreamPlayerUtilityModePlayLast(0);

    return 0;
}


int BasicPlayer :: PlayNext(void)
{
    playerLogDebug("BasicPlayer :: PlayNext\n");
    StreamPlayerUtilityModePlayNext(0);

    return 0;
}


int BasicPlayer :: PlayPrevious(void)
{
    playerLogDebug("BasicPlayer :: PlayPrevious\n");
    StreamPlayerUtilityModePlayPrevious(0);

    return 0;
}


int BasicPlayer :: PlayRepeatList()    //循环播放 all repeat
{
    playerLogDebug("BasicPlayer :: PlayRepeatList\n");
    StreamPlayerUtilityModePlayRepeatList(0);
    
    return 0;
}

int BasicPlayer :: PlayRepeatItem()    //单曲循环 repeat once
{
    playerLogDebug("BasicPlayer :: PlayRepeatItem\n");
    StreamPlayerUtilityModePlayRepeatItem(0);
    
    return 0;
}

int BasicPlayer :: PlayShuffleByList()   //随机播放 shuffle
{
    playerLogDebug("BasicPlayer :: PlayShuffleByList\n");
    StreamPlayerUtilityModePlayShuffleByList(0);
    
    return 0;
}

int BasicPlayer :: PlayOnceByList()      //顺序播放 order    
{
    playerLogDebug("BasicPlayer :: PlayOnceByList\n");
    StreamPlayerUtilityModePlayOnceByList(0);
    
    return 0;
}
  
int BasicPlayer :: PlayOnceByItem()     //单曲播放 once
{
    playerLogDebug("BasicPlayer :: PlayOnceByItem\n");
    StreamPlayerUtilityModePlayOnceByItem(0);
    
    return 0;
}



int BasicPlayer :: PlayByListIndex(int)  //从播放列表的第几个位置开始
{
    playerLogDebug("BasicPlayer :: PlayByListIndex\n");
    StreamPlayerUtilityModePlayByListIndex(0);

    return 0;
}


int BasicPlayer :: PlayByFileOffset(int) //从上次离开时记录的播放位置开始  //remember the play location of last times
{
    playerLogDebug("BasicPlayer :: PlayByFileOffset\n");
    StreamPlayerUtilityModePlayByFileOffset(0);

    return 0;
}


int BasicPlayer :: PlayByEntryId(int)  //从指定的列表中的文件ID开始
{
    playerLogDebug("BasicPlayer :: PlayByEntryId\n");
    StreamPlayerUtilityModePlayByEntryId(0);

    return 0;
}



//============================================================================================================

// 播放时间
unsigned int BasicPlayer :: getTotalTime()
{
    playerLogDebug("BasicPlayer :: getTotalTime\n");
    StreamPlayerGetTotalTime(0);

    return 0;
}


unsigned int BasicPlayer :: getCurrentTime()
{
    playerLogDebug("BasicPlayer :: getCurrentTime\n");
    StreamPlayerGetCurrentTime(0);

    return 0;
}


unsigned int BasicPlayer :: getCurrentTimeString(char *TimeString)
{
    playerLogDebug("BasicPlayer :: getCurrentTimeString\n");
    StreamPlayerGetCurrentTimeString(0);

    return 0;
}







//============================================================================================================

/**   字幕    **/
int BasicPlayer :: GetAllSubtitleInfo(string &strValue)
{
    playerLogDebug("BasicPlayer :: GetAllSubtitleInfo\n");
    StreamPlayerGetAllSubtitleInfo(0);

    return 0;
}


int BasicPlayer :: GetCurrentSubtitleInfo(string &strValue)
{
    playerLogDebug("BasicPlayer :: GetCurrentSubtitleInfo\n");
    StreamPlayerGetCurrentSubtitleInfo(0);

    return 0;
}


void BasicPlayer :: SelectSubtitle(int subtitlePid)
{
    playerLogDebug("BasicPlayer :: SelectSubtitle\n");
    StreamPlayerSelectSubtitle(0);

    return ;
}


void BasicPlayer :: SwitchSubtitle(void)
{
    playerLogDebug("BasicPlayer :: SwitchSubtitle\n");
    StreamPlayerSwitchSubtitle(0);

    return ;
}


int  BasicPlayer :: GetSubtitileFlag(void)
{
    playerLogDebug("BasicPlayer :: GetSubtitileFlag\n");
    StreamPlayerGetSubtitileFlag(0);

    return 0;
}


void BasicPlayer :: SetSubtitileFlag(int subtitleFlag)
{
    playerLogDebug("BasicPlayer :: SetSubtitileFlag\n");
    StreamPlayerSetSubtitileFlag(0);

    return ;
}





//============================================================================================================

/** 基本音频控制： **/
// 音量
int BasicPlayer :: GetVolume(void)
{
    playerLogDebug("BasicPlayer :: GetVolume\n");
    StreamPlayerGetVolume(0);

    return 0;
}


void BasicPlayer :: SetVolume(int)
{
    playerLogDebug("BasicPlayer :: SetVolume\n");
    StreamPlayerSetVolume(0);

    return ;
}


void BasicPlayer :: SetMute(int)
{
    playerLogDebug("BasicPlayer :: SetMute\n");
    StreamPlayerSetMute(0);

    return ;
}


int BasicPlayer :: GetMute(void)
{
    playerLogDebug("BasicPlayer :: GetMute\n");
    StreamPlayerGetMute(0);

    return 0;
}



/** 声道    **/
void BasicPlayer :: SetChannel(int)
{
    playerLogDebug("BasicPlayer :: SetChannel\n");
    StreamPlayerSetChannel(0);

    return ;
}


int BasicPlayer :: GetChannel(void)
{
    playerLogDebug("BasicPlayer :: GetChannel\n");
    StreamPlayerGetChannel(0);

    return 0;
}


int BasicPlayer :: GetCurrentAudioChannel(string &strValue)
{
    playerLogDebug("BasicPlayer :: GetCurrentAudioChannel\n");
    StreamPlayerGetCurrentAudioChannel(0);

    return 0;
}


void BasicPlayer :: SwitchAudioChannel(string strValue)
{
    playerLogDebug("BasicPlayer :: SwitchAudioChannel\n");
    StreamPlayerSwitchAudioChannel(0);

    return ;
}




/**   音轨   **/
void BasicPlayer :: SetTrack(int)
{
    playerLogDebug("BasicPlayer :: SetTrack\n");
    StreamPlayerSetTrack(0);

    return ;
}


int BasicPlayer :: GetTrack(void)
{
    playerLogDebug("BasicPlayer :: GetTrack\n");
    StreamPlayerGetTrack(0);

    return 0;
}


int BasicPlayer :: GetTrackInfo(char *pTrackInfo)
{
    playerLogDebug("BasicPlayer :: GetTrackInfo\n");
    StreamPlayerGetTrackInfo(0);

    return 0;
}


int BasicPlayer :: GetAllAudioTrackInfo(char *pTrackInfo)
{
    playerLogDebug("BasicPlayer :: GetAllAudioTrackInfo\n");
    StreamPlayerGetAllAudioTrackInfo(0);

    return 0;
}


int BasicPlayer :: GetCurrentAudioTrackInfo(char *pTrackInfo)
{
    playerLogDebug("BasicPlayer :: GetCurrentAudioTrackInfo\n");
    StreamPlayerGetCurrentAudioTrackInfo(0);

    return 0;
}


void BasicPlayer :: SelectAudioTrack(int audioTrackPid)
{
    playerLogDebug("BasicPlayer :: SelectAudioTrack\n");
    StreamPlayerSelectAudioTrack(0);

    return ;
}


void BasicPlayer :: SwitchAudioTrack(void)
{
    playerLogDebug("BasicPlayer :: SwitchAudioTrack\n");
    StreamPlayerSwitchAudioTrack(0);

    return ;
}





//============================================================================================================

/** 播放UI展示 **/
void BasicPlayer :: setUIFlags(uint32_t flags)
{
    playerLogDebug("BasicPlayer :: setUIFlags\n");
    return ;
}


void BasicPlayer :: clearUIFlags(uint32_t flags)
{
    playerLogDebug("BasicPlayer :: clearUIFlagsclearUIFlags\n");
    return ;
}


int BasicPlayer :: setStreamVideoLocation(int pIndex, int x, int y, int w, int h, int mode )
{
    playerLogDebug("BasicPlayer :: setStreamVideoLocation\n");
    return 0;
}


void BasicPlayer :: refreshVideoDisplay()
{
    playerLogDebug("BasicPlayer :: refreshVideoDisplay\n");
    return ;
}












}


//end namespace Player










