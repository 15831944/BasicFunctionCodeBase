
#include <stdio.h>
#include <SDL2/SDL.h>

#include "LogViews.h"
#include "ViewsUtilityInit.h"



/**
 *  name::  InitViewsUtility
 *          初始化输出显示，主要是初始化SDL输出
 *          //以后会使用SDL绘制播放器窗体，因此与播放器窗体相关的初始化也在这部分完成；
 *          其初始值由InitSystemOptions得到；
 *          其初始值由InitSystemOptions得到；
 *
 **/
int InitViewsUtility(void)
{
    viewsLogDebug("InitViewsUtility.\n");

    //------------SDL----------------
	int screen_w, screen_h;
	SDL_Window *screen; 
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

    //SDL 2.0 Support for multiple windows
	screen_w = 1280;//pCodecCtx->width;
	screen_h = 720;//pCodecCtx->height;
	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL);

	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);  

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

    video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
  
  
  
  
    //SDL---------------------------
    SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );  
    SDL_RenderClear( sdlRenderer );  
    //SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
    SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);  
    SDL_RenderPresent( sdlRenderer );  
    //SDL End-----------------------

    
    
    
    
    viewsLogDebug("InitViewsUtility END. \n");
    return 0;
}




