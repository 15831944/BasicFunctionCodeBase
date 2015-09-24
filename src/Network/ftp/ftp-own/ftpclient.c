
/**
    ftp client c program from busybox
**/

/** vi: set sw=4 ts=4: **/
/**
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
 **/
 
 
//#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <mntent.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <getopt.h>
#include <limits.h>


#define FTP_HEAD_SIZE 512
/* We hijack this constant to mean something else */
/* It doesn't hurt because we will remove this bit anyway */
#define DIE_ON_ERROR AI_CANONNAME
#define isdigit(a) ((unsigned)((a) - '0') <= 9)
//#define xatou(rest) xatoull##rest
#define LONE_DASH(s)     ((s)[0] == '-' && !(s)[1])
#define NOT_LONE_DASH(s) ((s)[0] != '-' || (s)[1])
#define ENABLE_FEATURE_IPV6 0
#define type long
#define xstrtou(rest) xstrtoul##rest
#define xstrto(rest)  xstrtol##rest
#define xatou(rest)   xatoul##rest
#define xato(rest)    xatol##rest
#define XSTR_UTYPE_MAX  ULONG_MAX
#define XSTR_TYPE_MAX   LONG_MAX
#define XSTR_TYPE_MIN   LONG_MIN
#define XSTR_STRTOU     strtoul
//#include "ftp_template.c"
#define bb_error_msg_and_die(fmt,args,...)  \
    do{ \
        printf ("%s( %d): \"fmt\"\n" ,  __FILE__, __LINE__,##args);  \
        exit(-1); \
    }while(0)

#define bb_error_msg(fmt,args,...)   printf ("%s( %d): "fmt , __FILE__, __LINE__,##args)





typedef signed char smallint;
typedef struct len_and_sockaddr {
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
    };
} len_and_sockaddr; 
typedef struct ftp_host_info_s {
    const char *user;
    const char *password;
    struct len_and_sockaddr *lsa;
} ftp_host_info_t; 
static smallint verbose_flag=1;
static smallint do_continue;






char * safe_strncpy(char *dst, const char *src, size_t size)
{
    if (!size) 
        return dst;
    dst[--size] = '\0';
    return strncpy(dst, src, size);
}


int xatou_0(char *buf)   
{   
    char c;   
    int i;   
    int j = 0;   
    int retval = 0;   
    char mod[3] = {1, 10, 100};   
   
    int len = strlen(buf);   
    if ( (!len)||(len > 3) )   
    {   
        return -1;   
    }      
   
    for (i = len - 1; i >= 0; i--)   
    {   
        c = buf[i];   
        retval += atoi(&c)*mod[j++];   
    }   
   
    return retval;   
}   
   
int xatoul_range(char *buf, int low, int top)   
{   
    int retval = xatou_0(buf);   
    if (retval < low)   
    {   
        retval = low;   
    }      
   
    if (retval > top)   
    {   
        retval = top;   
    }   
    printf("buf = %s\n", buf);     
    printf("###retval = %d\n", retval);   
    return retval;   
}   
   
   
uint16_t xatou16(const char *numstr)
{
    return (uint16_t)xatoul_range((char*)numstr, 0, 0xffff);
}

void set_nport(len_and_sockaddr *lsa, unsigned port)
{
    if (lsa->sa.sa_family == AF_INET) {
        lsa->sin.sin_port = port;
        return;
    }
    /* What? UNIX socket? IPX?? :) */
}

void *xmalloc(int size)
{
    void *ptr;
    ptr = malloc(size);
    if (ptr == NULL && size != 0){
        //bb_error_msg_and_die("memory exhausted");
        printf("memory exhausted\n");
        exit (-1);
    }
    return ptr;
}

