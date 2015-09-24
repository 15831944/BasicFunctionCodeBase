#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "ThreadMutex.h"
#include "ThreadTask.h"
#include "LogThreads.h"


typedef struct Threads {
    pthread_t threadNo;
    char threadName[THREAD_NAME_LENGTH];
} Threads_Type;

Threads_Type gThreadsPool[THREAD_TASK_NUMBER_MAX];

//static pthread_t gThreadsPool[THREAD_TASK_NUMBER_MAX];
static ThreadMutex_Type gThreadMutex = NULL;
static int gThreadIndex; //signed position in gThreadsPool;

typedef void *(*pthread_routine) (void *);



void ThreadTaskInit(void)
{
    int i;
    if (gThreadMutex) {
        threadsLogWarning("thread task has init.\n");
        return ;
    }

    gThreadMutex = ThreadMutexCreate( );
    for (i = 0; i < THREAD_TASK_NUMBER_MAX; i++) {
        gThreadsPool[i].threadNo = -1;
    }
    gThreadIndex = 0;

    return ;
}



int ThreadTaskCreateCommon(const char *threadName, ThreadFunction_Type threadFunc, void *args)
{
    int tIndex;

    if (gThreadMutex == NULL) {
        threadsLogError("thread task not init.\n");
        return -1;
    }

    if (ThreadMutexLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");
    if (gThreadIndex >= THREAD_TASK_NUMBER_MAX) {
        ThreadMutexUnLock(gThreadMutex);
        threadsLogError("thread task not init [%d], THREAD_TASK_NUMBER_MAX[%d], threadName[%s]\n", gThreadIndex, THREAD_TASK_NUMBER_MAX, threadName);
        return -1;
    }
    tIndex = gThreadIndex;
    gThreadIndex++;
    if (ThreadMutexUnLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");

    if (pthread_create(&gThreadsPool[tIndex].threadNo, NULL, (pthread_routine)threadFunc, args)) {
        threadsLogError("pthread create error.\n");
        return -1;
    }
    sprintf(gThreadsPool[tIndex].threadName, "%s", threadName);
    threadsLogInfo("ThreadTask Create Common thread, [%s] in gThreadsPool position [%d]\n ", threadName, tIndex);

    return 0;
}




int ThreadTaskCreateAdvanced(const char *threadName, ThreadFunction_Type threadFunc, void *args)
{
    int tIndex;

    if (gThreadMutex == NULL) {
        threadsLogError("thread task not init.\n");
        return -1;
    }

    if (ThreadMutexLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");
    if (gThreadIndex >= THREAD_TASK_NUMBER_MAX) {
        ThreadMutexUnLock(gThreadMutex);
        threadsLogError("thread task not init [%d], THREAD_TASK_NUMBER_MAX[%d], threadName[%s]\n", gThreadIndex, THREAD_TASK_NUMBER_MAX, threadName);
        return -1;
    }
    tIndex = gThreadIndex;
    gThreadIndex++;
    if (ThreadMutexUnLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");

    pthread_attr_t attrThread;
    struct sched_param param;
    pthread_attr_init(&attrThread);
    pthread_attr_setdetachstate(&attrThread, PTHREAD_CREATE_DETACHED);/** 设置线程为非绑定，不是必须的 **/
    pthread_attr_setschedpolicy(&attrThread, SCHED_FIFO);  /** 设置线程的调度策略为SCHED_FIFO先进先出 **/
    pthread_attr_getschedparam(&attrThread, &param);   /** 获取线程的调度参数 **/
    param.sched_priority = (sched_get_priority_max(SCHED_FIFO) + sched_get_priority_min(SCHED_FIFO)) / 2;
    pthread_attr_setschedparam(&attrThread, &param);
    //pthread_attr_setstacksize(&attrThread, stack_size); /**设置堆栈大小**/

    if (pthread_create(&gThreadsPool[tIndex].threadNo, &attrThread, (pthread_routine)threadFunc, args)) {
        threadsLogError("pthread create error.\n");
        return -1;
    }
    sprintf(gThreadsPool[tIndex].threadName, "%s", threadName);
    threadsLogInfo("ThreadTask Create Advance thread, [%s] in gThreadsPool position [%d]\n ", threadName, tIndex);

    return 0;
}


int ThreadPoolShow(void)
{
    int i;
    int tIndex;

    if (gThreadMutex == NULL) {
        threadsLogError("thread task not init.\n");
        return -1;
    }
    if (ThreadMutexLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");
    if (gThreadIndex >= THREAD_TASK_NUMBER_MAX || gThreadIndex <= 0) {
        ThreadMutexUnLock(gThreadMutex);
        threadsLogError("thread task gThreadIndex[%d] is error, THREAD_TASK_NUMBER_MAX[%d]\n", gThreadIndex, THREAD_TASK_NUMBER_MAX);
        return -1;
    }
    tIndex = gThreadIndex;
    if (ThreadMutexUnLock(gThreadMutex))
        threadsLogWarning("thread mutex locking error.\n");

    for (i = 0; i <= tIndex; i++) {
        threadsLogInfo("gThreadsPool index[%d], threadNo[%d], threadName[%s]\n", i, gThreadsPool[i].threadNo, gThreadsPool[i].threadName);
    }

    return 0;
}



