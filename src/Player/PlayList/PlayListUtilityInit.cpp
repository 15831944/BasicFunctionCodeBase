/**
 *  
 *  主要负责初始化Playing List
 *  
 **/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log/LogC.h"
#include "../LogPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"
#include "ThreadTask.h"

#ifdef __cplusplus
};
#endif

#include "PlayListUtility.h"
#include "PlayListUtilityInit.h"


static ThreadMutex_Type     gPlayingListlMutex = NULL;




static int PlayingListTask(void *arg)
{
    playerLogDebug("PlayingListTask::[%s]\n", arg);
    
    LoadList(0);
    
    return 0;
}


static int PlayingListThreadCreate()
{
    playerLogDebug("PlayingListThreadCreate\n");

    ThreadTaskCreateAdvanced("PlayingListTask", PlayingListTask, (void *)"PlayingListTask");
    sleep(8);
    playerLogDebug("PlayingListThreadCreate ==sleep(8)==end==========\n");

    return 0;
}




int InitPlayList(int argc, char *argv[])
{
    playerLogDebug("InitPlayList.\n");
    gPlayingListlMutex = ThreadMutexCreate();

    PlayingListThreadCreate();
    
    return 0;
}















