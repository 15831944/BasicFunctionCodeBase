
#include <stdio.h>
#define __STDC_CONSTANT_MACROS

//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>

#include "log/LogC.h"
#include "LogPlayer.h"

#ifdef __cplusplus
};
#endif

#include "CodecUtilityMessageType.h"
#include "SoftwareCodec.h"
#include "PlayUtilityDataStructures.h"

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

static int thread_exit=0;
static int video_opened = 0;

static int sfp_refresh_thread(void *opaque)
{
    printf("sfp_refresh_thread::video_opened[%d]\n", video_opened);
    
	while (thread_exit == 0) {
		SDL_Event event;
		event.type = SFM_REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}


int SoftwareCodecInit(int argc, char* argv[], CodecCtrl_type* codecCtrl)
{
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int ret, got_picture;
    
    
	//------------SDL----------------
	int screen_w, screen_h;
	SDL_Window *screen;  //2.0的一个特征，窗口指针
	SDL_Renderer* sdlRenderer;//渲染器
	SDL_Texture* sdlTexture;// 画刷
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

	struct SwsContext *img_convert_ctx;

	char filepath[]="cuc_ieschool.flv";

	av_register_all();
	//avformat_network_init();
	pFormatCtx = avformat_alloc_context();


    //打开媒体文件 //avformat_open_input   av_open_input_file
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) {
        playerLogError("Couldn't open input stream, argv[%s].\n", argv[1]); /*可能会有因monitor模块启动后，打不开的情况*/
        return -1;
    }
    
    // Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0){
		playerLogError("Couldn't find stream information.\n");  // Couldn't find stream information
		return -1;
	}
	videoindex=-1;
    
    // Find the first video stream
	for(i = 0; i < pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoindex = i;
			break;
		}
	if(videoindex == -1){
		playerLogError("Didn't find a video stream.\n");  // Didn't find a video stream
		return -1;
	}
    
    // Get a pointer to the codec context for the video stream
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    
    // Find the decoder for the video stream
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec == NULL){
		playerLogError("Codec not found.\n"); // Codec not found
		return -1;
	}
    
    // Open codec
	if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		playerLogError("Could not open codec.\n");
		return -1;
	}
        
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
    //Allocate frame video
    pFrame = av_frame_alloc();
    // Allocate video frame —— struct AVFrame
    pFrameYUV = av_frame_alloc();
#else
    pFrame = avcodec_alloc_frame();
    // Allocate video frame
    pFrameYUV = avcodec_alloc_frame();
#endif

    // Assign appropriate parts of buffer to image planes in pFrameYUV
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	//Output Info-----------------------------
	printf("---------------------------------- File Information ---------------------------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------------------------------------------\n");
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
                                     pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
	

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
		playerLogError( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return -1;
	} 
	//SDL 2.0 Support for multiple windows
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL);

	if(!screen) {  
		playerLogError("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  // 为创建的窗口配置渲染器
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);  //创建一个画刷

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
	//------------SDL End------------
    
	//Event Loop
	for (i = 0; ; i++) {
		//Wait
		SDL_WaitEvent(&event);
        
        playerLogInfo("event.type[%u],\n", event.type);
        switch(event.type){
        case SOFTWARE_CODEC_EVENT_OPEN:{
            //TODO:: do open stream
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_OPEN\n", event.type);
            break;
            }
        case SOFTWARE_CODEC_EVENT_PLAY:{
            //TODO:: do play stream
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_PLAY\n", event.type);
            video_opened = 1;
            break;
            }
        case SOFTWARE_CODEC_EVENT_CLOSE:{
            //TODO:: do close stream
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_CLOSE\n", event.type);
            break;
            }
        case SOFTWARE_CODEC_EVENT_STOP:{
            //TODO:: do stop stream
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_STOP\n", event.type);
            video_opened = 0;
            break;
            }    
        case SOFTWARE_CODEC_EVENT_PAUSE:{
            //TODO:: do pause play
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_PAUSE\n", event.type);
            video_opened = 0;
            break;
            }    
        case SOFTWARE_CODEC_EVENT_RESUME:{
            //TODO:: do resume play
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_RESUME\n", event.type);
            video_opened = 1;
            break;
            }    
        case SOFTWARE_CODEC_EVENT_SEEKTO:{
            //TODO:: do seek to (include fast forward && fast rewind)
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_SEEKTO\n", event.type);
            break;
            }
        case SOFTWARE_CODEC_EVENT_VIDEO_FULLSCREEN:{
            //TODO:: do change video window full screen
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_VIDEO_FULLSCREEN\n", event.type);
            break;
            }
        case SFM_REFRESH_EVENT:
        case SOFTWARE_CODEC_EVENT_VIDEO_REFRESH:{
            //TODO:: do refresh display
            playerLogVerbose("event.type[%u],SOFTWARE_CODEC_EVENT_VIDEO_REFRESH\n", event.type);
            if(video_opened == 0)
                break;
            if(av_read_frame(pFormatCtx, packet) >= 0){
				if(packet->stream_index == videoindex){
                    // Decode the next chunk of data
					ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if(ret < 0){
						playerLogError("Decode Error.\n");
						return -1;
					}
					if(got_picture){
						sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
						//SDL---------------------------
						SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );  
						SDL_RenderClear( sdlRenderer );  
						//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
						SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);  
						SDL_RenderPresent( sdlRenderer );  
						//SDL End-----------------------
					}
				}
				av_free_packet(packet);
			}else{
				//Exit Thread
				thread_exit=1;
				break;
			}
            break;
            }
        case SDL_QUIT:{
            //TODO:: do exit
            playerLogVerbose("SDL_QUIT\n");
            thread_exit=1;
            break;
            }
        default :{
            break;
            }
        } // end switch(event.type)
        playerLogInfo("for loop count i=[%d]\n", i);
	} //end for(;;)
        
	sws_freeContext(img_convert_ctx);
	SDL_Quit();
    
    
	//--------------
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)  
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
#else
    av_free(pFrameYUV);
#endif
    
	avcodec_close(pCodecCtx);
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)  
	avformat_close_input(&pFormatCtx);
#else
    av_close_input_file(pFormatCtx);
#endif

	return 0;
}





