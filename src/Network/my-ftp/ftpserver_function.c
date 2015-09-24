#include "ftpserver_function.h"






/** 输出到客户端 */
extern int AckResponse(const int connectFd, const int AckCode, const char* AckPromote);

/** 将buf以空格做分隔符分解成两个字符串 */
void explode(char *buf, char **cmd, char **param)
{
    printf("%s\n", param);
    return ;
}

/**
 *  HELP <command>	    
 *  返回指定命令信息
 **/
void Help(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    
    if (!param) {
        AckResponse(connectFd, ACK_214, " The following commands are recognized (* =>'s unimplemented).");
        AckResponse(connectFd, ACK_214, "   USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   ");
        AckResponse(connectFd, ACK_214, "   QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    ");
        AckResponse(connectFd, ACK_214, "   STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR    ");
        AckResponse(connectFd, ACK_214, "   DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    ");
        AckResponse(connectFd, ACK_214, "   SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    ");
        AckResponse(connectFd, ACK_214, " Direct comments to nolove@263.net");
    } else {
        AckResponse(ACK_214, " Sorry, I haven't write the topic help.");
    }

    return ;
}



//使用ini文件存储用户名密码，只要用户登录，将用户信息贮存在内存结构体链表中



typedef struct Node{
    char username[32];
    char passwd[32];
    int loginStatus;  //控制登陆流程：0-input username, 1-input passwd, 2-login
    char userInfo[64];
    struct Node * pNext; //指针域
} UserInfo, * pUserInfo;

UserInfo* g_userListHead = NULL;


//boolen
int g_userHasLoginFlag = 0;
int g_userAnonymousLoginFlag = 0;
int g_inputUserFlag = 0;


//TODO:: 修改为 用户信息结构体链表
char username[100];

/**
 *  USER <username>	
 *  系统登录的用户名 
 **/
void User(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    
    
    if(g_userHasLoginFlag == 1) {
        AckResponse(connectFd, ACK_503, "Bad sequence of commands: You are already logged in!");
        return ;
    }
    if(!param){
        AckResponse(connectFd, ACK_500, "Syntax error, command unrecognized: 'USER' command requires a parameter.");
        printf("\n");
        return ;
    }
    
    strncpy(username, param, 100);
    //TODO::比较ini配置文件中username
    //TODO::添加到全局列表
    if (strcasecmp(username, "anonymous") == 0) {
        g_userAnonymousLoginFlag = 1;
        strncpy(username, "ftp", 100);
        AckResponse(connectFd, ACK_331, "Anonymous login ok, send your complete e-mail address as password.");
    } else {
        char tmpBuf = {};
        sprintf(tmpBuf, "Password required for %s.", username);
        AckResponse(connectFd, ACK_331, "Anonymous login ok, send your complete e-mail address as password.");
    }

    g_inputUserFlag = 1;
    
    return ;
}

char path[PATH_MAX];
/**
 *  PASS <password>	    
 *  系统登录密码
 **/
