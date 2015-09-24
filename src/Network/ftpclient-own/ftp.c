/*
 * Xsec mini ftp client V 1.2
 *
 * It can support commands:
 * help, ls, als, cd, get, aget, put, aput, rename,
 * delete, mkdir, rmdir, pwd, bye, quit
 *
 * NOTE:
 * ls : It based on PORT command, the client listen on a port and
 *      wait the server to connect.
 * als: It based on PASV command, the server listen on a port and
 *      wait the client to connect.
 *
 * get,aget ,put,aput like ls and als.
 *
 * tested on:
 * Windows: Serv-U FTP Server v6.3
 *
 * by wzt       2008/1/2
 *
 * [url]http://tthacker.cublog.cn[/url]
 *
 *   This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>
#include "socket.h"
#include "ftp.h"

char session_tmp[MAX_DATA];
char ftp_data_ip[20], ftp_data_port1[10], ftp_data_port2[10];
int ftp_data_port;
int g_ftp_ip, g_ftp_port;
int ftp_sock = 0;

/* currently supported ftp commands */
struct ftp_cmd_struct ftp_cmds[] = {
        {"!",           ftp_do_exit},
        {"?",           ftp_do_help},
        {"help",        ftp_do_help},
        {"ls",          ftp_do_ls_pasv},
        {"als",         ftp_do_ls_port},
        {"cd",          ftp_do_cd},
        {"get",         ftp_do_get_pasv},
        {"aget",        ftp_do_get_port},
        {"put",         ftp_do_put_pasv},
        {"aput",        ftp_do_put_port},
        {"rename",      ftp_do_rename},
        {"delete",      ftp_do_delete},
        {"mkdir",       ftp_do_mkdir},
        {"rmdir",       ftp_do_rmdir},
        {"pwd",         ftp_do_pwd},
        {"bye",         ftp_do_bye},
        {"quit",        ftp_do_quit},
        {NULL,          NULL}
};

void do_ctrl_c(int sig_num)
{
        fprintf(stdout, "\n%s", FTP_BANNER);

        do_ftp_client();
}

int ftp_client_init(void)
{
        signal(SIGINT, do_ctrl_c);
        
        return 1;
}

/** change ip address like '192,168,6,1', the PORT command will use it */
void change_ip_addr(char *ip_addr)
{
        int i;
        
        for (i = 0; i < strlen(ip_addr); i++) {
                if (ip_addr[i] == '.')
                        ip_addr[i] = ',';
        }
}

/* set terminal echo off */
void terminal_echo_off(int fd)
{
        struct termios old_terminal;
        
        tcgetattr(fd, &old_terminal);
        old_terminal.c_lflag &= ~ECHO;
        tcsetattr(fd, TCSAFLUSH, &old_terminal);
}

/* set terminal echo on */
void terminal_echo_on(int fd)
{
        struct termios old_terminal;
        
        tcgetattr(fd, &old_terminal);
        old_terminal.c_lflag |= ECHO;
        tcsetattr(fd, TCSAFLUSH, &old_terminal);
}

/* get pass word */
int get_passwd(char *passwd, int passwd_len)
{
        int c, n = 0;

        fprintf(stdout, "Password:");

        scanf("%s", passwd);
        if (strlen(passwd) > passwd_len) {
                printf("Pass word too long.\n");
                do_ftp_client();
        }
        
        return n;
}
        
