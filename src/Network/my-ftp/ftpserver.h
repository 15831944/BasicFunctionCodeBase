
/**
 *  按功能分组的响应代码：
200 Command okay. （命令 OK）
500 Syntax error, command unrecognized. （语法错误，命令不能被识别）可能包含因为命令行太长的错误。
501 Syntax error in parameters or arguments. （参数语法错误）
202 Command not implemented, superfluous at this site. （命令没有实现，对本站点冗余）
502 Command not implemented. （命令没有实现）
503 Bad sequence of commands. （命令顺序错误）
504 Command not implemented for that parameter. （没有实现这个命令参数）


110 Restart marker reply. （重新开始标记响应）对于这种情况，文本应该是明确的，无需进行特殊实现；必须形如：MARK yyyy = mmmm ； yyyy 是用户进程数据流标记，mmmm服务器的等效标记（注意，标记间的空格和“=“）
211 System status, or system help reply. （系统状态，或者系统帮助响应。）
212 Directory status. （目录状态）
213 File status. （文件状态）
214 Help message. （帮助信息）关于如何使用服务器，或者特殊的非标准的命令的意义。只对人类用户有用。
215 NAME system type. （系统类型名称）这里的 NAME 指在 Assigned Numbers 文档中列出的正式名称。


120 Service ready in nnn minutes. （服务将在 nnn 分钟后准备完成）
220 Service ready for new user. （接受新用户服务准备完成）
221 Service closing control connection. （服务关闭控制连接）已注消
421 Service not available, closing control connection. （服务不可用，关闭控制连接）如果服务器知道它必须关闭，应该以 421 作为任何命令的响应代码。


125 Data connection already open; transfer starting. （数据连接已打开，传输开始）
225 Data connection open; no transfer in progress. （数据连接打开，没有传输）
425 Can't open data connection. （不能打开数据连接）
226 Closing data connection. （关闭数据连接）请求文件动作成功（例如，文件传输或者放弃）
426 Connection closed; transfer aborted. （连接关闭，放弃传输）


227 Entering Passive Mode (h1,h2,h3,h4,p1,p2). （进入被动模式）
230 User logged in, proceed. (用户成功登录，继续）
530 Not logged in. （没有登录成功）


331 User name okay, need password. （用户名 OK，需要密码）
332 Need account for login. （需要帐户才能登录）
532 Need account for storing files. （需要帐户来存储文件）


150 File status okay; about to open data connection. （文件状态 OK，将打开数据连接）
250 Requested file action okay, completed. （请求文件动作 OK，完成）
257 "PATHNAME" created. （创建了“PATHNAME”）
350 Requested file action pending further information. （请求文件动作需要进一步的信息）
450 Requested file action not taken. （请求文件动作没有执行）文件不可使用（例如，文件忙）
550 Requested action not taken. (请求的动作没有执行） 文件不可用 （例如， 没有找到文件 ，没有访问权限）


451 Requested action aborted. Local error in processing. （请求动作放弃，处理中发生本地错误）
551 Requested action aborted. Page type unknown. （请求动作放弃，未知的页面类型）


452 Requested action not taken. （请求动作未执行）系统存储空间不足。
552 Requested file action aborted. (请求文件动作被放弃)超出存储分配空间 （当前的路径或者数据集）
553 Requested action not taken. （请求动作未获得）文件名不允许。
 *  
 *  
 *  
 *  
 **/


