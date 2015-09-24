/**
 *  MonitorInterface.c文件
 *  负责初始化Play播放相关的结构体，主要是解码器跟上层控制相关的结构；
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

#include "Log/LogC.h"
#include "logMonitor.h"
#include "Monitors.h"

#include "ThreadMutex.h"
#include "ThreadTask.h"

#include "includes/ServerMain.h"
#include "MonitorInterface.h"
 
static ThreadMutex_Type     gMonitorMutex = NULL;



static int MonitorBuildTime()
{
    monitorLogInfo("Monitor Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



//Usage: ./run [-c control_port] [-d data_port][-r fs_root] [-f configuration_file]

static int MonitorFTPServerTask(void *arg)
{
    int argc = 5;
    char* argv[] = {"monitor.elf", "-c", "3456", "-d", "34567"};
    
    monitorLogInfo("MonitorFTPServerTask::[%s]\n", arg);
    ServerMain(argc, argv);
    return 0;
}

int MonitorThreadCreate()
{
    monitorLogInfo("MonitorThreadCreate\n");

    //FTPServer
    ThreadTaskCreateAdvanced("MonitorFTPServerTask", MonitorFTPServerTask, (void *)"MonitorFTPServerTask");

    //SystemMonitor
    //ThreadTaskCreateAdvanced("MonitorSystemMonitorTask", MonitorSystemMonitorTask, (void *)"MonitorSystemMonitorTask");
    
    //TinyHttpd
    //ThreadTaskCreateAdvanced("MonitorTinyHttpdTask", MonitorTinyHttpdTask, (void *)"MonitorTinyHttpdTask");
    
    
    monitorLogInfo("==============\n");
    sleep(6);
    return 0;
}


int InitMonitor(void)
{
    log_monitor_init();
    MonitorBuildTime();
    
    MonitorThreadCreate();

    
    monitorLogDebug("InitMonitor.\n");
    return 0;
}