void Pass(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    
    if (!g_inputUserFlag) {
        AckResponse(connectFd, ACK_503, "Bad sequence of commands: Login with USER first.");
        return;
    }
    char tmpBuf[BUFSIZE] = {};
    //读取存放在文件中的 username=passwd 键值对，；
    
    // 解密密码：
    DecryptCommand(param, EncryptMode_Unencrypt, tmpBuf);
    
    struct passwd *pw;
    struct spwd *spw;
    char *passwd, salt[13];

    /* judge and chdir to its home directory */
    if (!g_userAnonymousLoginFlag) {
        if (g_sysUid != 0) {
            AckResponse(connectFd, ACK_530, "Not logged in: Login incorrect.");
            g_inputUserFlag = 0;
            return;
        }
        
        //get current user info;
        // get passwd
        
        //从用户信息结构体链表中 获取用户登录相关信息 并判断
        //
        
        
        
        if ((pw = getpwnam(username)) == NULL) {
            AckResponse(connectFd, ACK_530, "Not logged in: Login incorrect.");
            g_inputUserFlag = 0;
            return;
        }
        passwd = pw->pw_passwd;
        if (passwd == NULL || strcmp(passwd, "x") == 0) {
            spw = getspnam(username);
            if (spw == NULL || (passwd = spw->sp_pwdp) == NULL) {
                AckResponse(connectFd, ACK_530, "Not logged in: Login incorrect.");
                g_inputUserFlag = 0;
                return;
            }
        }
        strncpy(salt, passwd, 12);
        if (strcmp(passwd, crypto((const char *)param, (const char *)salt)) != 0) {
            AckResponse(connectFd, ACK_530, "Not logged in: Login incorrect.");
            g_inputUserFlag = 0;
            return;
        }
        strcpy(path, "");
        setuid(pw->pw_uid);
        if (pw->pw_dir)
            strncpy(path, pw->pw_dir, PATH_MAX);
        else
            strcpy(path, "/");
        
        
        char tmpBuf[32] = {};
        sprintf(tmpBuf, "User logged in, proceed: User %s logged in.", username);
        AckResponse(connectFd, ACK_230, tmpBuf);
        
        
        chdir(path);
        getcwd(path, PATH_MAX);
        g_userHasLoginFlag = 1;

    } else {
        if ((pw = getpwuid(g_sysUid)) == NULL) {
            AckResponse(connectFd, ACK_530, "Not logged in: Login incorrect.");
            g_inputUserFlag = 0;
            return;
        }
        if (pw->pw_dir)
            strncpy(basedir, pw->pw_dir, PATH_MAX);
        else
            strcpy(basedir, "");

        strcpy(path, "/");
        chdir(basedir);
        getcwd(basedir, PATH_MAX);
        g_userHasLoginFlag = 1;
        AckResponse(connectFd, ACK_230, "User logged in, proceed: Anonymous access granted, restrictions apply.");
    }
    g_userHasLoginFlag = 1;
    AckResponse(connectFd, ACK_230, "User logged in, proceed: Anonymous access granted, restrictions apply.");

    
    return ;
}

/**
 *  PWD	                
 *  显示当前工作目录
 **/
void Pwd(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  CWD  <dir path>	    
 *  改变服务器上的工作目录
 **/
void Cwd(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  CDUP <dir path> 	
 *  改变服务器上的父目录
 **/
void Cdup(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  MKD  <directory>	
 *  在服务器上建立指定目录
 **/
void Mkd(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  RMD  <directory>	
 *  在服务器上删除指定目录
 **/
void Rmd(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  TYPE <data type>	
 *  数据类型（A=ASCII，E=EBCDIC，I=binary）
 **/
void Type(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  
 **/
void Size(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  STAT <directory>	
 *  在当前程序或目录上返回信息
 **/
void Stat(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  DELE <filename>	   
 *   删除服务器上的指定文件
 **/
void Dele(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  QUIT	            
 *  从 FTP 服务器上退出登录
 **/
void Quit(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  NOOP	            
 *  无动作，除了来自服务器上的承认
 **/
void Noop(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  LIST <name>	        
 *  如果是文件名列出文件信息，如果是目录则列出文件列表
 **/
void List(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  NLST <directory>	
 *  列出指定目录内容
 **/
void Nlst(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  RETR <filename>	    
 *  从服务器上找回（复制）文件
 **/
void Retr(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  STOR <filename> 	
 *  储存（复制）文件到服务器上
 **/
void Stor(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  APPE <filename>	    
 *  添加文件到服务器同名文件
 **/
void Appe(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/** 
 *  ABOR
 *  中断数据连接程序
 **/
void Abor(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  
 **/
void Mdtm(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  SYST	            
 *  返回服务器使用的操作系统
 **/
void Syst(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  REST <offset>	    
 *  由特定偏移量重启文件传递
 **/
void Rest(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  RNFR <old path>	    
 *  对旧路径重命名
 **/
void Rnfr(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  RNTO <new path>	    
 *  对新路径重命名
 **/
void Rnto(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  SITE <params>	    
 *  由服务器提供的站点特殊参数
 **/
void Site(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  PORT <address>	    
 *  IP 地址和两字节的端口 ID
 **/
void Port(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  PASV	            
 *  请求服务器等待数据连接
 **/
void Pasv(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}


/**
 *  STRU <type>	        
 *  数据结构（F=文件，R=记录，P=页面）
 **/
void Stru(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

/**
 *  MODE <mode>	        
 *  传输模式（S=流模式，B=块模式，C=压缩模式）
 **/
void Mode(int connectFd, char *param, void* reserved)
{
    printf("%s\n", param);
    return ;
}

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
 








