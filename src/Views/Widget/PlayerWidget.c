/**
 * 最简单的基于FFmpeg的视频播放器
 * Simplest FFmpeg Player
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序实现了视频文件的解码和显示（支持HEVC，H.264，MPEG2等）。
 * 是最简单的FFmpeg视频解码方面的教程。
 * 通过学习本例子可以了解FFmpeg的解码流程。
 * This software is a simplest video player based on FFmpeg.
 * Suitable for beginner of FFmpeg.
 */

/**
 *  编译：
 *  g++ simplest_ffmpeg_player.cpp -g -o simplest_ffmpeg_player.out \
 *  -I /usr/local/SDL/include -L /usr/local/SDL/lib -lSDLmain -lSDL \
 *  -I /usr/local/ffmpeg/include -L /usr/local/ffmpeg/lib -lswscale -lavcodec -lavformat -lavutil -lm -lz
 *
 *  所使用的库版本：    ffmpeg：0.8.15
 *                      SDL：1.2.0
 **/

#include <stdio.h>

//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include "SDL/SDL.h"

#include "LogWidget.h"
#include "PlayerWidget.h"

#ifdef __cplusplus
}
#endif



int init(int argc, char *argv[])
{

    return 0;
}