int do_ftp_cmd(int ftp_sock,char *cmd_tmp)
{
        int i = 0;
        char tmp[128];
        char *cp = strchr(cmd_tmp, ' ');

        if (cp)
                sscanf(cmd_tmp, "%s", tmp);
        else
                strcpy(tmp, cmd_tmp);
                
        while (ftp_cmds[i].cmd_name) {
                if (strcmp(tmp, ftp_cmds[i].cmd_name) != 0) {
                        i++;
                        continue;
                }
                else {
                        if (ftp_sock == 0) {
                                if (!strcmp(ftp_cmds[i].cmd_name, "help") ||
                                        !strcmp(ftp_cmds[i].cmd_name, "!") ||
                                        !strcmp(ftp_cmds[i].cmd_name, "?") ||
                                        !strcmp(ftp_cmds[i].cmd_name, "bye") ||
                                        !strcmp(ftp_cmds[i].cmd_name, "quit")) {
                                        ftp_cmds[i].cmd_handler(ftp_sock, cmd_tmp);
                                        return 1;
                                }
                                fprintf(stdout, "%s", FTP_NON_CONNECTED);
                                return 0;
                        }
                        ftp_cmds[i].cmd_handler(ftp_sock, cmd_tmp);
                        return 1;
                }
        }

        fprintf(stdout, "%s", FTP_ERR_CMD);
        return 0;
}

/**
 * our mini ftp client's consloe
 *
 * FIXME: when the first time it exeutes, it display tow ftp banners, like 'ftp>ftp>'.
 */
void do_ftp_client(void)
{
        char cmd_tmp[MAX_DATA];

        while (1) {
                fprintf(stdout, "%s", FTP_BANNER);
                gets(cmd_tmp);

                if (cmd_tmp[0] == 0) {
                        memset(cmd_tmp, 0, MAX_DATA);
                        continue;
                }

                if (strlen(cmd_tmp) > MAX_DATA) {
               //         fprintf(stdout, "%s", "Command length overflow");
                        DPRINT("%s", "Command length overflow");
                        exit(-1);
                }
                do_ftp_cmd(ftp_sock, cmd_tmp);
                memset(cmd_tmp , 0 , 1024);
        }
}

/** connect to ftp server, if successful return 1, failed return -1 */
int connect_ftp_server(unsigned ftp_ip, unsigned int ftp_port)
{
        struct sockaddr_in client;
        int ftp_sock_fd, len;
        
        /* we connect to the ftp server */
        ftp_sock_fd = tcp_connect(ftp_ip, ftp_port, 2);
        if (ftp_sock_fd <= 0)
                return -1;
        
        /* get localhost's ip and port */
        len = sizeof(struct sockaddr);
        if (getsockname(ftp_sock_fd, (struct sockaddr *)&client, &len) < 0) {
                DPRINT("%s", "getsockname error.\n");
                return -1;
        }
        
        local_port = ntohs(client.sin_port);
        strcpy(local_ip, inet_ntoa(client.sin_addr));
        change_ip_addr(local_ip);

        return ftp_sock_fd;
}

int ftp_send_data(int sock_fd, char *data, int len)
{
        int num, off = 0, left = len;
        
        while (1) {
                num = write(sock_fd, data + off, left);
                if (num < 0) {
                        if (errno == EINTR)
                                continue;
                        return num;
                }
                if (num < left) {
                        left -= num;
                        off += num;
                        continue;
                }
                return len;
        }
}

int ftp_recv_data(int sock_fd, char buf[], int len)
{
        int num;
        
        while (1) {
                num = read(sock_fd, buf, len);
                if (num < 0) {
                        if (errno == EINTR)
                                continue;
                        return num;
                }
                return num;
        }
}

int ftp_send_and_recv(int ftp_sock_fd, char *session_tmp, int len)
{
        int data_len;
        
        data_len = ftp_send_data(ftp_sock_fd, session_tmp, len);
        if (data_len < 0)
                return -1;

        data_len = ftp_recv_data(ftp_sock_fd, session_tmp, MAX_DATA);
        if (data_len < 0)
                return -1;

        session_tmp[data_len] = '\0';
        
        return data_len;
}

