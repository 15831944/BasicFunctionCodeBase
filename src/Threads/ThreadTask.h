#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__


#define THREAD_TASK_NUMBER_MAX      64
#define THREAD_NAME_LENGTH  64



#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ThreadFunction_Type)(void *arg);

void ThreadTaskInit(void);

int ThreadTaskCreateCommon(const char* threadName, ThreadFunction_Type threadFunc, void* args);
int ThreadTaskCreateAdvanced(const char* threadName, ThreadFunction_Type threadFunc, void* args);

int ThreadPoolShow(void);


#ifdef __cplusplus
}
#endif


#endif  //__THREAD_TASK_H__

