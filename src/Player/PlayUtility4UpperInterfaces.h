#ifndef __PLAYER_UTILITY_4_UPPER_INTERFACES_H__
#define __PLAYER_UTILITY_4_UPPER_INTERFACES_H__

//=================================================================================================
//=================================================================================================



/**
 *  通过发送命令，实现基本的播控功能
 **/
//基本播放控制函数

#ifdef __cplusplus 
extern "C" { 
#endif

int StreamPlayerUtilityCtrlOpen(int playerIndex);
int StreamPlayerUtilityCtrlPlay(int playerIndex);
int StreamPlayerUtilityCtrlClose(int playerIndex);
int StreamPlayerUtilityCtrlStop(int playerIndex);
int StreamPlayerUtilityCtrlPause(int playerIndex);
int StreamPlayerUtilityCtrlResume(int playerIndex);
int StreamPlayerUtilityCtrlSeekto(int playerIndex);
int StreamPlayerUtilityCtrlSeektoStart(int playerIndex);
int StreamPlayerUtilityCtrlSeektoEnd(int playerIndex);
int StreamPlayerUtilityCtrlFastForward(int playerIndex);
int StreamPlayerUtilityCtrlFastRewind(int playerIndex);


//播放方式控制
int StreamPlayerUtilityModePlayFirst(int playerIndex);
int StreamPlayerUtilityModePlayLast(int playerIndex);
int StreamPlayerUtilityModePlayNext(int playerIndex);
int StreamPlayerUtilityModePlayPrevious(int playerIndex);

int StreamPlayerUtilityModePlayRepeatList(int playerIndex);    //循环播放 all repeat
int StreamPlayerUtilityModePlayRepeatItem(int playerIndex);    //单曲循环 repeat once
int StreamPlayerUtilityModePlayShuffleByList(int playerIndex);   //随机播放 shuffle
int StreamPlayerUtilityModePlayOnceByList(int playerIndex);      //顺序播放 order      
int StreamPlayerUtilityModePlayOnceByItem(int playerIndex);      //单曲播放 once

int StreamPlayerUtilityModePlayByListIndex(int playerIndex);
int StreamPlayerUtilityModePlayByFileOffset(int playerIndex);
int StreamPlayerUtilityModePlayByEntryId(int playerIndex);





#ifdef __cplusplus 
} 
#endif




//播放时间
int StreamPlayerGetTotalTime(int playerIndex);
int StreamPlayerGetCurrentTime(int playerIndex);
int StreamPlayerGetCurrentTimeString(int playerIndex);




//播放字幕
int StreamPlayerGetAllSubtitleInfo(int playerIndex);
int StreamPlayerGetCurrentSubtitleInfo(int playerIndex);
int StreamPlayerSelectSubtitle(int playerIndex);
int StreamPlayerSwitchSubtitle(int playerIndex);
int StreamPlayerGetSubtitileFlag(int playerIndex);
int StreamPlayerSetSubtitileFlag(int playerIndex);





//播放音量
int StreamPlayerGetVolume(int playerIndex);
int StreamPlayerSetVolume(int playerIndex);
int StreamPlayerSetMute(int playerIndex);
int StreamPlayerGetMute(int playerIndex);


//播放声道
int StreamPlayerSetChannel(int playerIndex);
int StreamPlayerGetChannel(int playerIndex);
int StreamPlayerGetCurrentAudioChannel(int playerIndex);
int StreamPlayerSwitchAudioChannel(int playerIndex);


//播放音轨
int StreamPlayerSetTrack(int playerIndex);
int StreamPlayerGetTrack(int playerIndex);
int StreamPlayerGetTrackInfo(int playerIndex);
int StreamPlayerGetAllAudioTrackInfo(int playerIndex);
int StreamPlayerGetCurrentAudioTrackInfo(int playerIndex);
int StreamPlayerSelectAudioTrack(int playerIndex);
int StreamPlayerSwitchAudioTrack(int playerIndex);





#endif  //__PLAYER_UTILITY_4_UPPER_INTERFACES_H__