/* ftp login init */
int auth_ftp_user(int sock_fd, char *ftp_server)
{
        struct ftp_user_struct user;
        int data_len, ret;
        
        /* read banner data. if successful the server return 220 */
        data_len = ftp_recv_data(sock_fd, session_tmp, MAX_DATA);
        if (data_len < 0)
                return -1;
                
        session_tmp[data_len] = '\0';
        fprintf(stdout, "%s", session_tmp);
        if (strstr(session_tmp, "220") == NULL) {
                return -1;
        }
        
        fprintf(stdout, "User (%s:(root)): ", ftp_server);
        scanf("%s", user.user_name);

        /* send username, if successful the server return 331 */
        sprintf(session_tmp, "USER %s\r\n", user.user_name);
        data_len = ftp_send_and_recv(sock_fd, session_tmp, strlen(session_tmp));
        fprintf(stdout, "%s", session_tmp);
        if (strstr(session_tmp, "331") == NULL) {
                return -1;
        }
                
        terminal_echo_off(STDIN_FILENO);
        get_passwd(user.pass_word, PASSWD_LEN);
        terminal_echo_on(STDOUT_FILENO);
        printf("\n");
        
        /* send password, if successful the server return 230 */
        sprintf(session_tmp, "PASS %s\r\n", user.pass_word);
        data_len = ftp_send_and_recv(sock_fd, session_tmp, strlen(session_tmp));
        fprintf(stdout, "%s", session_tmp);
        if (strstr(session_tmp, "230") == NULL) {
                return -1;
        }
        
        return 1;
}

/* wait ftp server side to connect, it only used by PORT command mode */
int accept_ftp_server(void)
{
        struct sockaddr_in remote_addr;
        int sock_fd, sock_id, size;
        
        sock_fd = listen_port(htons(local_port));
       
        size = sizeof(struct sockaddr_in);
        while (1) {
                if ((sock_id = accept(sock_fd, (struct sockaddr *)&remote_addr,
                        &size)) == -1) {        
                        perror("[-] accept");
                        usleep(20);
                        continue;
                }
                return sock_id;
        }
}        

int ftp_common_cmd(int ftp_sock_fd, char *cmd_line, char *cmd, char *err_code)
{
        int data_len;
        char *cp = strchr(cmd_line, ' ');

        if (cp)
                sprintf(session_tmp, "%s %s\r\n", cmd, cp + 1);
        else
                sprintf(session_tmp, "%s\r\n", cmd);

        ftp_send_and_recv(ftp_sock_fd, session_tmp, strlen(session_tmp));

        fprintf(stdout, "%s", session_tmp);
        if (strstr(session_tmp, err_code) == NULL) {
                return -1;
        }
        
        return 1;
}

/* ftp command: help, currently it support 15 types. */
int ftp_do_help(int ftp_sock_fd, char *cmd_line)
{
        int i = 0;
        
        fprintf(stdout, "%s", "Commands may be abbreviated.     Commands are:\n\n");
        
        while (ftp_cmds[i].cmd_name) {
                printf("%s\t", ftp_cmds[i].cmd_name);
                if ((i % 5 == 0) && i != 0) {
                        printf("\n");
                }
                i++;
        }
        printf("\n");
}

/* ftp command: rename */
int ftp_do_rename(int ftp_sock_fd, char *cmd_line)
{
        char old_name[128], new_name[128];
        char new_cmd_line[MAX_DATA];
        char *cp, *cp1;

        cp = strchr(cmd_line, ' ');
        if (!cp)  {
                /* comamand like 'rename' */
                printf("From name ");
                gets(old_name);
                sprintf(new_cmd_line, "%s %s", cmd_line, old_name);
                ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RNFR", "350");

                printf("To name ");
                gets(new_name);
                sprintf(new_cmd_line, "%s %s", cmd_line, new_name);
                ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RNTO", "250");
        }
        else {
                cp++;
                cp1 = strchr(cp, ' ');
                if (!cp1) {
                        /* command like 'rename test.c' */
                        sprintf(old_name, "%s", cp);
                        printf("To name ");
                        gets(new_name);
                        ftp_common_cmd(ftp_sock_fd, cmd_line, "RNFR", "350");
                        sprintf(new_cmd_line, "%s %s", "RNTO", new_name);
                        ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RNTO", "250");
                }
                else {
                        /* command like 'rename test.c test1.c' */
                        sscanf(cmd_line, "%s %s %s", new_cmd_line, old_name, new_name);
                        printf("%s,%s\n", old_name, new_name);
                        sprintf(new_cmd_line, "%s %s", "RNFR", old_name);
                        ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RNFR", "350");
                        sprintf(new_cmd_line, "%s %s", "RNTO", new_name);
                        ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RNTO", "250");
                }
        }
}

