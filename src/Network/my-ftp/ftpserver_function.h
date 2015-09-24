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
#ifndef __FTPSERVER_FUNCTIONS_H__
#define __FTPSERVER_FUNCTIONS_H__

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
#define PATH_MAX        256

int user_valid; //boolean

char path[PATH_MAX]; //set
unsigned int file_rest; //set
int system_uid; //set
int pasv_mode ;  //boolean  //set get
int connfd;
int pasvfd;
FILE *file;
int data_pid ; //set get
FILE *data_file;

struct _port {
        uint32_t host;
        uint16_t port;
};
struct _port  local_port;


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
};


/** 输出到客户端 */
void outs(char *fmt,...);

/** 将buf以空格做分隔符分解成两个字符串 */
void explode(char *buf, char **cmd, char **param);

/**
 *  HELP <command>	    
 *  返回指定命令信息
 **/
void Help(char *param);

/**
 *  USER <username>	
 *  系统登录的用户名 
 **/
void User(char *param);

/**
 *  PASS <password>	    
 *  系统登录密码
 **/
void Pass(char *param);

/**
 *  PWD	                
 *  显示当前工作目录
 **/
void Pwd(char *param);

/**
 *  CWD  <dir path>	    
 *  改变服务器上的工作目录
 **/
void Cwd(char *param);

/**
 *  CDUP <dir path> 	
 *  改变服务器上的父目录
 **/
void Cdup(char *param);

/**
 *  MKD  <directory>	
 *  在服务器上建立指定目录
 **/
void Mkd(char *param);

/**
 *  RMD  <directory>	
 *  在服务器上删除指定目录
 **/
void Rmd(char *param);

/**
 *  TYPE <data type>	
 *  数据类型（A=ASCII，E=EBCDIC，I=binary）
 **/
void Type(char *param);

/**
 *  
 **/
void Size(char *param);

/**
 *  STAT <directory>	
 *  在当前程序或目录上返回信息
 **/
void Stat(char *param);

/**
 *  DELE <filename>	   
 *   删除服务器上的指定文件
 **/
void Dele(char *param);

/**
 *  QUIT	            
 *  从 FTP 服务器上退出登录
 **/
void Quit(char *param);

/**
 *  NOOP	            
 *  无动作，除了来自服务器上的承认
 **/
void Noop(char *param);

/**
 *  LIST <name>	        
 *  如果是文件名列出文件信息，如果是目录则列出文件列表
 **/
void List(char *param);

/**
 *  NLST <directory>	
 *  列出指定目录内容
 **/
void Nlst(char *param);

/**
 *  RETR <filename>	    
 *  从服务器上找回（复制）文件
 **/
void Retr(char *param);

/**
 *  STOR <filename> 	
 *  储存（复制）文件到服务器上
 **/
void Stor(char *param);

/**
 *  APPE <filename>	    
 *  添加文件到服务器同名文件
 **/
void Appe(char *param);

/** 
 *  ABOR
 *  中断数据连接程序
 **/
void Abor(char *param);

/**
 *  
 **/
void Mdtm(char *param);

/**
 *  SYST	            
 *  返回服务器使用的操作系统
 **/
void Syst(char *param);

/**
 *  REST <offset>	    
 *  由特定偏移量重启文件传递
 **/
void Rest(char *param);

/**
 *  RNFR <old path>	    
 *  对旧路径重命名
 **/
void Rnfr(char *param);

/**
 *  RNTO <new path>	    
 *  对新路径重命名
 **/
void Rnto(char *param);

/**
 *  SITE <params>	    
 *  由服务器提供的站点特殊参数
 **/
void Site(char *param);

/**
 *  PORT <address>	    
 *  IP 地址和两字节的端口 ID
 **/
void Port(char *param);

/**
 *  PASV	            
 *  请求服务器等待数据连接
 **/
void Pasv(char *param);


/**
 *  STRU <type>	        
 *  数据结构（F=文件，R=记录，P=页面）
 **/
void Stru(char *param);

/**
 *  MODE <mode>	        
 *  传输模式（S=流模式，B=块模式，C=压缩模式）
 **/
void Mode(char *param);

/**
 *  STOU <filename>	    
 *  储存文件到服务器名称上
 **/

 
/**
 *   REIN	            
 *  重新初始化登录状态连接
 **/
 
 
/**
 *   SMNT <pathname>	    
 *  挂载指定文件结构
 **/

 
/**
 *   ALLO <bytes>	    
 *  为服务器上的文件存储器分配字节
 **/
 
 
/**
 *   ACCT <account>	    
 *  系统特权帐号
 **/
 
 
 
#endif  //__FTPSERVER_FUNCTIONS_H__



