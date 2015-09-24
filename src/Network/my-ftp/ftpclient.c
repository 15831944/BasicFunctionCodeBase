/**
 *  vi: set sw=4 ts=4:
 *  
 *  ftp client c program from busybox
 *  
 * ftpget
 *
 * Mini implementation of FTP to retrieve a remote file.
 *
 * Copyright (C) 2002 Jeff Angielski, The PTR Group <jeff@theptrgroup.com>
 * Copyright (C) 2002 Glenn McGrath <bug1@iinet.net.au>
 *
 * Based on wget.c by Chip Rosenthal Covad Communications
 * <chip@laserlink.net>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 
 下面是 FTP client端发送的命令格式：
        USER <SP> <username> <CRLF>
        PASS <SP> <password> <CRLF>
        ACCT <SP> <account-information> <CRLF>
        CWD <SP> <pathname> <CRLF>
        CDUP <CRLF>
        SMNT <SP> <pathname> <CRLF>
        QUIT <CRLF>
        REIN <CRLF>
        PORT <SP> <host-port> <CRLF>
        PASV <CRLF>
        TYPE <SP> <type-code> <CRLF>
        STRU <SP> <structure-code> <CRLF>
        MODE <SP> <mode-code> <CRLF>
        RETR <SP> <pathname> <CRLF>
        STOR <SP> <pathname> <CRLF>
        STOU <CRLF>
        APPE <SP> <pathname> <CRLF>
        ALLO <SP> <decimal-integer> [<SP> R <SP> <decimal-integer>] <CRLF>
        REST <SP> <marker> <CRLF>
        RNFR <SP> <pathname> <CRLF>
        RNTO <SP> <pathname> <CRLF>
        ABOR <CRLF>
        DELE <SP> <pathname> <CRLF>
        RMD <SP> <pathname> <CRLF>
        MKD <SP> <pathname> <CRLF>
        PWD <CRLF>
        LIST [<SP> <pathname>] <CRLF>
        NLST [<SP> <pathname>] <CRLF>
        SITE <SP> <string> <CRLF>
        SYST <CRLF>
        STAT [<SP> <pathname>] <CRLF>
        HELP [<SP> <string>] <CRLF>
        NOOP <CRLF>
 **/


/**
 *  flow：
 *      建立socket链接；
 *      等待操作；
 *      通过命令选择要执行的操作：
 *      解析操作：
 *      按格式封装命令；
 *      发送命令并等待服务器响应；
 *      解析响应码 并输出相应内容；
 **/


/**
 *  创建socket：
 *  login登陆；
 *  发送操作命令；
 *      变更目录；
 *      获取文件——数据流；
 *      。。。
 *  操作结束，logout退出；
 *  
 **/
 
 
 
 
#define DEFAULT_USERNAME    "anounse"
#define DEFAULT_USERPASSWD  "" 
#define DEFAULT_SERVERIP    "" 
#define DEFAULT_SERVERPORT  21 
#define DEFAULT_TIMEOUT     300

typedef struct _options{
    int serverPort;
    char serverIP[32]; 
    char userName[32]; 
    char userPasswd[32]; 
    int timeOut;  //unit: seconds
} Option;

  
#include <linux/socket.h>
#include <netinet/in.h>

typedef struct _socketAddr{
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
    };
} SocketAddr;

 
 
/**
 *  从命令行输入和配置文件中解析配置参数
 *  只支持简单的格式解析，如[-p:port]
 *      sample: ftp  [-u:user_name]  [-p:passwd]  [-i:ip_addr]  [-n:ip_port]  [-t:time_out]
 **/
