#ifndef __THREAD_MUTEX_H__
#define __THREAD_MUTEX_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThreadMutex* ThreadMutex_Type;

ThreadMutex_Type ThreadMutexCreate(void);

int ThreadMutexLocking(ThreadMutex_Type mutex, const char* file, const char* func, int line);
int ThreadMutexUnLocking(ThreadMutex_Type mutex, const char* file, const char* func, int line);

#define ThreadMutexLock(mutex) ThreadMutexLocking(mutex,__FILE__,__FUNCTION__,__LINE__)
#define ThreadMutexUnLock(mutex) ThreadMutexUnLocking(mutex,__FILE__,__FUNCTION__,__LINE__)


#ifdef __cplusplus
}
#endif

#endif //__THREAD_MUTEX_H__

