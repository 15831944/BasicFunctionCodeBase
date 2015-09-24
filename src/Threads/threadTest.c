#include <stdio.h>
#include <unistd.h>

#include "ThreadMutex.h"
#include "ThreadTask.h"

#include "Log/LogC.h"
#include "LogThreads.h"
#include "Threads.h"


extern int log_threads_init();


static int testCommon(void *arg)
{
    printf("testCommon\n");
    threadsLogInfo("testCommon::[%s]\n", arg);
    return ;
}

static int testAdvanced(void *arg)
{
    printf("testAdvanced\n");
    threadsLogInfo("testAdvanced::[%s]\n", arg);
    return ;
}

int threadCreateTest()
{
    threadsLogDebug("threadCreateTest\n");

    ThreadTaskCreateCommon("testCommon", testCommon, (void *)"testCommon");
    ThreadTaskCreateAdvanced("testAdvanced", testAdvanced, (void *)"testAdvanced");

    threadsLogDebug("==============\n");
    sleep(6);
    return 0;
}


int main ()
{
    log_threads_init();

    ThreadTaskInit();

    threadCreateTest();

    ThreadPoolShow();
    return 0;
}






