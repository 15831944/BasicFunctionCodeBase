#ifndef __PLAYER_UTILITY_DATA_STRUCTURES_H__
#define __PLAYER_UTILITY_DATA_STRUCTURES_H__

#include "ThreadMutex.h"
#include "MessageQueue.h"
#include "PlayUtilityMessageType.h"



//=================================================================================================
//=================================================================================================


#define     PLAY_LIST_LENGTH_DEFAULT    64

typedef struct {
    int     testarg1;
    int     isPlaying;
    char**  PlayListAddr;
    char*   PlayUrl;

} PlayerArgs_Type;




typedef struct {
    int testarg1;
    int testarg2;
    
} CodecUtils_Type;


/**
 *  解码器编号
 **/
typedef enum codecID{
    CODEC_ID_RESERVED,
    CODEC_ID_RTSP,
    CODEC_ID_AUDIO,
    CODEC_ID_HTTP,
    CODEC_ID_PVR,
    CODEC_ID_DVBS,      //5
    CODEC_ID_MOSAIC,
    CODEC_ID_FLASH,
    CODEC_ID_DISK,
    CODEC_ID_RTP2TS,
    CODEC_ID_MIX_PCM,   //10
    CODEC_ID_MIX_MP3,
    CODEC_ID_HTTP_PCM,
    CODEC_ID_HTTP_MP3,
    CODEC_ID_ZEBRA,
    CODEC_ID_ZEBRA_PCM, //15
    CODEC_ID_HTTP_MPA,
    CODEC_ID_RESERVE,
    CODEC_ID_HTTP_LIVE,
    CODEC_ID_APPLE,
    CODEC_ID_MAX
} PlayCodecID_Type;




typedef enum mediaType{
    MEDIA_TYPE_RESERVED,
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_DATA,
    MEDIA_TYPE_SUBTITLE,
    MEDIA_TYPE_ATTACHMENT,
    MEDIA_TYPE_MAX
} PlayMedia_Type;




/**
 *  播放内容类型
 **/
typedef enum {
    APP_PLAYTYPE_RESERVED,
    APP_PLAYTYPE_VOD = 1,   //VOD播放
    APP_PLAYTYPE_IPTV,      //频道播放 APP_PLAYTYPE_UNICAST，APP_PLAYTYPE_MULTICAST和APP_PLAYTYPE_IGMP合并而成
    APP_PLAYTYPE_HTTP,      
    APP_PLAYTYPE_PVR,       //录制文件播放
    APP_PLAYTYPE_AUDIO,
    APP_PLAYTYPE_TSTV,      //支持本地时移频道播放
    APP_PLAYTYPE_MOSAIC,    //码赛克
    APP_PLAYTYPE_FLASH,     //FLASH
    APP_PLAYTYPE_DISK,      //磁盘文件播放
    APP_PLAYTYPE_RTP2TS,
    APP_PLAYTYPE_MIX_PCM,   
    APP_PLAYTYPE_MIX_MP3,
    APP_PLAYTYPE_HTTP_PCM,
    APP_PLAYTYPE_HTTP_MP3,
    APP_PLAYTYPE_ZEBRA,
    APP_PLAYTYPE_ZEBRA_PCM, 
    APP_PLAYTYPE_HTTP_MPA,  //支持 mp3,mp2,aac
    APP_PLAYTYPE_HLS,       //Http live streaming
    APP_PLAYTYPE_HTTP_LIVE, //Http live streaming 
    APP_PLAYTYPE_APPLE_VOD,
    APP_PLAYTYPE_APPLE_IPTV,
    APP_PLAYTYPE_MAX
} APP_PLAYTYPE;




/**
 *   基本播控操作接口：
 *          新建一个结构体，用于各种操作，包括快进快退等的操作
 **/
typedef struct playerController{
    int (* Open)  (void* handle, void* args);
    int (* Play)  (void* handle, void* args);
    int (* Close) (void* handle, void* args);
    int (* Stop)  (void* handle, void* args);
    
    int (* Pause)    (void* handle, void* args);
    int (* Resume)   (void* handle, void* args);
    
    int (* Seekto)       (void* handle, void* args, unsigned int offset);
    int (* SeektoStart)  (void* handle, void* args, unsigned int offset);
    int (* SeektoEnd)    (void* handle, void* args, unsigned int offset);
    
    int (* FastForward)     (void* handle, void* args, int scale);
    int (* FastRewind)      (void* handle, void* args, int scale);
    
} PlayerInterfacesCtrls_Type;



typedef struct {
    int                     index;
    int                     appType;
    int                     playState;
    int                     playScale;
    
    int                     isPlayRepeatList;
    int                     isPlayRepeatItem;
    
    MessageQueue_Type       msgQueue;
    
    PlayerArgs_Type         playArgs;
    CodecUtils_Type*        playUtils;
    
    
    char*                   argBuf;
    PlayerInterfacesCtrls_Type   codecCtrl[CODEC_ID_MAX];
} StreamCtrl_Type;


typedef struct {
    int                     index;
    int                     appType;
    int                     playState;
    int                     playScale;
    
    PlayCodecID_Type        codecID;
    
    PlayerArgs_Type         playArgs;
    CodecUtils_Type*        playUtils;
    
    char*                   argBuf;
} CodecCtrl_type;



typedef struct {
    StreamCtrl_Type*    stream;
    PlayerArgs_Type     playArgs;
    char*               argBuf; //play 的流地址
    int                 argSize; //对应的argbuf的size值

    unsigned int        playMagic; //对应一个特定的值.以保证上层和stram 操作同一个节目
    
    int                 openFlag; //执行open操作后,此标志位置1, (close)关闭置0
    
    unsigned int        timeTotal; //组播表示时移时间(-1表示不支持时移), VOD表示节目实际时间
    unsigned int        timeCurrent; //当前时间
    long long           byteTotal; //两个应该是local 播放用的.可不关注 zebra调用
    long long           byteCurrent;
    
} PlayCtrl_type;


















#endif  //__PLAYER_UTILITY_DATA_STRUCTURES_H__

