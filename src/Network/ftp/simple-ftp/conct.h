#ifndef _CONCT_H_
#define _CONCT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#define INVALID_SOCKET (~0)
#define DEFAULT_PORT_NUM 21
int  hListenSocket = INVALID_SOCKET,
     hControlSocket= INVALID_SOCKET,
     hDataSocket=    INVALID_SOCKET,
     flag_connected     = 0;
char host_add[1024]  = {0};
int bSendPort      = 1;
int get_host_reply(),
    get_line(),
send_ctrl_msg(char *, int),
flag_connect_to_server(char *name, char *port),
get_listen_socket();
#endif