/**
 * ftp command : ls
 * It based on PORT command, the client listen on a port and wait the server to connect.
 */
int ftp_do_ls_port(int ftp_sock_fd, char *cmd_line)
{
        int new_sock_id, byte;
        char data[MAX_DATA];
        
        /* first send PORT command */
        local_port++;
        sprintf(session_tmp, "PORT %s,%d,%d\r\n",
                        local_ip, (local_port - local_port % 256 ) / 256,
                        local_port % 256);
        ftp_common_cmd(ftp_sock_fd, session_tmp, "PORT", "200");

        /* then send LIST command */
        ftp_common_cmd(ftp_sock_fd, cmd_line, "LIST", "150");

        new_sock_id = accept_ftp_server();
        if (new_sock_id > 0)
                printf("accept the server side.\n");
                
        /* begin to recive datas */
        while (1) {
                byte = ftp_recv_data(new_sock_id, data, MAX_DATA);
                if (byte == 0) {
                        break;
                }
                data[byte] = '\0';
                fprintf(stdout, "%s", data);
        }
        close(new_sock_id);
        
        byte = read(ftp_sock_fd, data, 100);
        if (byte == 0) {
                printf("read NULL.\n");
                return -1;
        }
        data[byte] = '\0';
        printf("%s", data);
        
        return 1;
}

void abstract_data_info(void)
{
        char *cp;
        int i = 0, j = 0;

        /* abstract ip */
        cp = strchr(session_tmp, '(');
        if (cp) {
                cp++;
                while (*cp) {
                        if (*cp == ',') {
                                j++;
                                if (j == 4)
                                        break;
                                ftp_data_ip[i++] = '.';
                                cp++;
                                continue;
                        }
                        else
                                ftp_data_ip[i++] = *cp++;
                }
                ftp_data_ip[i] = '\0';

                /* abstract port */
                cp++; i = 0;
                while (*cp != ',')
                        ftp_data_port1[i++] = *cp++;
                ftp_data_port1[i] = '\0';

                cp++; i = 0;
                while (*cp != ')')
                        ftp_data_port2[i++] = *cp++;
                ftp_data_port2[i] = '\0';
        }
}

/**
 * ftp command : ls
 * It based on PASV command, the server listen on a port and wait the client to connect.
 */
int ftp_do_ls_pasv(int ftp_sock_fd, char *cmd_line)
{
        int new_sock_id, byte;
        char data[MAX_DATA], tmp[128];

        /* first send PASV command */
        sprintf(session_tmp, "%s\r\n", "PASV");
        ftp_common_cmd(ftp_sock_fd, session_tmp, "PASV", "227");

        //read(ftp_sock_fd, session_tmp, MAX_DATA);
        abstract_data_info();

        /* compute the real port */
        ftp_data_port = atoi(ftp_data_port1) * 256 + atoi(ftp_data_port2);
        
        /* connect to the ftp server */
        new_sock_id = tcp_connect(make_network_ip(ftp_data_ip), htons(ftp_data_port), 2);
        if (new_sock_id < 0)
                return -1;
                
        /* then send LIST command */
        ftp_common_cmd(ftp_sock_fd, cmd_line, "LIST", "150");

        /* begin to recive datas */
        while (1) {
                byte = ftp_recv_data(new_sock_id, data, MAX_DATA);
                if (byte == 0)
                        break;
                data[byte] = '\0';
                fprintf(stdout, "%s", data);
        }
        close(new_sock_id);

        byte = read(ftp_sock_fd, data, 100);
        if (byte == 0) {
                printf("read NULL.\n");
                return -1;
        }
        data[byte] = '\0';
        printf("%s", data);

        return 1;
}

