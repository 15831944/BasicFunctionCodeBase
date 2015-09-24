/*      Writen by Pacific, 2000/11/23   
Support command:
        214 The following commands are recognized (* =>'s unimplemented).
        214     USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   
        214     QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    
        214     STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR   
        214     DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    
        214     SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    
        214 Direct comments to nolove@263.net
        
命令	描述
//未实现功能
ACCT <account>	    系统特权帐号
ALLO <bytes>	    为服务器上的文件存储器分配字节
SMNT <pathname>	    挂载指定文件结构
REIN	            重新初始化登录状态连接
STRU <type>	        数据结构（F=文件，R=记录，P=页面）
MODE <mode>	        传输模式（S=流模式，B=块模式，C=压缩模式）
STOU <filename>	    储存文件到服务器名称上

//已实现功能
ABOR	            中断数据连接程序
APPE <filename>	    添加文件到服务器同名文件
CDUP <dir path> 	改变服务器上的父目录
CWD  <dir path>	    改变服务器上的工作目录
DELE <filename>	    删除服务器上的指定文件
HELP <command>	    返回指定命令信息
LIST <name>	        如果是文件名列出文件信息，如果是目录则列出文件列表
MKD  <directory>	在服务器上建立指定目录
NLST <directory>	列出指定目录内容
NOOP	            无动作，除了来自服务器上的承认
PASS <password>	    系统登录密码
PASV	            请求服务器等待数据连接
PORT <address>	    IP 地址和两字节的端口 ID
PWD	                显示当前工作目录
QUIT	            从 FTP 服务器上退出登录
REST <offset>	    由特定偏移量重启文件传递
RETR <filename>	    从服务器上找回（复制）文件
RMD  <directory>	在服务器上删除指定目录
RNFR <old path>	    对旧路径重命名
RNTO <new path>	    对新路径重命名
SITE <params>	    由服务器提供的站点特殊参数
STAT <directory>	在当前程序或目录上返回信息
STOR <filename> 	储存（复制）文件到服务器上
SYST	            返回服务器使用的操作系统
TYPE <data type>	数据类型（A=ASCII，E=EBCDIC，I=binary）
USER <username>>	系统登录的用户名        
    
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
#define ERROUTS(x)         outs("550 %s: " x, param)
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


///响应代码	解释说明
enum _AckCode{
    ACK_110	= 110, //   新文件指示器上的重启标记
    ACK_120 = 120, //	服务器准备就绪的时间（分钟数）
    ACK_125	= 125, //   打开数据连接，开始传输
    ACK_150 = 150, //	打开连接
    ACK_200 = 200, //	成功
    ACK_202 = 202, //	命令没有执行
    ACK_211 = 211, //	系统状态回复
    ACK_212,  //	目录状态回复
    ACK_213,  //	文件状态回复
    ACK_214,  //	帮助信息回复
    ACK_215,  //	系统类型回复
    ACK_220 = 220, //	服务就绪
    ACK_221,  //	退出网络
    ACK_225 = 225,  //	打开数据连接
    ACK_226,  //	结束数据连接
    ACK_227,  //	进入被动模式（IP 地址、ID 端口）
    ACK_230 = 230, //	登录因特网
    ACK_250 = 250, //	文件行为完成
    ACK_257 = 257, //	路径名建立
    ACK_331 = 331, //	要求密码
    ACK_332,  //	要求帐号
    ACK_350 = 350, //	文件行为暂停
    ACK_421 = 421, //	服务关闭
    ACK_425 = 425, //	无法打开数据连接
    ACK_426,  //	结束连接
    ACK_450 = 450, //	文件不可用
    ACK_451,  //	遇到本地错误
    ACK_452,  //	磁盘空间不足
    ACK_500 = 500, //	无效命令
    ACK_501,  //	错误参数
    ACK_502,  //	命令没有执行
    ACK_503,  //	错误指令序列
    ACK_504,  //	无效命令参数
    ACK_530 = 530, //	未登录网络
    ACK_532 = 532, //	存储文件需要帐号
    ACK_550 = 550, //	文件不可用
    ACK_551,  //	不知道的页类型
    ACK_552,  //	超过存储分配
    ACK_553,  //	文件名不允许
}


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
