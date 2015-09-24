/**
 *  Support command:
        214 The following commands are recognized (* =>'s unimplemented).
        214     USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   
        214     QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    
        214     STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR   
        214     DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    
        214     SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    
        214 Direct comments to nolove@263.net
 **/
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
 **/

 
 ///响应代码	        解释说明
enum _AckCode{
    ACK_110	= 110,  //   新文件指示器上的重启标记              110 Restart marker reply. （重新开始标记响应）对于这种情况，文本应该是明确的，无需进行特殊实现；必须形如：MARK yyyy = mmmm ； yyyy 是用户进程数据流标记，mmmm服务器的等效标记（注意，标记间的空格和“=“）
    ACK_120 = 120,  //	服务器准备就绪的时间（分钟数）         120 Service ready in nnn minutes. （服务将在 nnn 分钟后准备完成）
    ACK_125	= 125,  //   打开数据连接，开始传输                125 Data connection already open; transfer starting. （数据连接已打开，传输开始）
    ACK_150 = 150,  //	打开连接          150 File status okay; about to open data connection. （文件状态 OK，将打开数据连接）
    ACK_200 = 200,  //	成功              200 Command okay. （命令 OK）
    ACK_202 = 202,  //	命令没有执行      202 Command not implemented, superfluous at this site. （命令没有实现，对本站点冗余）
    ACK_211 = 211,  //	系统状态回复      211 System status, or system help reply. （系统状态，或者系统帮助响应。）
    ACK_212,        //	目录状态回复      212 Directory status. （目录状态）
    ACK_213,        //	文件状态回复      213 File status. （文件状态）
    ACK_214,        //	帮助信息回复      214 Help message. （帮助信息）关于如何使用服务器，或者特殊的非标准的命令的意义。只对人类用户有用。
    ACK_215,        //	系统类型回复      215 NAME system type. （系统类型名称）这里的 NAME 指在 Assigned Numbers 文档中列出的正式名称。
    ACK_220 = 220,  //	服务就绪          220 Service ready for new user. （接受新用户服务准备完成）
    ACK_221,        //	退出网络          221 Service closing control connection. （服务关闭控制连接）已注消
    ACK_225 = 225,  //	打开数据连接      225 Data connection open; no transfer in progress. （数据连接打开，没有传输）
    ACK_226,        //	结束数据连接      226 Closing data connection. （关闭数据连接）请求文件动作成功（例如，文件传输或者放弃）
    ACK_227,        //	进入被动模式      227 Entering Passive Mode (h1,h2,h3,h4,p1,p2). （进入被动模式）（IP 地址、ID 端口）
    ACK_230 = 230,  //	登录因特网        230 User logged in, proceed. (用户成功登录，继续）
    ACK_250 = 250,  //	文件行为完成      250 Requested file action okay, completed. （请求文件动作 OK，完成）
    ACK_257 = 257,  //	路径名建立        257 "PATHNAME" created. （创建了“PATHNAME”）
    ACK_331 = 331,  //	要求密码          331 User name okay, need password. （用户名 OK，需要密码）
    ACK_332,        //	要求帐号          332 Need account for login. （需要帐户才能登录）
    ACK_350 = 350,  //	文件行为暂停      350 Requested file action pending further information. （请求文件动作需要进一步的信息）
    ACK_421 = 421,  //	服务关闭          421 Service not available, closing control connection. （服务不可用，关闭控制连接）如果服务器知道它必须关闭，应该以 421 作为任何命令的响应代码。
    ACK_425 = 425,  //	无法打开数据连接  425 Can't open data connection. （不能打开数据连接）
    ACK_426,        //	结束连接          426 Connection closed; transfer aborted. （连接关闭，放弃传输）
    ACK_450 = 450,  //	文件不可用        450 Requested file action not taken. （请求文件动作没有执行）文件不可使用（例如，文件忙）
    ACK_451,        //	遇到本地错误      451 Requested action aborted. Local error in processing. （请求动作放弃，处理中发生本地错误）
    ACK_452,        //	磁盘空间不足      452 Requested action not taken. （请求动作未执行）系统存储空间不足。
    ACK_500 = 500,  //	无效命令          500 Syntax error, command unrecognized. （语法错误，命令不能被识别）可能包含因为命令行太长的错误。
    ACK_501,        //	错误参数          501 Syntax error in parameters or arguments. （参数语法错误）
    ACK_502,        //	命令没有执行      502 Command not implemented. （命令没有实现）
    ACK_503,        //	错误指令序列      503 Bad sequence of commands. （命令顺序错误）
    ACK_504,        //	无效命令参数      504 Command not implemented for that parameter. （没有实现这个命令参数）
    ACK_530 = 530,  //	未登录网络        530 Not logged in. （没有登录成功）
    ACK_532 = 532,  //	存储文件需要帐号  532 Need account for storing files. （需要帐户来存储文件）
    ACK_550 = 550,  //	文件不可用        550 Requested action not taken. (请求的动作没有执行） 文件不可用 （例如， 没有找到文件 ，没有访问权限）
    ACK_551,        //	不知道的页类型    551 Requested action aborted. Page type unknown. （请求动作放弃，未知的页面类型）
    ACK_552,        //	超过存储分配      552 Requested file action aborted. (请求文件动作被放弃)超出存储分配空间 （当前的路径或者数据集）
    ACK_553,        //	文件名不允许      553 Requested action not taken. （请求动作未获得）文件名不允许。
};


 
 