struct _cmd_list cmd_list[] = {
    {"USER",    User,   CHECK_NOLOGIN | NEED_PARAM  },
    {"PASS",    Pass,   CHECK_NOLOGIN   },
    {"ACCT"},
    {"CWD",     Cwd,    CHECK_LOGIN | NEED_PARAM    },
    {"XCWD",    Cwd,    CHECK_LOGIN | NEED_PARAM    },
    {"CDUP",    Cdup,   CHECK_LOGIN | NO_PARAM      },
    {"XCUP",    Cdup,   CHECK_LOGIN | NO_PARAM      },
    {"SMNT"},
    {"QUIT",    Quit,   NO_CHECK    },
    {"REIN"},
    {"PORT",    Port,   CHECK_LOGIN | NEED_PARAM    },
    {"PASV",    Pasv,   CHECK_LOGIN | NO_PARAM      },
    {"TYPE",    Type,   CHECK_LOGIN | NEED_PARAM    },
    {"STRU",    Stru,   CHECK_LOGIN | NEED_PARAM    }, //add own
    {"MODE",    Mode,   CHECK_LOGIN | NEED_PARAM    }, //add own
    {"RETR",    Retr,   CHECK_LOGIN | NEED_PARAM    },
    {"STOR",    Stor,   CHECK_LOGIN | NEED_PARAM    },
    {"STOU"},
    {"APPE",    Appe,   CHECK_LOGIN | NEED_PARAM    },
    {"ALLO"},
    {"REST",    Rest,   CHECK_LOGIN | NEED_PARAM    },
    {"RNFR",    Rnfr,   CHECK_LOGIN | NEED_PARAM    },
    {"RNTO",    Rnto,   CHECK_LOGIN | NEED_PARAM    },
    {"ABOR",    Abor,   CHECK_LOGIN | NO_PARAM      },
    {"DELE",    Dele,   CHECK_LOGIN | NEED_PARAM    },
    {"MDTM",    Mdtm,   CHECK_LOGIN | NEED_PARAM    },
    {"RMD",     Rmd,    CHECK_LOGIN | NEED_PARAM    },
    {"XRMD",    Rmd,    CHECK_LOGIN | NEED_PARAM    },
    {"MKD",     Mkd,    CHECK_LOGIN | NEED_PARAM    },
    {"XMKD",    Mkd,    CHECK_LOGIN | NEED_PARAM    },
    {"PWD",     Pwd,    CHECK_LOGIN | NO_PARAM      },
    {"XPWD",    Pwd,    CHECK_LOGIN | NO_PARAM      },
    {"SIZE",    Size,   CHECK_LOGIN | NEED_PARAM    },
    {"LIST",    List,   CHECK_LOGIN    },
    {"NLST",    List,   CHECK_LOGIN    },
    {"SITE",    Site,   CHECK_LOGIN | NEED_PARAM    },
    {"SYST",    Syst,   CHECK_LOGIN },
    {"STAT",    Stat,   CHECK_LOGIN | NEED_PARAM    },
    {"HELP",    Help,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {""}
};





#include <stdio.h> 
#include <signal.h> 
#include <sys/file.h> 
int main(int argc,char **argv)
{
    time_t now;
    int childpid,fd,fdtablesize;
    int error,in,out;
    
    /* 忽略终端 I/O信号,STOP信号 */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN); 
    signal(SIGHUP, SIG_IGN);
    
    /* 父进程退出,程序进入后台运行 */
    if(fork() != 0) 
        exit(1);
    if(setsid() < 0)
        exit(1);/* 创建一个新的会议组 */ 
    
    /* 子进程退出,孙进程没有控制终端了 */  
    if(fork() != 0) 
        exit(1);
    if(chdir("/tmp") == -1)
        exit(1);
    
    /* 关闭打开的文件描述符,包括标准输入、标准输出和标准错误输出 */ 
    /* getdtablesize():返回所在进程的文件描述附表的项数，即该进程打开的文件数目 */
    for (fd = 0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++) 
        close(fd);
    
    umask(0);/*重设文件创建掩模 */ 
    signal(SIGCHLD, SIG_IGN);/* 忽略SIGCHLD信号 */ 
    
    /*打开log系统*/
    syslog(LOG_USER | LOG_INFO, "守护进程测试!\n");  
    while(1)  
    {  
        time(&now);
        syslog(LOG_USER | LOG_INFO, "当前时间:t%stt\n", ctime(&now));
        sleep(6);
    } 
    
    return ;
 }


