#ifndef __PLAYER_UTILITY_HANDLER_H__
#define __PLAYER_UTILITY_HANDLER_H__

#include "PlayUtilityDataStructures.h"

typedef enum PlayState{
    PLAY_STATE_RESERVED = 0,
    PLAY_STATE_CLOSE,       //播放退出（mid_stream_close 触发状态改变）
    PLAY_STATE_OPEN,        //播放打开状态，此状态为一临时状态（只在播放模块内部使用）
    PLAY_STATE_PLAY,        //播放
    PLAY_STATE_PAUSE,       //暂停
    PLAY_STATE_FAST,        //快进快退
    PLAY_STATE_MAX
} PLAY_STATE;




/**
 *  线程：用于基本的控制命令的处理；
 *  
 **/
int StreamCtrlThreadCreate(StreamCtrl_Type *streamCtrl);













#endif  //__PLAYER_UTILITY_HANDLER_H__

