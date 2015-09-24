
/*      Writen by Pacific, 2000/11/23   
Support command:
        214 The following commands are recognized (* =>'s unimplemented).
        214     USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   
        214     QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    
        214     STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR   
        214     DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    
        214     SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    
        214 Direct comments to nolove@263.net
*/
#ifndef __FTPD_H__
#define __FTPD_H__

#define _XOPEN_SOURCE
#include <shadow.h>
#include <unistd.h>
#include <crypt.h>
#include <pwd.h>
#include <time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <utime.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <malloc.h>
#include <sys/file.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>


#define BUFSIZE         (1024)
#define ERRS(x)         outs("550 %s: " x, param)
#define PATH_MAX        256

//buffer and string 
char inbuf[BUFSIZE];
char genbuf[BUFSIZE];
char hostname[BUFSIZE];
char path[PATH_MAX];
char rename_file[PATH_MAX];
char username[100];
char basedir[PATH_MAX];

//system arguments
unsigned int ftp_port = 21;
unsigned int max_conn = 65535;
unsigned int timeout = 300;
unsigned int file_rest = 0;

char transfer_type='i';
int system_uid;

//boolean
int user_valid = 0;
int input_user = 0;
int anonymous_login = 0;
int pasv_mode = 0;
int transfer = 0;


int listenfd;
int connfd;
int pasvfd;
FILE *file;
int data_pid = 0;
FILE *data_file;


#define NO_CHECK                1
#define NEED_PARAM              2
#define NO_PARAM                4
#define CHECK_LOGIN             8
#define CHECK_NOLOGIN           16
#define NO_TRANSFER             32


struct _cmd_list {
        char *cmd;
        void (*func)(char *param);
        int check;
};

struct _port {
        uint32_t host;
        uint16_t port;
} remote_port, local_port;

int port_base = 3072;



#endif  //__FTPD_H__
