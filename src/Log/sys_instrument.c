/***************************************************************
 *
 *  死机堆栈打印模块。。。。。
 *  不用任何初始化，编译时加上-finstrument-functions参数即可。
 *  生成的可执行程序不要strip。
 *  
 *  由于动态库的加载基址不确定。。因此不支持库内符号查找。。
 *  如果死在库里，请用core dump。
 *
 *  死机后会输出一个pid一个tid。按照线程号(tid)查找调用即可。
 *
 *  这个模块本身的打印由环境变量控制。开打印需设置 FF_LOGLEVEL=1
 *
 *  Author: wuhanyong
 *  Date:   2011-12-19
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <elf.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_TRACE_THREADS     100
#define NOFF    __attribute__((__no_instrument_function__))
#define PRINTF(X, ...)\
    do {                                \
        if (log_level) {                \
            log_printf(__FILE__, __LINE__, __func__, X, ##__VA_ARGS__);\
        }                               \
    } while(0)
#define GETTID()    syscall(SYS_gettid)

void  _dump(const char * buffer, int size);
void NOFF Trace_Out(void);
void NOFF __cyg_profile_func_enter(void * this_func, void * call_site);
void NOFF __cyg_profile_func_exit(void * this_func, void * call_site);
static void NOFF log_printf(const char * file, int line, const char * func, const char * fmt, ...);
static void NOFF handle_sig(int sig, siginfo_t * info, void * arg);
static int              log_level   = 0;
static int              inited      = 0;
static pthread_mutex_t  ffmutex;

typedef struct  _DEBUGNODE
{
    struct _DEBUGNODE   *   prev;
    struct _DEBUGNODE   *   next;
    void                *   func;
    void                *   call_site;
}DEBUGNODE;

static struct _NODEHEADS
{
    pthread_t       tid;
    DEBUGNODE   *   head;
    DEBUGNODE   *   tail;
}nodes[MAX_TRACE_THREADS];

static void NOFF init(void)
{
    inited = 1;
    pthread_mutex_init(&ffmutex, NULL);

    char    *   env = getenv("FF_LOGLEVEL");
    if(env != NULL)
    {
        log_level = atoi(env);
    }else{
        log_level = 0;
    }
    memset(&nodes, 0, sizeof(nodes));
    PRINTF("INIT FF!\n");

    struct  sigaction   sa;
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_handler   = SIG_DFL;
    sa.sa_sigaction = handle_sig;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    //signal(SIGINT, sighandler);
}

static void NOFF log_printf(const char * file, int line, const char * func, const char * fmt, ...)
{
    if(!inited)
        init();
    va_list arglist;
    if(file == NULL || func == NULL || fmt == NULL)
    {
        return;
    }
    const char *  p = strrchr(file, '/');
    if(p == NULL)
    {
        p = file;
    }else{
        p ++;
    }

    time_t  t       = time(NULL);
    struct tm * ctm = localtime(&t);

    flock(1, F_LOCK, 0);
    printf("<FF> [%02d:%02d:%02d] [%s:%05d:%s]  ", ctm->tm_hour, ctm->tm_min, 
        ctm->tm_sec, p, line, func);
    va_start(arglist, fmt);
    vfprintf(stdout, fmt, arglist);
    va_end(arglist);
    flock(1, F_ULOCK, 0);
}

static void NOFF add_node(int threadid, void * func, void * call_site) 
{
    //PRINTF("threadid = %ld, func = %p, call_site = %p\n", threadid, func, call_site);
    int     i;
    int     index = -1;
    for(i = 0; i < MAX_TRACE_THREADS; i ++)
    {
        if(nodes[i].tid == 0 && index == -1)
        {
            index = i;
        }
        if(nodes[i].tid == threadid)
        {
            index = i;
            break;
        }
    }
    
    if(index == -1)
    {
        PRINTF("Max trace threads got. no more trace.\n");
        return;
    }

    DEBUGNODE   *   node = malloc(sizeof(DEBUGNODE));
    if(node == NULL)
    {
        PRINTF("malloc failed.\n");
        return;
    }
    memset(node, 0, sizeof(DEBUGNODE));
    node->func      = func;
    node->call_site = call_site;
    node->prev      = NULL;
    node->next      = NULL;

    nodes[index].tid = threadid;
    if(nodes[index].head == NULL)
    {
        nodes[index].head = node;
    }else{
        nodes[index].tail->next = node;
        node->prev = nodes[index].tail;
    }
    nodes[index].tail = node;

}

static void NOFF del_node(int threadid, void * func, void * call_site) 
{
//    PRINTF("threadid = %ld, func = %p, call_site = %p\n", threadid, func, call_site);
    int     i;
    for(i = 0; i < MAX_TRACE_THREADS; i ++)
    {
        if(nodes[i].tid == threadid)
        {
            break;
        }
    }
    
    if(i >= MAX_TRACE_THREADS)
    {
        PRINTF("No such thread.\n");
        return;
    }

    DEBUGNODE   *   node = nodes[i].tail;
    if(node == NULL)
    {
        printf("stack is destroyed!\n");
        Trace_Out();
        return;
    }
    if(node->func != nodes[i].tail->func)
    {
        printf("stack is destroyed!\n");
        Trace_Out();
        return;
    }

    node = node->prev;
    free(nodes[i].tail);
    nodes[i].tail = node;
    if(node == NULL)
    {
        nodes[i].head   = NULL;
        nodes[i].tid    = 0;
    }else{
        node->next      = NULL;
    }

}

static void NOFF handle_sig(int sig, siginfo_t * info, void * arg)
{
    PRINTF("Get signal %d\n", sig);
    _dump(info, sizeof(siginfo_t));
    Trace_Out();
    exit(0);
}

#if 0
static void NOFF sighandler(int sig)
{
    printf("Get signal %d\n", sig);
    Trace_Out();
    exit(0);
}
#endif

static const char * NOFF   addr2func(void * addr)
{
    if(addr == NULL)
        return NULL;
    static char     *   buffer = NULL;
    Elf32_Ehdr      *   ehdr;
    Elf32_Shdr      *   shdr;
    Elf32_Sym       *   sym;
    int                 len;
    FILE            *   fp;
    int                 bytes_read;
    int                 i;
    char            *   p;
    int                 SymCount;
    int                 SymIndex;
    int                 StrIndex;
    if(buffer == NULL)
    {
        fp = fopen("/proc/self/exe", "rb");
        if(fp == NULL)
        {
            perror("fopen");
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        PRINTF("file length = %d bytes.\n", len);
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(len);
        if(buffer == NULL)
        {
            fclose(fp);
            return NULL;
        }
        bytes_read = fread(buffer, len, 1, fp);
        fclose(fp);
        PRINTF("bytes_read = %d.\n", bytes_read);
    }

    ehdr        = (Elf32_Ehdr *)buffer;
    shdr        = (Elf32_Shdr *)(buffer + ehdr->e_shoff);
    p           = buffer + shdr[ehdr->e_shstrndx].sh_offset;
    SymIndex    = -1;
    StrIndex    = -1;
    for(i=0; i<ehdr->e_shnum; i++)
    {
        PRINTF("sym: %s\n", p + shdr[i].sh_name);
        if(shdr[i].sh_type == SHT_SYMTAB && strncmp(p + shdr[i].sh_name, ".symtab", 7) == 0)
        {
            SymIndex = i;
        }
        if(shdr[i].sh_type == SHT_STRTAB && strncmp(p + shdr[i].sh_name, ".strtab", 7) == 0)
        {
            StrIndex = i;
        }
    }
    if(SymIndex == -1)
    {
        PRINTF("No symbol table.\n");
        return NULL;
    }
    if(StrIndex == -1)
    {
        PRINTF("No string table.\n");
        return NULL;
    }

    sym         = (Elf32_Sym *)(buffer + shdr[SymIndex].sh_offset);
    p           = (char *)(buffer + shdr[StrIndex].sh_offset);
    SymCount    = shdr[SymIndex].sh_size / sizeof(Elf32_Sym);

    for(i=0; i<SymCount; i++)
    {
        if(sym[i].st_name != 0 && (Elf32_Addr)addr == sym[i].st_value && (sym[i].st_info & 0xf) != STB_LOCAL)
        {
            return p + sym[i].st_name;
        }
    }
    return NULL;
}


///////////////////////////////////////////////////////////////////////
//  
//  下面是导出的函数。
//

void NOFF Trace_Out(void)
{

    lockf(1, F_LOCK, 0);
    lockf(2, F_LOCK, 0);
    int             i;
    DEBUGNODE   *   node;
    printf("------------------TRACE OUT------------------\n");
    printf("caller: pid = %lu, tid = %lu\n", getpid(), GETTID());
    for(i=0; i<MAX_TRACE_THREADS; i++)
    {
        if(nodes[i].tid != 0)
        {
            printf("[TRACE] thread %lu:\n", nodes[i].tid);
            node = nodes[i].head;
            while(node != NULL)
            {
                printf("[TRACE] call_site: %08x\tfunc: %08x\t sym: %s\n", (unsigned int)node->call_site, (unsigned int)node->func, addr2func(node->func));
                node = node->next;
            }
        }
    }
    printf("------------------TRACE END------------------\n");
    unlink("/var/ffatom");
    lockf(1, F_ULOCK, 0);
    lockf(2, F_ULOCK, 0);
}

void NOFF __cyg_profile_func_enter(void * this_func, void * call_site) 
{
    if(!inited) init();
    //PRINTF("func = %p, callsite = %p\n", this_func, call_site);
    pthread_mutex_lock(&ffmutex);
    int tid = GETTID();
    add_node(tid, this_func, call_site);
    pthread_mutex_unlock(&ffmutex);
}

void NOFF __cyg_profile_func_exit(void * this_func, void * call_site)
{
    if(!inited) init();
    //PRINTF("func = %p, callsite = %p\n", this_func, call_site);
    int tid = GETTID();
    pthread_mutex_lock(&ffmutex);
    del_node(tid, this_func, call_site);
    pthread_mutex_unlock(&ffmutex);
}

#if 0
void ff_trace_signal(int sig)
{
    struct  sigaction   sa;
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_handler   = SIG_DFL;
    sa.sa_sigaction = handle_sig;
    sigaction(sig, &sa, 0);
//    sigaction(SIGSEGV, &sa, 0);
//    signal(SIGINT, sighandler);
}
#endif

void  _dump(const char * buffer, int size)
{
    int         i, j, t;
    for(i=0; i<size; i+=16)
    {
        printf("%08x: ", i);
        t = 16;
        if(i + 16 > size)
            t = size - i;
        for(j=0; j<t; j++)
        {
            printf("%02x ", buffer[i+j] & 0xff);
        }
        for(j=t; j<16; j++)
        {
            printf("   ");
        }
        printf("\t");
        for(j=0; j<t; j++)
        {
            if(buffer[i+j] >= 36 && buffer[i+j] <= 126 || buffer[i+j] == '"')
                printf("%c", buffer[i+j] & 0xff);
            else
                printf(".");
        }
        printf("\n");
    }
    return;
}

