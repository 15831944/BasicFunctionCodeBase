/*
 * socket.c (c) 2007        wzt
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include "socket.h"

extern pthread_mutex_t thread_lock;
extern unsigned long thread_num;

/**
 * make_network_ip - make ip from host byte to network byte.
 *
 * host - remote host ip.
 *
 * successfull return the network byte,failed return 0;
 */
unsigned int make_network_ip(char *host)
{
        struct hostent *h;
        unsigned int ret;

        if ((h = gethostbyname(host)) == NULL) {
                ret = inet_addr(host);
                if (ret == -1)
                        return 0;
                return ret;
        }
        else {
                ret = *((unsigned int *)h->h_addr);
                if (ret <= 0)
                        return 0;
                return ret;
        }
}

int tcp_connect(unsigned int remote_ip,unsigned int remote_port,int timeout)
{
        struct sockaddr_in serv_addr;
        int sock_fd;

        if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
                perror("[-] socket");
                exit(0);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = remote_port;
        serv_addr.sin_addr.s_addr = remote_ip;

        if (connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1)
               return 0;

        return sock_fd;
}

/**
 * tcp_connect_nblock - connect to remote host.
 *
 * ip: remote ip but with network byte.
 * port : remote port but with network byte.
 *
 * if connected successfull ,return remote socket descriptor,failed return 0;
 */
int tcp_connect_nblock(unsigned int remote_ip,unsigned int remote_port,int timeout)
{
        struct sockaddr_in serv_addr;
        struct timeval time_out;
        fd_set s_read;
        int sock_fd;
        unsigned long flag;
        int len,error;

        if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
                #ifdef DEBUG
                perror("[-] socket");
                #endif
                exit(0);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = remote_port;
        serv_addr.sin_addr.s_addr = remote_ip;

        flag = fcntl(sock_fd,F_GETFL,0);
        if (flag < 0) {
                printf("[-] get fcntl error.\n");
                return 0;
        }
        if (fcntl(sock_fd,F_SETFL,flag|O_NONBLOCK) < 0) {
                printf("[-] set fcntl error.\n");
                return 0;
        }

        if (connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) {        
               time_out.tv_sec = timeout;
               time_out.tv_usec = 0;
               FD_ZERO(&s_read);
               FD_SET(sock_fd,&s_read);
        
               switch (select(sock_fd + 1,NULL,&s_read,NULL,&time_out)) {
                        case -1:
                               #ifdef DEBUG
                               printf("[-] select error.\n");
                               #endif
                               goto err;
                        case 0:
                                /*
                                #ifdef DEBUG
                                printf("[-] %-5d \t\t\t\t[timeout]\n",ntohs(remote_port));
                                #endif
                                */
                                goto err;
                        default:
                                if (FD_ISSET(sock_fd,&s_read)) {
                                        len = sizeof(error);
                                        if (getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,(char *)&error,&len) < 0)
                                                goto err;
                                        if (error == 0) {
                                                if (fcntl(sock_fd,F_SETFL,flag) < 0) {
                                                        printf("[-] fcntl recover error.\n");
                                                        return 0;
                                                }
                                                return sock_fd;
                                        }
                                        else
                                                goto err;
                                }
                                else
                                        goto err;
                }
        }

        err:
        close(sock_fd);
        return 0;
}


/**
 * tcp_connect_fast - connect to remote host.
 *
 * the function like tcp_connect(),but when it connected successfull,it close
 * the socket immediately,do not read and write data.
 *
 * ip: remote ip but with network byte.
 * port : remote port but with network byte.
 *
 * if connected successfull ,return 1,failed return 0;
 */
int tcp_connect_fast(unsigned int remote_ip,unsigned int remote_port,int timeout)
{
        struct sockaddr_in serv_addr;
        struct timeval time_out;
        fd_set s_read;
        int sock_fd;
        unsigned long flag;
        int len,error;

        if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
                #ifdef DEBUG
                perror("[-] socket");
                #endif
                exit(0);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = remote_port;
        serv_addr.sin_addr.s_addr = remote_ip;

        flag = fcntl(sock_fd,F_GETFL,0);
        if (flag < 0) {
                #ifdef DEBUG
                printf("[-] get fcntl error.\n");
                #endif
                return 0;
        }
        if (fcntl(sock_fd,F_SETFL,flag|O_NONBLOCK) < 0) {
                #ifdef DEBUG
                printf("[-] set fcntl error.\n");
                #endif
                return 0;
        }

        if (connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) {        
               time_out.tv_sec = timeout;
               time_out.tv_usec = 0;
               FD_ZERO(&s_read);
               FD_SET(sock_fd,&s_read);
        
               switch (select(sock_fd + 1,NULL,&s_read,NULL,&time_out)) {
                        case -1:
                               #ifdef DEBUG
                               printf("[-] select error.\n");
                               #endif
                               goto err;
                        case 0:
                                /*
                                #ifdef DEBUG
                                printf("[-] %-5d \t\t\t\t[timeout]\n",ntohs(remote_port));
                                #endif
                                */
                                goto err;
                        default:
                                if (FD_ISSET(sock_fd,&s_read)) {
                                        len = sizeof(error);
                                        if (getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,(char *)&error,&len) < 0)
                                                goto err;
                                        if (error == 0) {
                                                //shutdown(sock_fd,2);
                                                close(sock_fd);
                                                return 1;
                                        }
                                        else
                                                goto err;
                                }
                                else
                                        goto err;
                }
        }

        err:
//        pthread_mutex_lock(&thread_lock);
//        thread_num--;
//        pthread_mutex_unlock(&thread_lock);
        
        close(sock_fd);
        return 0;
}

/**
 * tcp_listen - bind a port with localhost.
 *
 * port : localport but with network byte.
 *
 * successfull return the remote socket descriptor,failed retrurn 0;
 */
int listen_port(unsigned int port)
{
        struct sockaddr_in my_addr;
        int sock_fd,sock_id;
        int size,flag = 1;

        if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
                #ifdef DEBUG
                perror("[-] socket");
                #endif
                exit(1);
        }

        my_addr.sin_family = AF_INET;
        my_addr.sin_port = port;
        my_addr.sin_addr.s_addr = 0;

        setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR, (char*)&flag,sizeof(flag));

        if (bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1) {
                #ifdef DEBUG
                perror("[-] bind");
                #endif
                return 0;
        }

        if (listen(sock_fd,MAXUSER) == -1) {
                #ifdef DEBUG
                perror("[-] listen");
                #endif
                return 0;
        }

        return sock_fd;
}