/**
 * ftp command: get
 * It based on PASV command, the server listen on a port and wait the client to connect.
 */
int ftp_do_get_pasv(int ftp_sock_fd, char *cmd_line)
{
        int fd, new_sock_id, byte;
        char server_file_name[128], local_file_name[128];
        char new_cmd_line[MAX_DATA], data[MAX_DATA];
        char *cp, *cp1;

        cp = strchr(cmd_line, ' ');
        /* the command like this 'get' */
        if (!cp) {
                /* get remote file name */
                printf("Remote file ");
                scanf("%s", server_file_name);
                sprintf(new_cmd_line, "%s %s", cmd_line, server_file_name);

                /* get local file name */
                printf("Local file ");
                scanf("%s", local_file_name);
        }
        else {
                cp++;
                cp1 = strchr(cp, ' ');
                /* the command like this 'get test.txt' */
                if (!cp1) {
                        sprintf(server_file_name, "%s", cp);
                        printf("Local file ");
                        scanf("%s", local_file_name);
                        sprintf(new_cmd_line, "%s %s", "RETR", server_file_name);
                }
                /* the command like this 'get test.txt /tmp/test1.txt' */
                else {
                        sscanf(cmd_line, "%s %s %s", new_cmd_line,
                                server_file_name, local_file_name);
                        sprintf(new_cmd_line, "%s %s", "RETR", server_file_name);
                }
        }

        /* first send PASV command */
        sprintf(session_tmp, "%s\r\n", "PASV");
        ftp_common_cmd(ftp_sock_fd, session_tmp, "PASV", "227");

        //read(ftp_sock_fd, session_tmp, MAX_DATA);
        abstract_data_info();

        /* compute the real port */
        ftp_data_port = atoi(ftp_data_port1) * 256 + atoi(ftp_data_port2);
        
        /* connect to the ftp server */
        new_sock_id = tcp_connect(make_network_ip(ftp_data_ip), htons(ftp_data_port), 2);
        if (new_sock_id < 0)
                return -1;
        
        /* create a local file */
        fd = creat(local_file_name, 0777);
        if (fd < 0)
                return -1;
        DPRINT("%s", "file create ok.\n");

        /* then send RETR command */
        if (ftp_common_cmd(ftp_sock_fd, new_cmd_line, "RETR", "150") == -1)
                return -1;

        /* begin to recive datas */
        while (1) {
                byte = ftp_recv_data(new_sock_id, data, MAX_DATA);
                if (byte == 0)
                        break;
                ftp_send_data(fd, data, byte);
        }
        close(new_sock_id);
        close(fd);

        byte = read(ftp_sock_fd, data, 100);
        if (byte == 0) {
                printf("read NULL.\n");
                return -1;
        }
        data[byte] = '\0';
        printf("%s", data);

        return 1;
}

/**
 * ftp command: put
 *
 * It based on PORT command, the client listen on a port and wait the server to connect.
 */
