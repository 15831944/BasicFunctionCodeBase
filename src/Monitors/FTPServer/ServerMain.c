
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include "includes/Conf.h"
#include "includes/Errors.h"
#include "includes/Common.h"
#include "includes/Networking.h"
#include "includes/ServerMain.h"



// initialize configuration
struct config configuration;
int signaled = 0;

void sig_handler(int s)
{
    signaled = 1;
    return ;
}

int ServerMain(int argc, char **argv)
{
    struct sigaction s_action;
    char cwd[PATH_MAX];
    
    // register signal handler
    sigemptyset(&s_action.sa_mask);
    s_action.sa_handler = sig_handler;
    s_action.sa_flags = 0;
    sigaction(SIGINT, &s_action, NULL);

    readConfiguration(&configuration, argc, argv);
    printConfiguration(&configuration);
    if (getuid()) {
        fprintf(stderr, "Run with root privileges to chroot.\n");
        exit(1);
    }
    if (getcwd(cwd, PATH_MAX) == NULL) {
        err(1, "Failed to get current directory name.");
    }
    if (chroot(cwd) == -1) {
        err(1, "Change root failed.");
    }
    //setgid(UID);
    //if ( setuid(UID) == -1) {
    //    err(1, "Failed to set to desired UID.");
    //}
    // create a socket and start listening
    printf("Starting the server, quit with Ctrl + C\n");
    startServer(&configuration);
    
    return (0);
}



//////////////////////////////////////////////////////////////////////////
#if 0
int main (int argc, char** argv)
{
    printf("===========================main===============\n");

    printf("main param:: argc[%d], argv[%p].\n", argc, argv);
    for(int i = 0; i < argc; i++){
        printf("param:: the [%d] param is [%s].\n", i, argv[i]);
    }
    
    ServerMain(argc, argv);

    return 0;
}




#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/param.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>

void init_daemon()
{
    int pid;
    int i;
    
    pid = fork();
    if(pid < 0)    
        exit(1);  //创建错误，退出
    else if(pid > 0) //父进程退出
        exit(0);
        
    setsid(); //使子进程成为组长
    pid = fork();
    if(pid > 0)
        exit(0); //再次退出，使进程不是组长，这样进程就不会打开控制终端
    else if(pid < 0)    
        exit(1);

    //关闭进程打开的文件句柄
    for(i = 0; i < NOFILE; i++)
        close(i);
    chdir("/root/test");  //改变目录
    umask(0);//重设文件创建的掩码
    
    return;
}


void main_deamon()
{
    FILE *fp;
    time_t t;
    init_daemon();
    while(1)
    {
        sleep(60); //等待一分钟再写入
        fp=fopen("testfork2.log","a");
        if(fp>=0)
        {
            time(&t);
            fprintf(fp,"current time is:%s\n",asctime(localtime(&t)));  //转换为本地时间输出
            fclose(fp);
        }
    }
    return;
}







#endif