int ParseOptions(const char** buf, const int count, Option* options, SocketAddr* serverSocketAddrs)
{
    int i, okFlag;
    struct addrinfo hint;
    struct addrinfo *result = NULL;
    int rc;

    if(buf == NULL || count == 0){
        printf("ParseOptions, param options buf is null error.\n");
        return -1;
    }
    
    /* set option default */
    sprintf(options->userName, "%s", DEFAULT_USERNAME);
    sprintf(options->userPasswd, "%s", DEFAULT_USERPASSWD);
    sprintf(options->serverIP, "%s", DEFAULT_SERVERIP);
    options->serverPort = DEFAULT_SERVERPORT;
    options->timeOut = DEFAULT_TIMEOUT;
     
    okFlag = 1;
    /* get the option param */
    for (i = 1; i < count; i++) {
        if (buf[i][0] != '-' || buf[i][2] != ':')
            okFlag = 0;
        switch (buf[i][1]) {
        case 'u':
            sprintf(options->userName, "%s",&buf[i][3]);
            if (strlen(options->userName) == 0)
                okFlag = 0;
            break;
        case 'p':
            sprintf(options->userPasswd, "%s",&buf[i][3]);
            if (strlen(options->userPasswd) == 0)
                okFlag = 0;
            break;
        case 'i':
            sprintf(options->serverIP, "%s",&buf[i][3]);
            if (strlen(options->serverIP) == 0)
                okFlag = 0;
            break;
        case 'n':
            options->serverPort = atoi(&buf[i][3]);
            if (options->serverPort == 0)
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
        printf("Usage: %s [-u:user] [-p:passwd] [-i:ip] [-n:port] [-t:timeout] port: default is 21.", argv[0]);
        return -1;
    }
    
    /* set server socket addr */
    memset(&hint, 0 , sizeof(hint));
    hint.ai_family = AF_INET; 
    hint.ai_socktype = SOCK_STREAM;  /* Needed. Or else we will get each address thrice (or more) for each possible socket type (tcp,udp,raw...): */
    hint.ai_flags = ai_flags & ~DIE_ON_ERROR;
    rc  = getaddrinfo(options->serverIP, NULL, &hint, &result);
    if (rc || !result) {
        printf("bad address '%s'", options->serverIP);
        if (ai_flags & DIE_ON_ERROR)
            exit(-1);
        freeaddrinfo(result);
        return -1;
    }
    
    serverSocketAddrs->len = result->ai_addrlen;
    memcpy(&serverSocketAddrs->sa, result->ai_addr, result->ai_addrlen);
    if (serverSocketAddrs->sa.sa_family == AF_INET) {
        serverSocketAddrs->sin.sin_port = htons(port);
    } /* What? UNIX socket? IPX?? :) */
    
    freeaddrinfo(result);
    return 0;
}




/**
 *  
 *  创建socket链接：
 *  
 **/
int CreateSocket(SocketAddr* serverSocketAddrs, int socketCtrlFd)
{
    socketCtrlFd = socket(serverSocketAddrs->sa.sa_family, SOCK_STREAM, 0);
    if (socketCtrlFd < 0) {
        printf("socket\n");
        exit (-1);
    }
    
    if (connect(socketCtrlFd, serverSocketAddrs->sa, serverSocketAddrs->len) < 0) {
        close(socketCtrlFd);
        if (serverSocketAddrs->sa.sa_family == AF_INET) {
            printf("%s (%d)", "cannot connect to remote host", inet_ntoa(((struct sockaddr_in *)serverSocketAddrs->sa).sin_addr));
            exit (-1);
        }
    }
    
    return socketCtrlFd;
}




/**
 *  拼装命令：
 *      类似：
 *          USER <SP> <username> <CRLF> 
 *          PASS <SP> <password> <CRLF>
 **/
int ComposeCommand(const char** cmd, const char** param, char* sendBuf)
{
    if (strlen(*param) ==  0)
        return -1;
    
    if (strlen(*param) ==  0)
        sprintf(sendBuf, "%s\r\n", *cmd);
    else
        sprintf(sendBuf, "%s %s\r\n", *cmd, *param);
    
    return 0;
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

/**
 *  拼装命令：
 *      类似：
 *          USER <SP> <username> <CRLF> 
 *          PASS <SP> <password> <CRLF>
 **/
int EncryptCommand(const char* tmpBuf, const int encryptMode, char* sendBuf)
{
    if(encryptMode <= EncryptMode_Reserved || encryptMode >= EncryptMode_Undefined){
        printf("encryptMode[%d] error.\n", encryptMode);
        return -1;
    }
    
    switch(encryptMode){
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
            strcpy(tmpBuf, sendBuf);
            break;
    }
    
    return 0;
}



#define BUFSIZE         (512)
typedef signed char smallint;
len_and_sockaddr;
typedef struct ftp_host_info_s {
    const char *user;
    const char *password;
    struct SocketAddr *lsa;
} ftp_host_info_t;
/**
 *  
 *  发送命令：
 *  
 **/
int SendCommandAndReceiveAck(const int socketCtrlFd, const char** cmd, const char** param, char* cmdPromote)
{
    FILE *controlStream;
    char tmpSendBuf[BUFSIZE] = {};
    char sendBuf[BUFSIZE] = {};
    char recvBuf[BUFSIZE] = {};
    int ackCode;
    
    /* Connect to the command socket */
    controlStream = fdopen(socketCtrlFd, "r+");
    if (controlStream == NULL) { /* fdopen failed - extremely unlikely */
        printf("fdopen error.\n");
        exit (-1);
    }
    
    //组装命令
    ComposeCommand(cmd, param, tmpSendBuf);
    
    //加密命令
    EncryptCommand(tmpSendBuf, EncryptMode_Unencrypt, sendBuf);
    
    //发送命令
    if (sendBuf) {
        fprintf(controlStream, "%s", sendBuf);
    }
    
    //接收服务端ACK
    memset(recvBuf, 0, BUFSIZE);
    do {
        if (fgets(recvBuf, BUFSIZE - 3, controlStream) == NULL) {
            printf("fgets error.\n");
            exit(-1);
        }
    } while (!isdigit(recvBuf[0]) || recvBuf[3] != ' ');
    
    //解析响应码
    if (ParseAckResponse((const char*)recvBuf, ackCode, cmdPromote) == -1){
        printf("ParseAckResponse error.\n");
        exit(-1);
    }
    
    fclose(controlStream);
    return ackCode;
}




/**
 *  
 *  解析服务器应答
 *  
 **/
int ParseAckResponse(const char* ackBuf, int ackCode, char* cmdPromote)
{
    if (strlen(ackBuf) == 0) 
        return -1;
    
    char* p = NULL;
    char code[4] = {};
    char promote[128] = {};
    
    if ((ackBuf[strlen(ackBuf) - 2] == '\r') && (ackBuf[strlen(ackBuf) - 1] == '\n')) {
        p = strchr(ackBuf, ' ');

        if (p != NULL) {
            strncpy(code, ackBuf, p - ackBuf); //find ackCode
            code[strlen(code)] = '\0'; 
            
            strncpy(cmdPromote, p + 1, strlen(ackBuf) - (p - ackBuf) - 1); //find cmdPromote
            cmdPromote[strlen(cmdPromote)] = '\0';
        else {
            strcpy(code, ackBuf); //find ackCode
            code[strlen(code)] = '\0';
            
            strcpy(cmdPromote, "");
        }
        ackCode = atoi(code);
        printf("ackCode:[%d]\n", ackCode);
    } else {
        printf("server Ack Response format error.\n");
        return -1;
    }
        
    return 0;
}



int Login(const Option* options, SocketAddr* serverSocketAddrs)
{
    // 创建socket
    int socketCtrlFd;
    CreateSocket(serverSocketAddrs, socketCtrlFd);
    
    //// 发送空命令，判断服务是否就绪
    // 发送命令，接收服务端应答，解析应答
    char cmdPromote[128];
    int ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "", "", cmdPromote);
    if (ackCode != ACK_220){
        printf("ftp_die\n");
        exit(-1);
    }
    
    //User 
    ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "USER", options->userName, cmdPromote);
    printf("ackCode[%d], cmdPromote[%s]\n", ackCode, cmdPromote);
    switch(ackCode){
        case ACK_230:
            printf("ACK_230\n");
            exit(-1);
            break;
        case ACK_331:
            printf("ACK_331\n");
            
            //passwd
            //加密密码
            char tmpBuf[1024] = {};
            EncryptCommand(options->userPasswd, EncryptMode_Unencrypt, tmpBuf);
            
            // 发送命令，接收服务端应答，解析应答
            ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "PASS", tmpBuf, cmdPromote);
            printf("ackCode[%d], cmdPromote[%s]\n", ackCode, cmdPromote);
            break;
        default :
            printf("default\n");
            exit(-1);
    }
    ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "TYPE", "I", cmdPromote);
    printf("ackCode[%d], cmdPromote[%s]\n", ackCode, cmdPromote);
    
    return socketCtrlFd;
}






