//funstack.c  
#define _GNU_SOURCE  
#include <memory.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <signal.h>  
#include <ucontext.h>  
#include <dlfcn.h>  
#include <execinfo.h>  

#if defined(REG_RIP)  
    #define SIGSEGV_STACK_IA64  
    #define REGFORMAT "%016lx"  
#elif defined(REG_EIP)  
    #define SIGSEGV_STACK_X86  
    #define REGFORMAT "%08x"  
#else  
    #define SIGSEGV_STACK_GENERIC  
    #define REGFORMAT "%x"  
#endif  


static void signal_segv(int signum, siginfo_t* info, void*ptr) {  
    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};  
    size_t i;  
    ucontext_t *ucontext = (ucontext_t*)ptr;  
#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)  
    int f = 0;  
    Dl_info dlinfo;  
    void **bp = 0;  
    void *ip = 0;  
#else  
    void *bt[20];  
    char **strings;  
    size_t sz;  
#endif  


#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)  
# if defined(SIGSEGV_STACK_IA64)  
    ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];  
    bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];  
# elif defined(SIGSEGV_STACK_X86)  
    ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];  
    bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];  
# endif  

    fprintf(stderr, "Stack trace:/n");  
    while(bp && ip) {  
        if(!dladdr(ip, &dlinfo))  
            break;  
        const char *symname = dlinfo.dli_sname;  
        fprintf(stderr, "% 2d: %p %s+%u (%s)/n",  
                    ++f,  ip,  symname,  
                    (unsigned)(ip - dlinfo.dli_saddr),  
                    dlinfo.dli_fname);  
        if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))  
            break;  
        ip = bp[1];  
        bp = (void**)bp[0];  
    }  
#else  
    fprintf(stderr, "Stack trace (non-dedicated):/n");  
    sz = backtrace(bt, 20);  
    strings = backtrace_symbols(bt, sz);  
    for(i = 0; i < sz; ++i)  
            fprintf(stderr, "%s/n", strings[i]);  
#endif  
    fprintf(stderr, "End of stack trace/n");  
    
    
    return;  
}  
int setup_sigsegv() {  
    struct sigaction action;  
    
    memset(&action, 0, sizeof(action));  
    action.sa_sigaction = signal_segv;  
    action.sa_flags = SA_SIGINFO;  
    if(sigaction(SIGUSR1, &action, NULL) < 0) {  
        perror("sigaction");  
        return 0;  
    }  
    
    return 1;  
}  
  
void func1()  
{  
    raise(SIGUSR1);  
    
    return ;  
}  
void func2()  
{  
    raise(SIGUSR1);  
    
    return ;  
}  
void entry()  
{  
    func1();  
    func2();  
    
    return;  
}  
int main()  
{  
    setup_sigsegv();  
    entry();  
    
    return 0;
}  