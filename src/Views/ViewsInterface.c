/**
 *  ViewsInterface.c文件
 *  负责Play播放SDL层的初始化；
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include "SDL2/SDL.h"

#include "LogC.h"
#include "LogViews.h"
#include "Views.h"

#include "ViewsInterface.h"

#ifdef __cplusplus
}
#endif




static int ViewsBuildTime()
{
    viewsLogInfo("Views Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



int InitViews(void)
{
    log_views_init();
    ViewsBuildTime();
    
    
    viewsLogDebug("InitViews.\n");
    return 0;
}









