#ifndef __AUDIO_UTILITY_INTERFACES_H__
#define __AUDIO_UTILITY_INTERFACES_H__



/**
 *  Codec提供的接口
 **/
 
 

//=================================================================================================
// 音量调节
//=================================================================================================
int CodecUtilityGetVolume(int playerIndex);
int CodecUtilitySetVolume(int playerIndex);
int CodecUtilitySetMute(int playerIndex);
int CodecUtilityGetMute(int playerIndex);


//=================================================================================================
// 声道调节
//=================================================================================================
int CodecUtilitySetChannel(int playerIndex);
int CodecUtilityGetChannel(int playerIndex);
int CodecUtilityGetCurrentAudioChannel(int playerIndex);
int CodecUtilitySwitchAudioChannel(int playerIndex);



//=================================================================================================
// 音轨调节
//=================================================================================================
int CodecUtilitySetTrack(int playerIndex);
int CodecUtilityGetTrack(int playerIndex);
int CodecUtilityGetTrackInfo(int playerIndex);
int CodecUtilityGetAllAudioTrackInfo(int playerIndex);
int CodecUtilityGetCurrentAudioTrackInfo(int playerIndex);
int CodecUtilitySelectAudioTrack(int playerIndex);
int CodecUtilitySwitchAudioTrack(int playerIndex);









#endif  //__AUDIO_UTILITY_INTERFACES_H__