static len_and_sockaddr* str2sockaddr(const char *host, int port, int ai_flags)
{
    int rc;
    len_and_sockaddr *r = NULL;
    struct addrinfo *result = NULL;
    const char *org_host = host; /* only for error msg */
    const char *cp;
    struct addrinfo hint;
    /* Ugly parsing of host:addr */
    if (ENABLE_FEATURE_IPV6 && host[0] == '[') {
        host++;
        cp = strchr(host, ']');
        if (!cp || cp[1] != ':') { /* Malformed: must have [xx]:nn */
            //bb_error_msg_and_die("bad address '%s'", org_host);
            printf("bad address '%s'", org_host);
            exit (-1);
        }
		//return r; /* return NULL */
    } else {
        cp = strrchr(host, ':');
        if (ENABLE_FEATURE_IPV6 && cp && strchr(host, ':') != cp) {
            /* There is more than one ':' (e.g. "::1") */
            cp = NULL; /* it's not a port spec */
        }
    }
	
    if (cp) {
        int sz = cp - host + 1;
        host = safe_strncpy(alloca(sz), host, sz);
        if (ENABLE_FEATURE_IPV6 && *cp != ':')
            cp++; /* skip ']' */
        cp++; /* skip ':' */
        port = xatou16(cp);
    }
    memset(&hint, 0 , sizeof(hint));
#if !ENABLE_FEATURE_IPV6
    hint.ai_family = AF_INET; /* do not try to find IPv6 */
#else
    hint.ai_family = af;
#endif
    /* Needed. Or else we will get each address thrice (or more) for each possible socket type (tcp,udp,raw...): */
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = ai_flags & ~DIE_ON_ERROR;
    rc = getaddrinfo(host, NULL, &hint, &result);
    if (rc || !result) {
        //bb_error_msg("bad address '%s'", org_host);
        printf("bad address '%s'", org_host);
        if (ai_flags & DIE_ON_ERROR)
            exit(-1);
        goto ret;
    }
    r = xmalloc(offsetof(len_and_sockaddr, sa) + result->ai_addrlen);
    r->len = result->ai_addrlen;
    memcpy(&r->sa, result->ai_addr, result->ai_addrlen);
    set_nport(r, htons(port));
ret:
    freeaddrinfo(result);
    return r;
}

int xsocket(int nDomain, int nType, int nProtocol)
{
    int r;
    r = socket(nDomain, nType, nProtocol);
    if (r < 0) {
        /* Hijack vaguely related config option */
        //bb_error_msg_and_die("socket");
        printf("socket\n");
        exit (-1);
    }
    return r;
}

void xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)
{
    if (connect(s, s_addr, addrlen) < 0) {
        close(s);
        if (s_addr->sa_family == AF_INET){
            //bb_error_msg_and_die("%s (%s)", "cannot connect to remote host", inet_ntoa(((struct sockaddr_in *)s_addr)->sin_addr));
            printf("%s (%d)", "cannot connect to remote host", inet_ntoa(((struct sockaddr_in *)s_addr)->sin_addr));
            exit (-1);
        }
		//bb_error_msg_and_die("cannot connect to remote host");
    }
}

int xconnect_stream(const len_and_sockaddr *lsa)
{
    int fd = xsocket(lsa->sa.sa_family, SOCK_STREAM, 0);
    xconnect(fd, &lsa->sa, lsa->len);
    return fd;
}

int xopen(const char *pathname, int flags, int mode)
{
    int ret;
    ret = open(pathname, flags, mode);
    if (ret < 0) {
        //bb_error_msg_and_die("can't open '%s'", pathname);
        printf("can't open '%s'", pathname);
        exit (-1);
    }
    return ret;
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
    ssize_t n;
    do {
        n = read(fd, buf, count);
    } while (n < 0 && errno == EINTR);
    return n;
}

ssize_t safe_write(int fd, const void *buf, size_t count)
{
    ssize_t n;
    do {
        n = write(fd, buf, count);
    } while (n < 0 && errno == EINTR);
    return n;
}

/**
 * Write all of the supplied buffer out to a file. This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 **/
