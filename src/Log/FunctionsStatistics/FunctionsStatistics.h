/***************************************************************
 *
 *  函数性能统计模块。。。。。
 *  不用任何初始化(需要初始化的东西会在main()函数执行之前完成），
 *  编译时加上-finstrument-functions参数即可。
 *  生成的可执行程序不要strip。(strip后无法通过函数地址查找函数名)
 *  
 *  由于动态库的加载基址不确定。。因此不支持库内符号查找。。
 *
 *  调用ShowStatistics后会输出各函数执行次数与平均花费时间，
 *  以微秒计数。可通过环境变量FF_THRESHOLD来设置统计阈值。
 *
 *  与上次的死机堆栈打印模块无法一起使用。
 *
 *  使用这个功能后平均性能会比不使用时低一些，因为在每个函数执行
 *  时都会做统计。
 *
 **************************************************************/

/* 贴一部分输出样例：
 * function performance statistics: (threshold 1000 us.)
 * -----------------------------------------------------------
 *   tid      times      average time(us)    function
 *  2060        1               8668      Jvm_Main_Running
 *  2060       18              55791      TAKIN_Proc_Key
 *  2060        1               1015      _ZN5Hippo17HippoContextHWC109ioctlReadERNS_7HStringES2_
 *  2060        3               2417      _ZN5Hippo17HippoContextHWC1010ioctlWriteERNS_7HStringES2_
 *  2060        1         2723133737      mid_sem_take
 *  2060        1               1166      mid_net4_staticip_connect
 *  2060        1               1181      mid_net4_connect
 *  2060        1               1186      mid_net_connect
 *  2060        1               1564      app_aes_decrypt
 *  2060        1               1654      app_distinguish_root_picture
 *  2060        1               1573      sys_AESpassword_check
 *  2060        1               1226      customer_config_load
 *  2060        2              20052      _GLOBAL__I_SystemManager.cpp
 *  2060       18              55842      _ZN5Hippo17BrowserAgentTakin13handleMessageEPNS_7MessageE
 *  2060        2              42384      _ZN5Hippo17BrowserAgentTakin7openUrlEPKc
 *  2060        2               2242      TAKIN_browser_updateScreen
 *  2060       13              20970      _ZN5Hippo16LayerMixerDevice7refreshEv
 *  2060        2               2059      _ZN5Hippo16LayerMixerDevice11setStandardENS0_5Layer8StandarE
 *  2060        1               1603      logModuleInit
 *  2060      603              20087      _ZN5Hippo12MessageQueue4nextEv
 *  2060       32              40054      _ZN5Hippo14MessageHandler15dispatchMessageE
 *  2060       14              19726      _ZN5Hippo19NativeHandlerPublic13handleMessageEPNS_7MessageE
 *  2060        1               8286      _ZN5Hippo20NativeHandlerBootC10C1Ev
 *  2060        3              22883      _ZN5Hippo23NativeHandlerRunningC1013handleMessageEPNS_7MessE
 *  2060       14              19736      _ZN5Hippo22NativeHandlerPublicC1013handleMessageEPNS_7MessaE
 *  2060        3               1911      a_Hippo_Port_JseParamWriteC10
 *  2060        1               1459      aes
 *  ...(略)
 * -----------------------------------------------------------
 */
 
/**
*
    编译时需加上 -lpthread -g -finstrument-functions 参数；
    例如：  g++ -Wall main.cpp functions_performance.cpp -lpthread -g -finstrument-functions -o test.elf
*
**/
#ifndef __FUNCTIONS_PERFORMANCE_H__
#define __FUNCTIONS_PERFORMANCE_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>

#define NOFF    __attribute__((__no_instrument_function__))
#define GETTID()    syscall(SYS_gettid)

typedef long long __int64;

extern "C" void * NOFF malloc(size_t size);
extern "C" void NOFF free(void *ptr);
extern "C" void NOFF __cyg_profile_func_enter(void * this_func, void * call_site);
extern "C" void NOFF __cyg_profile_func_exit(void * this_func, void * call_site);
extern "C" void NOFF ShowStatistics(char * output, int len);

/*-----------------------------------------------------------*/
/*                     Class    Init              */
/*----------------------------------------------------------*/
class Init {
public:
    Init() NOFF;
    ~Init() NOFF;
    int     threshold;
};

/*-----------------------------------------------------------*/
/*                  Class    ThreadLocalStorage            */
/*----------------------------------------------------------*/
class ThreadLocalStorage {
public:
    ThreadLocalStorage() NOFF;
    ~ThreadLocalStorage() NOFF;
    void Set(void* value) NOFF;
    void* Get() NOFF;
private:
    pthread_key_t   key_;
};

/*-----------------------------------------------------------*/
/*                     Class    Stack              */
/*----------------------------------------------------------*/
class Stack {
public:
    Stack() NOFF;
    ~Stack() NOFF;
    void Push(void * func, long long tm) NOFF;
    void Pop(void*& func, long long& tm) NOFF;
    void Print(void) NOFF;
private:
    struct Data {
        void * func;
        long long tm;
        Data* next;
        Data* prev;
    } *first, *last;
};

/*----------------------------------------------------------*/
/*                     Class    Lock              */
/*---------------------------------------------------------*/
class Lock {
public:
    Lock() NOFF;
    ~Lock() NOFF;
    void lock(void) NOFF;
    void unlock(void) NOFF;
private:
    pthread_mutex_t mutex;
};

typedef struct tagResult {
    void * func;
    long long tm;
    tagResult* next;
    tagResult* prev;
} Result;

typedef struct tagResultLink {
    unsigned int tid;
    Result* result;
    tagResultLink* prev;
    tagResultLink* next;
} ResultLink;


#endif
