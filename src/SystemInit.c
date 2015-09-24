#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


#include "build_info.h"
#include "LogC.h"
#include "logEntry.h"
#include "Entry.h"


#include "InitPlayer.h"
#include "InitVersions.h"

#include "MonitorInterface.h"
#include "MessageInterface.h"
#include "SettingsInterface.h"
#include "ThreadInterface.h"
#include "ViewsInterface.h"
#include "UtilsInterface.h"



extern int InitSettings(void);
extern int InitMessage(void);
extern int InitThread(void);
extern int InitUtils(void);
//运行环境初始化
static int EnvInit(void)
{
    entryLogDebug("EnvInit ... \n");
    
    VersionInit();
    InitMessage();
    InitThread();
    InitUtils();
    InitSettings();

    return 0;
}


extern int BasicPlayerInit();
extern int InitMonitor(void);
int main(int argc, char *argv[])
{
    printf("=============test==================\n");
    log_entry_init();
    
    EnvInit();
    InitPlayer(argc, argv);
    
    
    BasicPlayerInit();
    InitMonitor(); //必须放在InitPlayer()之后，否则会出现avformat_open_input() open input stream error 的情况

    sleep(45);  //目的是等待播放结束，可以由一个while循环替代，或者讲一个线程循环放到此处。
    return 0;
}