/**
 *  FTP client端发送的命令格式：
        USER <SP> <username> <CRLF>
        PASS <SP> <password> <CRLF>
        ACCT <SP> <account-information> <CRLF>
        CWD <SP> <pathname> <CRLF>
        CDUP <CRLF>
        SMNT <SP> <pathname> <CRLF>
        QUIT <CRLF>
        REIN <CRLF>
 *  
 **/

 
 
#define DEFAULT_USERNAME            "anounse"
#define DEFAULT_USERPASSWD          "123456" 
#define DEFAULT_MAX_CONNECT_COUNT   32767 
#define DEFAULT_SERVERPORT          21 
#define DEFAULT_TIMEOUT             300

typedef struct _options{
    char userName[32]; 
    char userPasswd[32]; 
    int serverPort;
    int maxConnectCount; 
    int timeOut;  //unit: seconds
} Option;

/**
 *  从命令行输入和配置文件中解析配置参数
 *  只支持简单的格式解析，如[-p:port]
 *      sample: ftp_server -p:port -m:max_connect_count -t:time_out
 **/
int ParseOptions(const char** buf, const int count, Option* options)
{
    int i, okFlag;
    
    if(buf == NULL || count == 0){
        printf("ParseOptions, param options buf is null error.\n");
        return -1;
    }
    
    /* set option default */
    sprintf(options->userName, "%s", DEFAULT_USERNAME);
    sprintf(options->userPasswd, "%s", DEFAULT_USERPASSWD);
    options->serverPort = DEFAULT_SERVERPORT;
    options->maxConnectCount = DEFAULT_MAX_CONNECT_COUNT;
    options->timeOut = DEFAULT_TIMEOUT;

    okFlag = 1;
    /* get the param */
    for (i = 1; i < count; i++) {
        if (buf[i][0] != '-' || buf[i][2] != ':')
            okFlag = 0;
        switch (buf[i][1]) {
        case 'p':
            options->serverPort = atoi(&buf[i][3]);
            if (options->serverPort == 0)
                okFlag = 0;
            break;
        case 'm':
            options->maxConnectCount = atoi(&buf[i][3]);
            if (options->maxConnectCount == 0)
                okFlag = 0;
            break;
        case 't':
            options->timeOut = atoi(&buf[i][3]);
            if (options->timeOut == 0)
                okFlag = 0;
            break;
        default:
            okFlag = 0;
        }
        if (!okFlag)
            break;
    }

    if (!okFlag) {
        printf("Usage: %s [-p:port] [-m:maxconn] [-t:timeout]ntport: default is 21.ntmaxconn: the max client num connected.n", argv[0]);
        return -1;
    }

    return 0;
}
 
 

 
#include <sys/socket.h>
/**
 *  服务器应答输出
 *      应答格式：
 *      sample: "225 Data connection open; no transfer in progress."
 **/
int AckResponse(const int connectFd, const int AckCode, const char* AckPromote)
{
    char tmp[1024] = {};
    
    sprintf(tmp, "%d %s\r\n", AckCode, AckPromote);
    if (send(connectFd, tmp, strlen(tmp), 0) == SOCKET_ERROR) {
        printf("AckResponse,send acknowledgement error.\n");
        return -1;
    }
    
    return 0;
}