ssize_t full_write(int fd, const void *buf, size_t len)
{
    ssize_t cc;
    ssize_t total;
    total = 0;
    while (len) {
        cc = safe_write(fd, buf, len);
        if (cc < 0)
            return cc;  /* write() returns -1 on failure. */
        total += cc;
        buf = ((const char *)buf) + cc;
        len -= cc;
    }
    return total;
}

static off_t bb_full_fd_action(int src_fd, int dst_fd, off_t size)
{
    int status = -1;
    off_t total = 0;
    char buffer[BUFSIZ];
	
    if (src_fd < 0)
        goto out;
		
    if (!size) {
        size = BUFSIZ;
        status = 1; /* copy until eof */
    }
	
    while (1) {
        ssize_t rd;
        rd = safe_read(src_fd, buffer, size > BUFSIZ ? BUFSIZ : size);
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
        if (dst_fd >= 0) {
            ssize_t wr = full_write(dst_fd, buffer, rd);
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
	
out:
    return status ? -1 : total;
}

off_t bb_copyfd_size(int fd1, int fd2, off_t size)
{
    if (size) {
        return bb_full_fd_action(fd1, fd2, size);
    }
    return 0;
}

off_t bb_copyfd_eof(int fd1, int fd2)
{
    return bb_full_fd_action(fd1, fd2, 0);
}

static void ftp_die(const char *msg, const char *remote)
{
    /* Guard against garbage from remote server */
    const char *cp = remote;
    while (*cp >= ' ' && *cp < '0x7f') 
		cp++;
    //bb_error_msg_and_die("unexpected server response%s%s: %.*s", msg ? " to " : "", msg ? msg : "", (int)(cp - remote), remote);
    printf("unexpected server response%s%s: %.*s", msg ? " to " : "", msg ? msg : "", (int)(cp - remote), remote);
    exit(-1);
}

static int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
    printf("ftpcmd++++++++cmd %s %s\n", s1, s2);
    unsigned n;
    if (verbose_flag) {
        //bb_error_msg("cmd %s %s", s1, s2);
        printf("cmd %s %s\n", s1, s2);
    }
    if (s1) {
        if (s2) {
            fprintf(stream, "%s %srn", s1, s2);
        } else {
            fprintf(stream, "%srn", s1);
        }
    }
    printf("ftpcmd++++111111111111++++cmd %s %s\n", s1, s2);
    do {
        char *buf_ptr;
        if (fgets(buf, 510, stream) == NULL) {
            //bb_error_msg_and_die("fgets");
            printf("fgets\n");
            exit(-1);
        }
        printf("ftpcmd+++22222222222+++++cmd %s %s\n", s1, s2);
        buf_ptr = strstr(buf, "rn");
        if (buf_ptr) {
            *buf_ptr = '\0';
            printf("--%sn", buf);
        }
        printf("ftpcmd+++33333333333+++++cmd %s %s\n", s1, s2);
    } while (!isdigit(buf[0]) || buf[3] != ' ');
    buf[3] = '\0';
	//n = xatou(buf);
    n = atol(buf);
    buf[3] = '\0';
    printf("ftpcmd=================cmd %s %s\n", s1, s2);
    return n;
}

static int xconnect_ftpdata(ftp_host_info_t *server, char *buf)
{
    printf("xconnect_ftpdata++++++++++++++++++++++\n");
    char *buf_ptr;
    unsigned short port_num;
    /* Response is "NNN garbageN1,N2,N3,N4,P1,P2[)garbage]
     * Server's IP is N1.N2.N3.N4 (we ignore it)
     * Server's port for data connection is P1*256+P2 */
    buf_ptr = strrchr(buf, ')');
    if (buf_ptr) *buf_ptr = '\0';
    buf_ptr = strrchr(buf, ',');
    *buf_ptr = '\0';
    port_num = xatoul_range(buf_ptr + 1, 0, 255);
    buf_ptr = strrchr(buf, ',');
    *buf_ptr = '\0';
    port_num += xatoul_range(buf_ptr + 1, 0, 255) * 256;
    set_nport(server->lsa, htons(port_num));
     printf("xconnect_ftpdata============\n");
    return xconnect_stream(server->lsa);
}

static FILE *ftp_login(ftp_host_info_t *server)
{
    printf("ftp_login++++++++++++++++++++\n");
    FILE *control_stream;
    char buf[512];
    /* Connect to the command socket */
    control_stream = fdopen(xconnect_stream(server->lsa), "r+");
    if (control_stream == NULL) {
        /* fdopen failed - extremely unlikely */
        //bb_error_msg_and_die("ftp login");
        printf("ftp login");
        exit (-1);
    }
	printf("ftp_login++11111111111111111111++++++\n");
    if (ftpcmd(NULL, NULL, control_stream, buf) != 220) {
        printf("ftp_die++\n");
        //ftp_die(NULL, buf);
    }
	printf("ftp_login+++++++2222222222222222222++++\n");
    /*  Login to the server */
    switch (ftpcmd("USER", server->user, control_stream, buf)) {
        case 230:
            printf("ftp_login++++230++++++\n");
            break;
        case 331:
            printf("ftp_login+++++++331++++++++\n");
            if (ftpcmd("PASS", server->password, control_stream, buf) != 230) {
                ftp_die("PASS", buf);
                printf("ftp_login+++ftp_die++++PAS+++++\n");
            }
            break;
        default:
            printf("ftp_login++++++default+++++++\n");
            ftp_die("USER", buf);
    }
    ftpcmd("TYPE I", NULL, control_stream, buf);
    printf("ftp_login==================\n");
    return control_stream;
}

static int ftp_receive(ftp_host_info_t *server, FILE *control_stream, const char *local_path, char *server_path)
{
    char buf[512];
    /* I think 'filesize' usage here is bogus. Let's see... */
	//off_t filesize = -1;
	#define filesize ((off_t)-1)
    int fd_data;
    int fd_local = -1;
    off_t beg_range = 0;
    /* Connect to the data socket */
    if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
        ftp_die("PASV", buf);
    }
	
    fd_data = xconnect_ftpdata(server, buf);
    if (ftpcmd("SIZE", server_path, control_stream, buf) == 213) {
		//filesize = BB_STRTOOFF(buf + 4, NULL, 10);
		//if (errno || filesize < 0)
		//  ftp_die("SIZE", buf);
    } else {
        do_continue = 0;
    }
	
    if (LONE_DASH(local_path)) {
        fd_local = STDOUT_FILENO;
        do_continue = 0;
    }
	
    if (do_continue) {
        struct stat sbuf;
        if (lstat(local_path, &sbuf) < 0) {
            //bb_error_msg_and_die("lstat");
            printf("lstat error.");
            exit -1;
        }
        if (sbuf.st_size > 0) {
            beg_range = sbuf.st_size;
        } else {
            do_continue = 0;
        }
    }
	
    if (do_continue) {
        sprintf(buf, "REST %""1""d", (int)beg_range);
        if (ftpcmd(buf, NULL, control_stream, buf) != 350) {
            do_continue = 0;
        } else {
			//if (filesize != -1)
			//  filesize -= beg_range;
        }
    }
	
    if (ftpcmd("RETR", server_path, control_stream, buf) > 150) {
        ftp_die("RETR", buf);
    }
	
    /* only make a local file if we know that one exists on the remote server */
    if (fd_local == -1) {
        if (do_continue) {
            fd_local = xopen(local_path, O_APPEND | O_WRONLY, 0666);
        } else {
            fd_local = xopen(local_path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        }
    }
	
    /* Copy the file */
    if (filesize != -1) {
        if (bb_copyfd_size(fd_data, fd_local, filesize) == -1)
            return EXIT_FAILURE;
    } else {
        if (bb_copyfd_eof(fd_data, fd_local) == -1)
            return EXIT_FAILURE;
    }
	
    /* close it all down */
    close(fd_data);
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die(NULL, buf);
    }
    ftpcmd("QUIT", NULL, control_stream, buf);
	
    return EXIT_SUCCESS;
}

