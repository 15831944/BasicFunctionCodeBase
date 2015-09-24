 #ifndef __SOFTWARE_CODEC2_H__
#define __SOFTWARE_CODEC2_H__
 
 
 
#ifdef __cplusplus
extern "C" {
#endif
#include "PlayUtilityDataStructures.h"



int SoftwareCodecInit(int argc, char* argv[], CodecCtrl_type* codecCtrl);
 
 
 
 
 
 
 
 
 
 
#ifdef __cplusplus
};
#endif
 
 
 
 
 
 
class CRTSPPlayer{
public:
    CRTSPPlayer(HWND hWnd, LPRECT lpRect);
    ~CRTSPPlayer(void);
    
public:
    // 打开媒体文件
    BOOL OpenMedia(LPCTSTR pFileName);
    
    // 播放
    void Play();
    
    // 暂停
    void Pause();
    
    // 停止
    void Stop();
    
    // 获取播放状态
    RTSP_PLAYSTATUS GetPlayStatus(void);
    
    private:
    // 解码初始化
    int DecodeInit(LPCTSTR pFileName);
    
    // 卸载
    void DecodeUninit();
    
    // 开始解码线程
    BOOL StartDecodeThread();
    
    // 停止解码线程
    void StopDecodeThread();
    
    // 解码线程
    static int WINAPI ThreadDecodeVideo(LPVOID lpParam);
    
    // 开始解码任务
    int BeginDecode();
    
    // 显示
    void Display();
    
    // 图像转换
    int ImgConvert(AVPicture * dst, PixelFormat dstFormt, const AVPicture * src, PixelFormat srcFormt, int src_width, int src_height);
    
    // 设置播放状态
    void SetPlayStatus(RTSP_PLAYSTATUS playStatus);
    
private:
    HANDLE m_hDecodeThread;
    BOOL m_bExitDecodeThread;
    TCHAR m_strFilePath[MAX_PATH];
    AVFormatContext* m_pFormatContext;
    AVCodecContext* m_pCodecContext;
    AVCodec* m_pCodec;
    AVPacket m_struPacket;
    int m_nStreamIndex;
    AVFrame* m_pFrameYUV;
    AVFrame* m_pFrameRGB;
    int m_nFrameWidth;
    int m_nFrameHeight;
    BYTE* m_pBufRGB; // 解码后的RGB数据
    RTSP_PLAYSTATUS m_nPlayStatus;
    HWND m_hWnd;
    RECT m_rcWnd;
    
}; 
 
 
 
 
 
 
 
 
#endif  //__SOFTWARE_CODEC2_H__