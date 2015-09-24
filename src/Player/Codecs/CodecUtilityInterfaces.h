#ifndef __CODEC_UTILITY_INTERFACES_H__
#define __CODEC_UTILITY_INTERFACES_H__



/**
 *  Codec提供的接口
 **/
 
 
//=================================================================================================
// 播放
//=================================================================================================
int PlayerCodecsOpen();
int PlayerCodecsPlay();
int PlayerCodecsClose();
int PlayerCodecsStop();
int PlayerCodecsPause();
int PlayerCodecsResume();
int PlayerCodecsSeekto();
int PlayerCodecsSeektoStart();
int PlayerCodecsSeektoEnd();
int PlayerCodecsFastForward();
int PlayerCodecsFastRewind();





//=================================================================================================
// 功能接口
//=================================================================================================
enum codecMode{
    CODEC_MODE_NORMAL,
    CODEC_MODE_ZEBRA,
    CODEC_MODE_NANO,
    CODEC_MODE_JUDGE
};
int CodecSubtitleNumberGet(int* subtitleNum);    
int CodecSubtitlePidGet(int index, int* subtitlePid);
int CodecSubtitleLangGet(int index, char* languageCode);
int CodecSubtitleLangFullNameGet(int index, char* languageName);



//=================================================================================================
// 播放字幕
//=================================================================================================
int CodecUtilityGetAllSubtitleInfo(int playerIndex);
int CodecUtilityGetCurrentSubtitleInfo(int playerIndex);
int CodecUtilitySelectSubtitle(int playerIndex);
int CodecUtilitySwitchSubtitle(int playerIndex);
int CodecUtilityGetSubtitileFlag(int playerIndex);
int CodecUtilitySetSubtitileFlag(int playerIndex);








#endif  //__CODEC_UTILITY_INTERFACES_H__