int ftp_do_put_port(int ftp_sock_fd, char *cmd_line)
{
        int fd, new_sock_id, byte;
        char server_file_name[128], local_file_name[128];
        char new_cmd_line[MAX_DATA], data[MAX_DATA];
        char *cp, *cp1;

        local_port++;
        sprintf(session_tmp, "PORT %s,%d,%d\r\n",
                        local_ip, (local_port - local_port % 256 ) / 256,
                        local_port % 256);

        if (ftp_common_cmd(ftp_sock_fd, session_tmp, "PORT", "200") == -1)
                return -1;

        cp = strchr(cmd_line, ' ');
        /* the command like this 'put' */
        if (!cp) {
                printf("Local file ");
                scanf("%s", local_file_name);

                printf("Remote file ");
                scanf("%s", server_file_name);
                sprintf(new_cmd_line, "%s %s", cmd_line, server_file_name);
        }
        else {
                cp++;
                cp1 = strchr(cp, ' ');
                /* the command like this 'put test.txt' */
                if (!cp1) {
                        sprintf(local_file_name, "%s", cp);
                        printf("Remote file ");
                        scanf("%s", server_file_name);
                        sprintf(new_cmd_line, "%s %s", "STOR", server_file_name);
                }
                /* the command like this 'put test.txt /tmp/test1.txt' */
                else {
                        sscanf(cmd_line, "%s %s %s", new_cmd_line,
                                local_file_name, server_file_name);
                        sprintf(new_cmd_line, "%s %s", "STOR", server_file_name);
                }
        }

        fd = open(local_file_name, O_RDONLY);
        if (fd < 0)
                return -1;
        DPRINT("%s", "open file ok.\n");

        if (ftp_common_cmd(ftp_sock_fd, new_cmd_line, "STOR", "150") == -1)
                return -1;

        new_sock_id = accept_ftp_server();
        if (new_sock_id > 0)
                DPRINT("%s", "accept the server side.\n");

        while (1) {
                byte = ftp_recv_data(fd, data, MAX_DATA);
                if (byte == 0)
                        break;
                ftp_send_data(new_sock_id, data, byte);
        }
        close(new_sock_id);
        close(fd);

        byte = read(ftp_sock_fd, data, 100);
        if (byte == 0) {
                printf("read NULL.\n");
                return -1;
        }
        data[byte] = '\0';
        fprintf(stdout, "%s", data);

        return 1;
}

/**
 * ftp command: put
 *
 * It based on PORT command, the server listen on a port and wait the client to connect.
 */
int ftp_do_put_pasv(int ftp_sock_fd, char *cmd_line)
{
        int fd, new_sock_id, byte;
        char server_file_name[128], local_file_name[128];
        char new_cmd_line[MAX_DATA], data[MAX_DATA];
        char *cp, *cp1;

        local_port++;
        sprintf(session_tmp, "PORT %s,%d,%d\r\n",
                        local_ip, (local_port - local_port % 256 ) / 256,
                        local_port % 256);

        if (ftp_common_cmd(ftp_sock_fd, session_tmp, "PORT", "200") == -1)
                return -1;

        cp = strchr(cmd_line, ' ');
        /* the command like this, 'put' */
        if (!cp) {
                printf("Local file ");
                scanf("%s", local_file_name);

                printf("Remote file ");
                scanf("%s", server_file_name);
                sprintf(new_cmd_line, "%s %s", cmd_line, server_file_name);
        }
        else {
                cp++;
                cp1 = strchr(cp, ' ');
                /* the command like this 'get test.txt' */
                if (!cp1) {
                        sprintf(local_file_name, "%s", cp);
                        printf("Remote file ");
                        scanf("%s", server_file_name);
                        sprintf(new_cmd_line, "%s %s", "STOR", server_file_name);
                }
                /* the command like this 'get test.txt /tmp/test1.txt' */
                else {
                        sscanf(cmd_line, "%s %s %s", new_cmd_line,
                                local_file_name, server_file_name);
                        sprintf(new_cmd_line, "%s %s", "STOR", server_file_name);
                }
        }

        /* first send PASV command */
        sprintf(session_tmp, "%s\r\n", "PASV");
        ftp_common_cmd(ftp_sock_fd, session_tmp, "PASV", "227");

        abstract_data_info();

        /* compute the real port */
        ftp_data_port = atoi(ftp_data_port1) * 256 + atoi(ftp_data_port2);

        /* connect to the ftp server */
        new_sock_id = tcp_connect(make_network_ip(ftp_data_ip), htons(ftp_data_port), 2);
        if (new_sock_id < 0)
                return -1;

        fd = open(local_file_name, O_RDONLY);
        if (fd < 0) {
                fprintf(stdout, "%s: File not found\n", local_file_name);
                return -1;
        }
        DPRINT("%s", "open file ok.\n");
                
        if (ftp_common_cmd(ftp_sock_fd, new_cmd_line, "STOR", "150") == -1)
                return -1;

        while (1) {
                byte = ftp_recv_data(fd, data, MAX_DATA);
                if (byte == 0)
                        break;
                ftp_send_data(new_sock_id, data, byte);
        }
        close(new_sock_id);
        close(fd);

        byte = read(ftp_sock_fd, data, 100);
        if (byte == 0) {
                printf("read NULL.\n");
                return -1;
        }
        data[byte] = '\0';
        fprintf(stdout, "%s", data);

        return 1;
}