/**
 *  
 *  数据接收：
 *  
 **/
int ReceiveDataHeadInfo(const int socketCtrlFd, SocketAddr* serverSocketAddrs, const char* serverPath, char* recvDataHeadInfoBuf, int* socketDataFd)
{    
    //pasv
    char cmdPromote[128];
    int ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "PASV", "", cmdPromote);
    if (ackCode != ACK_227){
        printf("ftp_die\n");
        exit(-1);
    }

    /* Connect to the data socket */
    // 创建 data socket
    CreateSocket(serverSocketAddrs, *socketDataFd);
    if (*socketDataFd < 0)
        return -1;
    
    //size
    ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "SIZE", serverPath, cmdPromote);
    if (ackCode == ACK_213){
        fileSize = strtol(cmdPromote, NULL, 10);
        if (fileSize < 0){
            printf("ftp_die\n");
            exit(-1);
        }
    } else {
        
    }
    
    //retr
    ackCode = SendCommandAndReceiveAck((const int)socketCtrlFd, "RETR", serverPath, cmdPromote);
    if (ackCode > ACK_150){
        printf("ftp_die\n");
        exit(-1);
    }
    
    // recv data head info
    memset(recvDataHeadInfoBuf, 0, BUFSIZE);
    ssize_t headSize;
    do {
        headSize = read(*socketDataFd, recvDataHeadInfoBuf, BUFSIZE);
    } while (headSize < 0 && errno == EINTR);

    return (int)headSize;
}