/**
 *  错误处理函数，用于命令出错处理
 **/
void ErrorHandler(int connectFd)
{
    printf("ErrorHandler:errno=[%d].\n", errno);
    switch (errno) {
    case ENOTEMPTY:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Directory not empty");
        break;
    case ENOSPC:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Disk full");
        break;
    case EEXIST:
        AckResponse(connectFd, ACK_550, "Requested action not taken. File exists");
        break;
    case ENAMETOOLONG:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Path is too long");
        break;
    case ENOENT:
        AckResponse(connectFd, ACK_550, "Requested action not taken. No such file or directory");
        break;
    case ENOTDIR:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Not a directory");
        break;
    case EISDIR:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Is a directory");
        break;
    default:
        AckResponse(connectFd, ACK_550, "Requested action not taken. Permission denied");
    }

    return ;
} 


//boolen type
int userValid = 0;
/**
 *  命令校验
 **/
int CommandVerification(const char* cmd, const char* param, const int check, const int connectFd)
{
    if (check & NO_CHECK) {
        printf("Verification, NO_CHECK.\n");
        return 1;
    }
    
    if (check & CHECK_LOGIN && !userValid) {
        AckResponse(connectFd, ACK_530, "Please login with USER and PASS");
        return 0;
    }

    if (check & CHECK_NOLOGIN && userValid) {
        AckResponse(connectFd, ACK_503, "You are already logged in!");
        return 0;
    }

    if (check & NEED_PARAM) {
        if (!param)
            AckResponse(connectFd, ACK_501, "Invalid number of arguments, check more arguments.");
        return (param != NULL);
    }

    if (check & NO_PARAM) {
        if (param)
            AckResponse(connectFd, ACK_501, "Invalid number of arguments.");
        return (param == NULL);
    }
    
    return 1;
}



#define BUFSIZE         (1024)

void SignalChldHandler()
{
    printf("SIGCHLD handler.\n");
    return ;
}


void SignalTermHandler()
{
    printf("SIGTERM handler.\n");
    
    return ;
}



#include <stdio.h> 
#include <signal.h> 
#include <sys/file.h> 
int SetProgramDaemon(int sysUid)  /* set the program a daemon */
{
    int fd;
    
    /* 父进程退出,程序进入后台运行 */
    if(fork() != 0) 
        exit(1);
    
    /* 创建一个新的会议组 */ 
    if(setsid() < 0)
        exit(1);
    
    /* 关闭打开的文件描述符,包括标准输入、标准输出和标准错误输出 */ 
    /* getdtablesize():返回所在进程的文件描述附表的项数，即该进程打开的文件数目 */
    for (fd = 0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++) 
        close(fd);
    
    open("/dev/null", O_RDONLY);
    dup2(stdin, stdout);  // 0: stdin;  1: stdout
    dup2(stdin, stderr);  // 2: stderr
    
    if ((fd = open("/dev/tty", O_RDWR)) > 0) {
        ioctl(fd, TIOCNOTTY, NULL) ;
        close(fd);
    }

    if (fork())
        exit(0);

    /* get the uid */
    sysUid = getuid();
    
    return 1;
 }




#include <sys/types.h>
#include <unistd.h>

/**
 *  创建 ctrl socket链接，初始化并返回
 **/
void CreateListenSocket(const Option* options, int listenFd)
{
    int fd, i;
    struct sockaddr_in socketAddr;
    struct hostent *host;
    struct in_addr ia;

    /* register the signal handler */
    signal(SIGCHLD, SignalChldHandler);
    signal(SIGTERM, SignalTermHandler);

    /* init network */
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family      = AF_INET;
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddr.sin_port        = htons(options->serverPort);

    if (gethostname(hostname, BUFSIZE) == -1)
        strcpy(hostname, "");

    i = 1;
    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
        setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&i, sizeof(int)) == -1) {
        perror("my FTP:");
        return -1;
    }

    if (bind(listenFd, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) == -1) {
        fprintf(stderr, "my FTP: Can not bind port %d.\n", options->serverPort);
        return -1;
    }
    if (listen(listenFd, options->maxConnectCount) == -1) {
        perror("my FTP:");
        return -1;
    }
    
    /* get the uid */
    //int sysUid = -1;
    SetProgramDaemon(g_sysUid);
    
    return ;
} 
 
 
enum _encryptMode{
    EncryptMode_Reserved = 0,
    EncryptMode_BASE64,
    EncryptMode_AES,
    EncryptMode_DES,
    EncryptMode_MD5,
    EncryptMode_RSA,
    EncryptMode_SHA128,
    EncryptMode_SHA256,
    EncryptMode_Private01,
    EncryptMode_Private02,
    EncryptMode_Private03,
    EncryptMode_Private04,
    EncryptMode_Unencrypt,
    EncryptMode_Undefined
};