static int ftp_receive_head(FILE *control_stream, int *pnFdData, int *pnFileSize, ftp_host_info_t *server, 
							char *server_path, char caFtpHead[FTP_HEAD_SIZE])
{
    printf("ftp_receive_head++++++++++++++++++++++\n");
    char buf[512];
    int rd;
	
    /* Connect to the data socket */
    if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
        ftp_die("PASV", buf);
    }
	
    *pnFdData = xconnect_ftpdata(server, buf);
    if (*pnFdData < 0)
        return -1;
		
    if (ftpcmd("SIZE", server_path, control_stream, buf) == 213) {
        *pnFileSize = strtol(buf + 4, NULL, 10);
        if (*pnFileSize < 0)
            ftp_die("SIZE", buf);
    } else {
        do_continue = 0;
    }
	
    if (ftpcmd("RETR", server_path, control_stream, buf) > 150) {
        ftp_die("RETR", buf);
    }
    rd = safe_read(*pnFdData, caFtpHead, FTP_HEAD_SIZE);
	printf("ftp_receive_head=============\n");
    return rd;
}

static int ftp_send(ftp_host_info_t *server, FILE *control_stream, const char *server_path, char *local_path)
{
    struct stat sbuf;
    char buf[512];
    int fd_data;
    int fd_local;
    int response;
	
    /*  Connect to the data socket */
    if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
        ftp_die("PASV", buf);
    }
    fd_data = xconnect_ftpdata(server, buf);
	
    /* get the local file */
    fd_local = STDIN_FILENO;
    if (NOT_LONE_DASH(local_path)) {
        fd_local = xopen(local_path, O_RDONLY,0666);
        fstat(fd_local, &sbuf);
        sprintf(buf, "ALLO %lu", (long unsigned int)sbuf.st_size);
        response = ftpcmd(buf, NULL, control_stream, buf);
        switch (response) {
            case 200:
            case 202:
                break;
            default:
                close(fd_local);
                ftp_die("ALLO", buf);
                break;
        }
    }
	
    response = ftpcmd("STOR", server_path, control_stream, buf);
    switch (response) {
        case 125:
        case 150:
            break;
        default:
            close(fd_local);
            ftp_die("STOR", buf);
    }
	
    /* transfer the file  */
    if (bb_copyfd_eof(fd_local, fd_data) == -1) {
        exit(EXIT_FAILURE);
    }
	
    /* close it all down */
    close(fd_data);
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die("close", buf);
    }
    ftpcmd("QUIT", NULL, control_stream, buf);
	
    return EXIT_SUCCESS;
}