/* ftp command : mkdir */
int ftp_do_mkdir(int ftp_sock_fd, char *cmd_line)
{
        ftp_common_cmd(ftp_sock_fd, cmd_line, "MKD", "257");
}

/* ftp command: rmdir */
int ftp_do_rmdir(int ftp_sock_fd, char *cmd_line)
{
        ftp_common_cmd(ftp_sock_fd, cmd_line, "RMD", "257");
}

/* ftp command: cd */
int ftp_do_cd(int ftp_sock_fd, char *cmd_line)
{
        ftp_common_cmd(ftp_sock_fd, cmd_line, "CWD", "250");
}

/* ftp command: pwd */
int ftp_do_pwd(int ftp_sock_fd, char *cmd_line)
{
        ftp_common_cmd(ftp_sock_fd, cmd_line, "PWD", "257");
}

/* ftp command: delete */
int ftp_do_delete(int ftp_sock_fd, char *cmd_line)
{
        ftp_common_cmd(ftp_sock_fd, cmd_line, "DELE", "250");
}

/* ftp command: ! */
int ftp_do_exit(int ftp_sock_fd, char *cmd_line)
{
        if (ftp_sock)
                ftp_common_cmd(ftp_sock_fd, cmd_line, "QUIT", "221");
        close(ftp_sock_fd);

        exit(0);
}

/* ftp command: bye */
int ftp_do_bye(int ftp_sock_fd, char *cmd_line)
{
        if (ftp_sock)
                ftp_common_cmd(ftp_sock_fd, cmd_line, "QUIT", "221");
        close(ftp_sock_fd);

        exit(0);
}

/* ftp command: quit */
int ftp_do_quit(int ftp_sock_fd, char *cmd_line)
{
        if (ftp_sock)
                ftp_common_cmd(ftp_sock_fd, cmd_line, "QUIT", "221");
        close(ftp_sock_fd);

        exit(0);
}

void ftp_client_usage(char *pro)
{
        fprintf(stdout, "%s V %2.1f\tby wzt\n",
                BANNER, VERSION);
        //fprintf(stdout, "\n%s\n", BANNER1);
        fprintf(stdout, "\n%s [host] [port]\n", pro);
}

int main(int argc, char **argv)
{
        int ret;

 //       ftp_client_init();
        if (argc == 1)
                do_ftp_client();
                
        if (argc > 3) {
                ftp_client_usage(argv[0]);
                exit(0);
        }

        g_ftp_ip = make_network_ip(argv[1]);
        if (g_ftp_ip == 0) {
                fprintf(stderr, "Unknown host %s.\n", argv[1]);
                ftp_sock = 0;
                do_ftp_client();
        }
        
        if (argc == 2)
                g_ftp_port = htons(FTP_PORT);
        else
                g_ftp_port = htons(atoi(argv[2]));
                
        ftp_sock = connect_ftp_server(g_ftp_ip, g_ftp_port);
        
        if (ftp_sock == -1) {
                fprintf(stderr, "> ftp: connect: time out.\n");
                ftp_sock = 0;
                do_ftp_client();
        }
        fprintf(stderr, "Connected to %s.\n", argv[1]);

        ret = auth_ftp_user(ftp_sock, argv[1]);
        if (ret == -1) {
                ftp_sock = 0;
                do_ftp_client();
        }
        
        do_ftp_client();
        
        return 0;
}



