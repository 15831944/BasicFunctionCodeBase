
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ThreadMutex.h"
#include "LogThreads.h"


struct ThreadMutex {
    pthread_mutex_t id;
} ;


ThreadMutex_Type ThreadMutexCreate(void)
{
    ThreadMutex_Type mutex;

    mutex = (ThreadMutex_Type)malloc(sizeof(struct ThreadMutex));
    if (mutex == NULL) {
        threadsLogError("thread mutex malloc error.\n");
        return NULL;
    }
    if (pthread_mutex_init(&mutex->id, NULL)) {
        free(mutex);
        threadsLogError("thread mutex init error.\n");
        return NULL;
    }

    return mutex;
}


int ThreadMutexLocking(ThreadMutex_Type mutex, const char *file, const char *func, int line)
{
    if (mutex == NULL) {
        threadsLogError("thread mutex error, location[%s:%s::%d]\n", file, func, line);
        return -1;
    }
    pthread_mutex_lock(&mutex->id);

    return 0;
}


int ThreadMutexUnLocking(ThreadMutex_Type mutex, const char *file, const char *func, int line)
{
    if (mutex == NULL) {
        threadsLogError("thread mutex error, location[%s:%s::%d]\n", file, func, line);
        return -1;
    }
    pthread_mutex_unlock(&mutex->id);

    return 0;
}