#define FTPGETPUT_OPT_CONTINUE  1
#define FTPGETPUT_OPT_VERBOSE   2
#define FTPGETPUT_OPT_USER  4
#define FTPGETPUT_OPT_PASSWORD  8
#define FTPGETPUT_OPT_PORT  16

FILE* ftp_get_head(const char *host, int port, const char *user, const char *password, char *server_path, 
				char caFtpHead[FTP_HEAD_SIZE], int *pnHeadSize, int *pnFdData, FILE *ctrl_stream, int *pnFileSize)
{
    printf("ftp_get_head++++++++++++++++++++++\n");
    ftp_host_info_t server;
    FILE *control_stream;
    char caPort[10];
    server.user = user;
    server.password = password;
    server.lsa = str2sockaddr(host, port, DIE_ON_ERROR);
    printf("ftp_get_head===1111111111111=======\n");
    control_stream = ftp_login(&server);
    printf("ftp_get_head====2222222222222======\n");
    memcpy(ctrl_stream, control_stream, sizeof(FILE));
    *pnHeadSize = ftp_receive_head(control_stream, pnFdData, pnFileSize, &server, server_path, caFtpHead);
	printf("ftp_get_head====3333333333======\n");
    return control_stream;
}

int ftp_recv_to_file(int nFdData, FILE *control_stream, char caFtpHead[FTP_HEAD_SIZE], int nHeadSize, const char *pcLocalFileName)
{
    int fd_local = -1;
    char buf[512];
    int status = -1;
    off_t total = 0;
    char buffer[BUFSIZ];
    int size=0;
    fd_local = xopen(pcLocalFileName, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	
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
        rd = safe_read(nFdData, buffer, size > BUFSIZ ? BUFSIZ : size);
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
    close(nFdData);
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die(NULL, buf);
    }
    ftpcmd("QUIT", NULL, control_stream, buf);
	
out:
    return status ? -1 : total;
}

