#ifndef __BASICPLAYER_H__
#define __BASICPLAYER_H__

#include <string>


#ifdef __cplusplus

namespace Player {
using namespace std;

class BasicPlayer {
public:
    BasicPlayer();
    ~BasicPlayer();
    
    
    /** 基本的播放控制 **/
    // 播放控制
    virtual int Open();
    virtual int Close();
    
    virtual int Play();
    virtual int Stop();
    
    virtual int Pause();
    virtual int Resume();
    
    virtual int FastForward(int);
    virtual int FastRewind(int);

    virtual int SeekTo();    
	virtual int SeekToStart(void);
    virtual int SeekEnd();
    

    
    // 播放方式
    virtual int PlayFirst(void);
    virtual int PlayLast(void);
    
    virtual int PlayNext(void);
    virtual int PlayPrevious(void);
    
    virtual int PlayRepeatList();    //循环播放 all repeat
    virtual int PlayRepeatItem();    //单曲循环 repeat once
    virtual int PlayShuffleByList();   //随机播放 shuffle
    virtual int PlayOnceByList();      //顺序播放 order      
    virtual int PlayOnceByItem();      //单曲播放 once

    virtual int PlayByListIndex(int);  //从播放列表的第几个位置开始
    virtual int PlayByFileOffset(int);  //从上次离开时记录的播放位置开始  //remember the play location of last times
    virtual int PlayByEntryId(int);  //从指定的列表中的文件ID开始
    
    
    
    // 播放时间
    virtual unsigned int getTotalTime();
    virtual unsigned int getCurrentTime();
    virtual unsigned int getCurrentTimeString(char *TimeString);

    // 字幕
    int GetAllSubtitleInfo(string& strValue);
    int GetCurrentSubtitleInfo(string& strValue);
    void SelectSubtitle(int subtitlePid);
    void SwitchSubtitle(void);
    int  GetSubtitileFlag(void);
    void SetSubtitileFlag(int subtitleFlag);


    
    /** 基本音频控制： **/
    // 音量
    int GetVolume(void);
    void SetVolume(int);
    void SetMute(int);
    int GetMute(void);
    
    // 声道
    void SetChannel(int);
    int GetChannel(void);
    int GetCurrentAudioChannel(string& strValue);
    void SwitchAudioChannel(string strValue);

    // 音轨
    void SetTrack(int);
    int GetTrack(void);
    int GetTrackInfo(char *pTrackInfo);
    int GetAllAudioTrackInfo(char *pTrackInfo);    
    int GetCurrentAudioTrackInfo(char *pTrackInfo);    

    void SelectAudioTrack(int audioTrackPid);
    void SwitchAudioTrack(void);


    
    /** 播放UI展示 **/
    void enableUI(bool enable) { mUIEnabled = enable; }
    bool IsUIEnabled() { return mUIEnabled; }
    enum UIFlag_Mask {
        PlayState_Mask    = 0x00000001,
        ChannelNO_Mask    = 0x00000010,
        ProgressBar_Mask  = 0x00000100,
        AudioMute_Mask    = 0x00001000,
        AudioTrack_Mask   = 0x00010000,
        AudioVolume_Mask  = 0x00100000,
        AudioBGGraph_Mask = 0x01000000
    };
    void setUIFlags(uint32_t flags);
    void clearUIFlags(uint32_t flags);
    
    
    int setStreamVideoLocation(int pIndex, int x, int y, int w, int h, int mode );
    void refreshVideoDisplay();
    int mPlaylistMode;
    int mPlayCycleFlag;                 // 0为循环播放，1为播放一次.默认为1
    int mPlayRandomFlag;                // 0为顺序播放，1为随机播放.默认为0
    int mDisplayMode;                   // 0为小窗口播放，1为全屏播放，2、3、255
    int m_subtitleFlag;
	int m_muteFlag;
	int	m_VideoDisplaytop;
	int m_VideoDisplayleft;
	int	m_VideoDisplaywidth;
	int	m_VideoDisplayheight;
	int m_PlayerInstanceType; /*0,Main; 1, PIP; 2, MOSAIC */

protected:
    
    
private:
    //Program *mProgram;
    /* 不是所有播放实例都可以操作界面元素，应该由主的、全屏的控制。其它的播放实例mUIEnabled == false。*/
    static bool mUIEnabled;
    static uint32_t mUIFlags;
    static uint32_t mUIFlagsForcedMask;
    static uint32_t mUIFlagsForcedValue;



    
};  //end class BasicPlayer



}  //end namespace Player

#endif //__cplusplus

#endif //__BASICPLAYER_H__