int DecryptCommand(const char* recvBuf, const int decryptMode, char* tmpRecvBuf)
{
    if(decryptMode <= EncryptMode_Reserved || decryptMode >= EncryptMode_Undefined){
        printf("decryptMode[%d] error.\n", decryptMode);
        return -1;
    }
    
    switch(decryptMode){
        case EncryptMode_BASE64:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_AES:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_DES:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_MD5:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_RSA:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_SHA128:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_SHA256:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_Private01:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_Private02:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_Private03:
            printf("encryptMode[%d].\n", encryptMode);
            break;
        case EncryptMode_Private04:
            printf("encryptMode[%d].\n", encryptMode);
            break;    
        case EncryptMode_Unencrypt:
        default:
            printf("default encryptMode[%d].\n", encryptMode);
            strcpy(recvBuf, tmpRecvBuf);
            break;
    }
    
    return 0;
}
 

/**
 *  从接收到的消息中，解析命令和参数
 *      将buf以空格做分隔符分解成两个字符串;如： CWD <SP> <pathname> <CRLF>
 *      sample: CWD /home/hybroad/bin \r\n
 **/
int ParseCommand(const char* recvBuf, char** cmd, char** param)
{
    char* p = NULL;
    char tmpRecvBuf[BUFSIZE] = {};
    
    // 解密命令：
    DecryptCommand(recvBuf, EncryptMode_Unencrypt, tmpRecvBuf);
    
    // 解析命令参数
    if ((tmpRecvBuf[strlen(tmpRecvBuf) - 2] == '\r') && (tmpRecvBuf[strlen(tmpRecvBuf) - 1] == '\n')) {
        p = strchr(tmpRecvBuf, ' ');

        if (p != NULL) {
            strncpy(*cmd, tmpRecvBuf, p - tmpRecvBuf); //find cmd
            *cmd[strlen(cmd)] = '\0'; 
            
            strncpy(*param, p + 1, strlen(tmpRecvBuf) - (p - tmpRecvBuf) - 1); //find param
            *param[strlen(param)] = '\0';
        else {
            strcpy(*cmd, tmpRecvBuf); //find cmd
            *cmd[strlen(cmd)] = '\0';
            
            *param = NULL;
        }
    } else {
        printf("command format error.\n");
        return -1;
    }
    
    return 1;
}
 

 
/**
 *  从命令注册表中，查找函数运行并返回结果
 **/
void RunCommand(const char** cmd, const char** param, const int connectFd, char* retValue)
{
    int currentPos = 0;
    int okay = 0;
    
    if (strcmp(*cmd, "") == 0){
        printf("cmd error, is NULL.\n");
        return -1;
    }
    
    /* Make cmd UPPERCASE */
    for (i = 0; i < strlen(*cmd); i++)
        *cmd[i] = toupper(*cmd[i]);
    
    /* find command from list */
    while (strcmp(commandList[currentPos].command, "") != 0) {
        if (strcmp(commandList[currentPos].command, *cmd) == 0) {
            if (commandList[currentPos].function == NULL) {
                char tmpBuf[32] = {};
                sprintf(tmpBuf, "%s command unimplemented.", *cmd);
                AckResponse(connectFd, ACK_502, tmpBuf);
            } else {
                if (1 == CommandVerification(*cmd, *param, commandList[currentPos].check))
                    (*commandList[currentPos].function)(connectFd, *param, NULL);
                else
                    printf("RunCommand, Command Verification error.\n");
            }
            okay = 1;
            break;
        }
        currentPos++;
    }
    if (!okay) {
        char tmpBuf[32] = {};
        sprintf(tmpBuf, " command %s not understood.", *cmd);
        AckResponse(connectFd, ACK_500, tmpBuf);
    }
    
    return ;
}




/**
 *  help帮助
 **/
void Help(const char* param)
{
    if (!param) {
        AckResponse(connectFd, ACK_214, " The following commands are recognized (* =>'s unimplemented).");
        AckResponse(connectFd, ACK_214, "   USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   ");
        AckResponse(connectFd, ACK_214, "   QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    ");
        AckResponse(connectFd, ACK_214, "   STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR    ");
        AckResponse(connectFd, ACK_214, "   DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    ");
        AckResponse(connectFd, ACK_214, "   SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    ");
        AckResponse(connectFd, ACK_214, " Direct comments to nolove@263.net");
    } else {
        AckResponse(connectFd, ACK_214, " Sorry, I haven't write the topic help.");
    }

    return ;
}


//static int connectFd;
static char path[PATH_MAX];
 
struct _port {
        uint32_t host;
        uint16_t port;
} localPort;

void DoingAlarm(int connectFd)
{
    char tmpBuf[32] = {};
    sprintf(tmpBuf, "No Transfer Timeout (%d seconds): Service not available, closing control connection.", DEFAULT_MAX_CONNECT_COUNT);
    AckResponse(connectFd, ACK_421, tmpBuf);
    fclose(file);
    close(connectFd);
    exit(0);
}



int g_sysUid;
/**
 *  
 *  解析配置；
 *  创建监听socket，循环监听端口；
 *  接收到命令后，fork子进程进行处理；
 *  查找命令表，运行并返回应答码；
 *  进入到下一次的监听循环；
 *  
 **/
int main(int argc, char** argv)
{
    Option options;
    
    //TODO:解析配置参数
    ParseOptions((const char**) argv, (const int) argc, &options);
    
    
    //TODO:创建监听socket，进入主循环，监听端口数据
    int listenFd;
    CreateListenSocket((const Option*) &options, listenFd);
    
    //TODO: set the program in a daemon
    SetProgramDaemon(sysUid);

    //TODO:fork子进程进行命令处理
    /* main loop */
    struct sockaddr_in socketAddr;
    int len;
    FILE *fileStream;
    char recvBuf[BUFSIZE];
    char *cmd, *param;
    char retValue[BUFSIZE];
    int connectFd;
    char tmpBuf[64] = {0};
    
    while (1) {
        len = sizeof(socketAddr);
        connectFd = accept(listenfd, (struct sockaddr *)&socketAddr, &len);
        if (connectFd == -1)
            continue;
        
        //fork子进程处理命令
        if (fork() == 0) {
            len = sizeof(socketAddr);
            getsockname(connectFd, (struct sockaddr *)&socketAddr, &len);
            localPort.host = socketAddr.sin_addr.s_addr;
            fileStream = fdopen(connectFd, "rt+");
            setbuf(fileStream, (char *)NULL);  //关闭缓冲，参数设置为NULL
            getcwd(path, PATH_MAX);  //获取当前目录的绝对路径
            
            sprintf(tmpBuf, "myFTP ready, hostname: %s", hostname);
            AckResponse(connectFd, ACK_220, tmpBuf);
            
            signal(SIGALRM, DoingAlarm);
            alarm(options.timeOut);
            
            /* child process's main loop */
            while (fgets(recvBuf, BUFSIZE, fileStream) != NULL) {
                /* 解析命令和参数 */
                ParseCommand((const char*)recvBuf, &cmd, &param);
                if (strcmp(cmd, "") == 0)
                    continue;
                
                /* 从命令注册表中，查找函数，校验命令和参数，运行并返回返回应答码 */
                RunCommand((const char**)&cmd, (const char**)&param, connectFd, retValue);
            }
            
            fclose(fileStream);
            exit(0);
        }
        close(connectFd);
    }
    
    
    return 0;
}





 
#define NO_CHECK                1 //1
#define NEED_PARAM              1<<1 //2
#define NO_PARAM                1<<2 //4
#define CHECK_LOGIN             1<<3 //8
#define CHECK_NOLOGIN           1<<4 //16
#define NO_TRANSFER             1<<5 //32

struct _CmdList {
        char *command;
        void (*function)(int connectFd, char *param, void* reserved);
        int check;
};

 
struct _CmdList commandList[] = {
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
    
    /////own
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {"NOOP",    Noop,   NO_CHECK    },
    {""}
};