int ReceiveDataToFile(const int socketCtrlFd, const int socketDataFd, SocketAddr* serverSocketAddrs, int headSize, const char* serverPath, const char *localFileName)
{
    
    int localFd = -1;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    int mode = 0666;

    localFd = open(localFileName, flags, mode);
    if (localFd < 0) {
        printf("can't open '%s'", localFileName);
        exit (-1);
    }
    
    caFtpHead[FTP_HEAD_SIZE] = {};
    /**
     * Write all of the supplied buffer out to a file. This does multiple writes as necessary.
     * Returns the amount written, or -1 on an error.
     **/
    ssize_t cc;
    ssize_t total;
    total = 0;
    while (headSize) {
        cc = safe_write(localFd, caFtpHead, headSize);
        if (cc < 0)
            return cc;  /* write() returns -1 on failure. */
        total += cc;
        caFtpHead = ((const char *)caFtpHead) + cc;
        headSize -= cc;
    }

    
    
    
    
    /* Connect to the command socket */
    FILE* controlStream = fdopen(socketCtrlFd, "r+");
    if (controlStream == NULL) { /* fdopen failed - extremely unlikely */
        printf("fdopen error.\n");
        exit (-1);
    }
    
    fclose(controlStream);
    
    int ftp_recv_to_file(int socketDataFd, FILE *control_stream, char caFtpHead[FTP_HEAD_SIZE], int nHeadSize, const char *localFileName)
{
    int fd_local = -1;
    char buf[512];
    int status = -1;
    off_t total = 0;
    char buffer[BUFSIZ];
    int size = 0;
    fd_local = xopen(localFileName, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if (fd_local < 0) {
        goto out;
    }

    ssize_t wr = full_write(fd_local, caFtpHead, nHeadSize);
    
    if (wr < nHeadSize) {
        //bb_error_msg(bb_msg_write_error);
        goto out;
    }

    total += nHeadSize;
    if (!size) {
        size = BUFSIZ;
        status = 1; /* copy until eof */
    }

    while (1) {
        ssize_t rd;
        rd = safe_read(socketDataFd, buffer, size > BUFSIZ ? BUFSIZ : size);
        //printf("-----rd =%d----n", rd);
        if (!rd) { /* eof - all done */
            status = 0;
            break;
        }

        if (rd < 0) {
            //bb_error_msg(bb_msg_read_error);
            break;
        }

        /* dst_fd == -1 is a fake, else... */
        if (fd_local >= 0) {
            ssize_t wr = full_write(fd_local, buffer, rd);
            if (wr < rd) {
                //bb_error_msg(bb_msg_write_error);
                break;
            }
        }

        total += rd;
        if (status < 0) { /* if we aren't copying till EOF... */
            size -= rd;
            if (!size) {
                /* 'size' bytes copied - all done */
                status = 0;
                break;
            }
        }
    }

    /* close it all down */
    close(fd_local);
    close(socketDataFd);
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die(NULL, buf);
    }
    ftpcmd("QUIT", NULL, control_stream, buf);

out:
    return status ? -1 : total;
}

    
    
    
    return 0;
}

int ReceiveDataToRam(Option* options, SocketAddr* serverSocketAddrs, char* ramBuf)
{
    
    
    return 0;
}