int ftp_recv_to_ram(int nFdData, FILE *control_stream, char caFtpHead[FTP_HEAD_SIZE], int nHeadSize, char *pcRam)
{
    int fd_local = -1;
    char buf[512];
    int status = -1;
    off_t total = 0;
    char buffer[BUFSIZ];
    int size=0;
    memcpy(pcRam, caFtpHead, nHeadSize);
    total += nHeadSize;
	
    while (1) {
        ssize_t rd;
        rd = safe_read(nFdData, buffer, BUFSIZ);
		//printf("-----rd =%d----n", rd);
        if (!rd) { /* eof - all done */
            status = 0;
            break;
        }
		
        if (rd < 0) {
			//bb_error_msg(bb_msg_read_error);
            break;
        }
		
        memcpy(pcRam+total, buffer, rd);
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
    close(nFdData);
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die(NULL, buf);
    }
    ftpcmd("QUIT", NULL, control_stream, buf);
	
    return status ? -1 : total;
}



int main()
{
    /* content-length of the file */
    unsigned opt;
    const char *port = "ftp";
    ftp_host_info_t *server;
    /* socket to ftp server */
    FILE *control_stream;
    static FILE ctrl_stream;
    /* continue previous transfer (-c) */
    char caFtpHead[FTP_HEAD_SIZE];
    int nFileSize;
    int rd = 0;
    int nFdData;
    int nSave;
    char *pcRam;
	
    //FILE* ftp_get_head(const char *host, int port, const char *user, const char *password, char *server_path, 
	//			char caFtpHead[FTP_HEAD_SIZE], int *pnHeadSize, int *pnFdData, FILE *ctrl_stream, int *pnFileSize)
    control_stream = ftp_get_head("192.168.1.170", 21, "sun", "123456", "sip11.cfg", 
									caFtpHead, &rd, &nFdData, &ctrl_stream, &nFileSize);
    caFtpHead[rd-1] = '\0';
    printf("-------rd=%d===nFileSize=%d===\n", rd, nFileSize);
    printf("-------------------------n %s n------------------------\n",caFtpHead);
	
	//nSave = ftp_recv_to_file(nFdData, control_stream,
	//caFtpHead, rd,"test.cfg");
    pcRam = (char*)calloc(sizeof(char), nFileSize);
    if(NULL == pcRam) {
        perror("calloc error\n");
        exit(-1);
    }
	
    nSave = ftp_recv_to_ram(nFdData, control_stream, caFtpHead, rd, pcRam);
    printf("nSave =%d \n", nSave);
    pcRam[200] = '\0';
    printf("-------------------------n %s n------------------------\n",pcRam+130000);
    return 0;
	
	
    /* Set default values */
    server = xmalloc(sizeof(*server));
    server->user = "xyx";
    server->password = "123456";
	
    /* We want to do exactly _one_ DNS lookup, since some sites (i.e. ftp.us.debian.org) use round-robin DNS
     * and we want to connect to only one IP... */
    server->lsa = str2sockaddr("192.168.0.139", 21, DIE_ON_ERROR);
    //printf("BUFSIZ=%d  off_t = %d\n", BUFSIZ, (off_t)-1);
	
    /* Connect/Setup/Configure the FTP session */
    control_stream = ftp_login(server);
    ftp_receive(server, control_stream, "ram.bin", "ram_zimage.bin");
    return 0;
}


