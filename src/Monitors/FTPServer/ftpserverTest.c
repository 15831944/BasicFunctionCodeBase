#include <stdio.h>

#include "Log/LogC.h"
#include "logMonitor.h"
#include "Monitors.h"
#include "includes/ServerMain.h"

extern int log_monitor_init();
extern int ServerMain(int argc, char** argv);
int main (int argc, char** argv)
{
    int i = 0;
    
    printf("===========================main===============\n");
    log_monitor_init();

    printf("main param:: argc[%d], argv[%p].\n", argc, argv);
    for(i = 0; i < argc; i++){
        printf("param:: the [%d] param is [%s].\n", i, argv[i]);
    }
    
    ServerMain(argc, argv);

    return 0;
}




//实现一个后台进程需要完成一系列的工作，
//包括：关闭所有的文件描述字；改变当前工作目录；重设文件存取屏蔽码(umask) ；在后台执行；脱离进程组；忽略终端I/O信号；脱离控制终端。

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int daemonize(void)
{
        int fd;
        switch(fork()) {
                case -1:
                        return (-1);
                case 0:
                        break;
                default:
                        //将父进程结束,让子进程变成真正的孤儿进程,并被init进程接管
                        exit(0);
        }
        //子进程成为新的会话组长和新的进程组长，并与原来的登录会话和进程组脱离
        setsid();

        if ((fd = open("daemon.log", O_CREAT|O_RDWR|O_APPEND, 0)) != -1) {
                //dup2(int oldhandle, int newhandle)复制文件句柄
                dup2(fd, STDIN_FILENO);
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                //0,1和2文件句柄分别与标准输入,标准输出,标准错误输出相关联
                //所以用户应用程序调用open函数打开文件时,默认都是以3索引为开始句柄
                //fd已经由新的句柄代替,关闭fd句柄
                if (fd > STDERR_FILENO) (void)close(fd);
        }
        return 0;
}

int test(int argc, char *argv[])
{
        daemonize();
        printf("%s\n","hello");
        return 0;
}
