
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <elf.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <typeinfo>

#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <utility>

#include "FunctionsStatistics.h"



static int loglevel = 0;
#define PRINTF(X, ...)\
    do {                                \
        if (loglevel) {                \
            log_printf(__FILE__, __LINE__, __func__, X, ##__VA_ARGS__);\
        }                               \
    } while(0)

#define RUN_HERE() PRINTF("Run here: %s:%d\n", __func__, __LINE__)

static void NOFF log_printf(const char *file, int line, const char *func, const char *fmt, ...);
static const char *NOFF addr2func(void *addr);
static void NOFF AddResult(void *func, long long tm);

static bool enable_record = true;

static void log_printf(const char *file, int line, const char *func, const char *fmt, ...)
{
    /*{{{*/
    va_list arglist;
    if (file == NULL || func == NULL || fmt == NULL) {
        return;
    }
    const char   *p = strrchr(file, '/');
    if (p == NULL) {
        p = file;
    } else {
        p ++;
    }

    time_t  t       = time(NULL);
    struct tm *ctm = localtime(&t);

    lockf(1, F_LOCK, 0);
    printf("<FF> [%02d:%02d:%02d] [%s:%05d:%s]  ", ctm->tm_hour, ctm->tm_min, ctm->tm_sec, p, line, func);
    va_start(arglist, fmt);
    vfprintf(stdout, fmt, arglist);
    va_end(arglist);
    lockf(1, F_ULOCK, 0);
}  /*}}}*/

static const char *addr2func(void *addr)
{
    /*{{{*/
    if (addr == NULL)
        return NULL;
    static char        *buffer = NULL;
    Elf32_Ehdr         *ehdr;  /* The ELF file header.  This appears at the start of every ELF file.  */ /* ELF Header数据结构 */
    Elf32_Shdr         *shdr;  /* Section header.  */ /* 节区头部数据结构 */
    Elf32_Sym          *sym;  /* Symbol table entry.  */ /* 符号表项格式定义 */
    int                 len;
    FILE               *fp;
    int                 bytes_read;
    int                 i;
    char               *p;
    int                 SymCount;
    int                 SymIndex;
    int                 StrIndex;


    if (buffer == NULL) {
        fp = fopen("/proc/self/exe", "rb");
        if (fp == NULL) {
            perror("fopen");
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        PRINTF("file length = %d bytes.\n", len);
        fseek(fp, 0, SEEK_SET);
        buffer = (char *)malloc(len);
        if (buffer == NULL) {
            fclose(fp);
            return NULL;
        }
        bytes_read = fread(buffer, len, 1, fp);
        fclose(fp);
        PRINTF("bytes_read = %d.\n", bytes_read);
    }

    ehdr        = (Elf32_Ehdr *)buffer;
    shdr        = (Elf32_Shdr *)(buffer + ehdr->e_shoff);  /* Section header table file offset */
    p           = buffer + shdr[ehdr->e_shstrndx].sh_offset;   /* Section header string table index */   /* Section file offset */
    SymIndex    = -1;
    StrIndex    = -1;
    for (i = 0; i < ehdr->e_shnum; i++) { /* Section header table entry count */
        PRINTF("sym: %s\n", p + shdr[i].sh_name);  /* Section name (string tbl index) */
        if (shdr[i].sh_type == SHT_SYMTAB && strcmp(p + shdr[i].sh_name, ".symtab") == 0) {  /* Section type */
            SymIndex = i;
        }
        if (shdr[i].sh_type == SHT_STRTAB && strcmp(p + shdr[i].sh_name, ".strtab") == 0) {
            StrIndex = i;
        }
    }
    if (SymIndex == -1) {
        PRINTF("No symbol table.\n");
        return NULL;
    }
    if (StrIndex == -1) {
        PRINTF("No string table.\n");
        return NULL;
    }

    sym         = (Elf32_Sym *)(buffer + shdr[SymIndex].sh_offset);
    p           = (char *)(buffer + shdr[StrIndex].sh_offset);
    SymCount    = shdr[SymIndex].sh_size / sizeof(Elf32_Sym);

    for (i = 0; i < SymCount; i++) {
        if (sym[i].st_name != 0 && (Elf32_Addr)addr == sym[i].st_value && (sym[i].st_info & 0xf) != STB_LOCAL) {
            /* Symbol name (string tbl index) */ /* Symbol value */ /* Symbol type and binding */
            return p + sym[i].st_name;
        }
    }
    return NULL;
}/*}}}*/


/*-----------------------------------------------------------------------------------------------------*/
/*                     Class    Init              */
/*-----------------------------------------------------------------------------------------------------*/
Init::Init()
    : threshold(0)
{
    enable_record = false;
    char *p = getenv("FF_THRESHOLD");
    printf("p = %s\n", p);
    if (p) {
        printf("atoi(p) = %d\n", atoi(p));
        threshold = atoi(p);
    }
    enable_record = true;
}

Init::~Init()
{
}

/*-----------------------------------------------------------------------------------------------------*/
/*                        Class  ThreadLocalStorage               */
/*-----------------------------------------------------------------------------------------------------*/
ThreadLocalStorage::ThreadLocalStorage()
{
    int error = pthread_key_create(&key_, NULL);
    if (error) {
        key_ = (pthread_key_t)0;
    }
}
ThreadLocalStorage::~ThreadLocalStorage()
{
    pthread_key_delete(key_);
}
void ThreadLocalStorage::Set(void *value)
{
    pthread_setspecific(key_, value);
}
void *ThreadLocalStorage::Get()
{
    return pthread_getspecific(key_);
}


/*-----------------------------------------------------------------------------------------------------*/
/*                        Class  Stack               */
/*-----------------------------------------------------------------------------------------------------*/
Stack::Stack()
    : first(NULL)
    , last(NULL)
{
}

Stack::~Stack()
{
    Data *temp = first;
    while (temp) {
        first = temp->next;
        free(temp);
        temp = first;
    }
    first = NULL;
    last = NULL;
}

void Stack::Push(void *func, long long tm)
{
    Data *temp = (Data *)malloc(sizeof(Data));
    temp->func = func;
    temp->tm = tm;
    temp->next = NULL;
    temp->prev = last;
    if (!first) {
        first = temp;
    } else {
        last->next = temp;
    }
    last = temp;
}

void Stack::Pop(void *&func, long long &tm)
{
    if (last == NULL)
        return;

    func = last->func;
    tm = last->tm;

    Data *temp = last->prev;
    free(last);

    last = temp;
    if (temp == NULL) {
        first = NULL;
    }

}

void Stack::Print(void)
{
    Data *temp = first;
    int index = 1;
    while (temp != NULL) {
        printf("%d. %s\n", index++, addr2func(temp->func));
        temp = temp->next;
    }
}

/*-----------------------------------------------------------------------------------------------------*/
/*                        Class  Lock               */
/*-----------------------------------------------------------------------------------------------------*/
Lock::Lock()
{
    pthread_mutex_init(&mutex, NULL);
}

Lock::~Lock()
{
}

void Lock::lock(void)
{
    pthread_mutex_lock(&mutex);
}

void Lock::unlock(void)
{
    pthread_mutex_unlock(&mutex);
}



/*-----------------------------------------------------------------------------------------------------*/
/*                        functions Statistics                     */
/*-----------------------------------------------------------------------------------------------------*/
static ThreadLocalStorage first_result;
static ThreadLocalStorage last_result;
static ResultLink *first_link = NULL;
static ResultLink *last_link = NULL;
static Lock         link_lock;
static Init         initer;

static void AddResult(void *func, long long tm)
{
    /*{{{*/
    if (tm <= initer.threshold)
        return;
    Result *first = static_cast<Result *>(first_result.Get());
    Result *last = static_cast<Result *>(last_result.Get());
    Result *temp = (Result *)malloc(sizeof(Result));
    temp->func = func;
    temp->tm = tm;
    temp->prev = last;
    temp->next = NULL;
    if (!first) {
        first = temp;
        first_result.Set(first);

        ResultLink *t = (ResultLink *)malloc(sizeof(ResultLink));
        t->result = first;
        t->next = NULL;
        t->prev = last_link;
        t->tid = GETTID();

        link_lock.lock();
        if (!first_link) {
            first_link = t;
            last_link = t;
        } else {
            last_link->next = t;
        }
        last_link = t;
        link_lock.unlock();
    } else {
        last->next = temp;
    }
    last = temp;
    last_result.Set(last);
}/*}}}*/

void ShowStatistics(char *output, int len)
{
    enable_record = false;
    // link_lock.lock();
    std::stringstream   oss;
    int count = 0;
    oss << "function performance statistics: (threshold: " << initer.threshold << " us.)" << std::endl;
    oss << ("-----------------------------------------------------------") << std::endl;
    oss << (" tid      times      average time(us)    function") << std::endl;
    ResultLink *temp = first_link;
    while (temp != NULL) {
        Result *res = temp->result;

        std::map<void *, std::vector<long long> > m;
        m.clear();

        while (res != NULL) {
            m[res->func].push_back(res->tm);
            res = res->next;
        }
        int k = 0;

        std::map<void *, std::vector<long long> >::iterator  iter;
        for (iter = m.begin(); iter != m.end(); iter++) {

            printf("====k[%d]\n", k);
            std::vector<long long>  v = iter->second;
            std::vector<long long>::iterator    it;
            long long sum = 0;
            for (it = (iter->second).begin(); it != (iter->second).end(); it++) {
                sum += *it;
            }
            oss << std::setw(6) << temp->tid
                << "   "
                << std::setw(6) << iter->second.size()
                << "         "
                << std::setw(10) << sum / iter->second.size()
                << "      " << addr2func(iter->first) << std::endl;
            count ++;
        }
        temp = temp->next;
    }
    oss << count << " functions." << std::endl;
    oss << ("-----------------------------------------------------------") << std::endl;
    // lockf(1, F_LOCK, 0);
    if (output) {
        snprintf(output, len, "%s", oss.str().c_str());
    }
    printf("------%s", oss.str().c_str());
    fflush(stdout);
    // lockf(1, F_ULOCK, 0);
    // link_lock.unlock();
    enable_record = true;
}

static ThreadLocalStorage tls;
void __cyg_profile_func_enter(void *this_func, void *call_site)
{
    if (!this_func)
        return;

    if (!enable_record)
        return;
    Stack *pStack = static_cast<Stack *>(tls.Get());
    if (!pStack) {
        pStack = new Stack();
        tls.Set(pStack);
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    pStack->Push(this_func, tv.tv_sec * 1000000 + tv.tv_usec);
}

void __cyg_profile_func_exit(void *this_func, void *call_site)
{
    if (!this_func)
        return;
    if (!enable_record)
        return;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    Stack *p = static_cast<Stack *>(tls.Get());
    if (p) {
        void *func = NULL;
        long long tm = 0;
        p->Pop(func, tm);
        if (func != this_func) {
            printf("func = [%s], this_func = [%s]\n", addr2func(func), addr2func(this_func));
            printf("?????\n");
            p->Print();
            exit(0);
        }
        printf("function %s, time: %lld us.\n", addr2func(func), tv.tv_sec * 1000000 + tv.tv_usec - tm);
        AddResult(func, tv.tv_sec * 1000000 + tv.tv_usec - tm);
    }
}


