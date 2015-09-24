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

 
 //log format:  cmd    :ack_code   :msg
 // .e.g        TYPE I : 231 : the  type is error.
 
 
/**
 *  
 *  建立socket链接；
 *  等待操作；
 *  通过命令选择要执行的操作：
 *  解析操作：
 *  按格式封装命令；
 *  发送命令并等待服务器响应；
 *  解析响应码 并输出相应内容；
 **/


/**
 *  
 *  
 *  #include<netdb.h>

函数原型
int getaddrinfo( const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result );

参数说明
hostname:一个主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
service：服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等
hints：可以是一个空指针，也可以是一个指向某个addrinfo结构体的指针，调用者在这个结构中填入关于期望返回的信息类型的暗示。举例来说：如果指定的服务既支持TCP也支持UDP，那么调用者可以把hints结构中的ai_socktype成员设置成SOCK_DGRAM使得返回的仅仅是适用于数据报套接口的信息。
result：本函数通过result指针参数返回一个指向addrinfo结构体链表的指针。
返回值：0——成功，非0——出错
 *  
 **/

     /**
     *  int send(int sockfd, const void *msg, int len, int flags);
send 的参数含义如下：
l sockfd 是代表你与远程程序连接的套接字描述符。
l msg 是一个指针，指向你想发送的信息的地址。
l len 是你想发送信息的长度。
l flags 发送标记。一般都设为0（你可以查看send 的man pages 来获得其他的参数
值并且明白各个参数所代表的含义）。
     **/
     
     
     
     
     
     
     
     
     
     