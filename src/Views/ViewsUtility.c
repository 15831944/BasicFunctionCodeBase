
#include <stdio.h>

#include "ViewsUtility.h"



static int gScreenW = 0;
static int gScreenH = 0;

SDL_Renderer* sdlRenderer;
SDL_Texture* sdlTexture;




int InitViewsUtility(void)
{
    
    
    
    
    
    return 0;
}




























/**
 *  
 *  
 *  //SDL2.0入门
#include "SDL2/SDL.h"
//SDL必备头文件
#include<SDL2/SDL_image.h>
//SDL使用图片扩展


int main(int argc,char **argv)
{
//SDL必须这样写主函数

SDL_Window *w=NULL;//2.0的一个特征，窗口指针
SDL_Surface *sur = NULL;// 表面
SDL_Texture *tex = NULL;// 画刷
SDL_Renderer *ren=NULL;//渲染器

SDL_Init(SDL_INIT_EVERYTHING);//启动SDL


w = SDL_CreateWindow("别踩白块儿", 0, 0, 480, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);// 配置窗口参数并创建 ，480和800是窗口大小，但是别在意，手机上永远是全屏。
SDL_Window *SDL_CreateWindow（）；
｛
“窗口标题，x，y，宽，高，falg”//四个参数

最后的参数种类：
SDL_WINDOW_FULLSCREEN 全屏窗口
SDL_WINDOW_FULLSCREEN_DESKTOP 全屏窗口在当前桌面分辨率
SDL_WINDOW_OPENGL 与OpenGL窗口可用的上下文
SDL_WINDOW_HIDDEN 窗口不可见
SDL_WINDOW_SHOWN 窗口是可见的
SDL_WINDOW_BORDERLESS 没有窗口装饰
SDL_WINDOW_RESIZABLE 窗口可以调整大小
SDL_WINDOW_MINIMIZED 窗口最小化
SDL_WINDOW_MAXIMIZED 窗口最大化
SDL_WINDOW_INPUT_GRABBED 窗口有输入焦点
SDL_WINDOW_ALLOW_HIGHDPI 窗口应该创建在high-DPI模式如果支持(> = SDL 2.0.1)
｝


ren = SDL_CreateRenderer(w, -1, SDL_RENDERER_PRESENTVSYNC);// 为创建的窗口配置渲染器

sur=IMG_Load("/storage/sdcard0/c put/icon.png");//使用图片扩展函数，加载图片文件，你可以加载任意图片格式//2.0的一个特点，你必须使用绝对路径，这个是我的路径，你们自己修改

tex=SDL_CreateTextureFromSurface(ren,sur);//创建一个画刷，从sur指针那里

SDL_RenderCopy(ren,tex,NULL,NULL);//将画刷粘贴到渲染器上

SDL_RenderPresent(ren);//更新屏幕，相当于1.2中的SDL_Flip


SDL_Delay(2000);//暂停2秒
SDL_DestroyTexture(tex);//释放画刷所占内存SDL_DestroyRenderer(ren);//释放渲染器
SDL_DestroyWindow(w);
//释放渲染器和窗口并退出
SDL_Quit();//记得退出SDL
return 0;
}
 *  
 *  
 **/