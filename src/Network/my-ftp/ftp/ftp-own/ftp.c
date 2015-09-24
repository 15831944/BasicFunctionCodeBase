#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <errno.h>
#include <assert.h>
#include <netinet/in.h>

#define TRACE() printf("%s %d\n", __FUNCTION__, __LINE__);


#define OFF_FMT "l"
enum {
    LSA_SIZEOF_SA = sizeof(
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
#if ENABLE_FEATURE_IPV6
        struct sockaddr_in6 sin6;
#endif
    }
                    )
};

typedef struct len_and_sockaddr {
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
#if ENABLE_FEATURE_IPV6
        struct sockaddr_in6 sin6;
#endif
    };
} len_and_sockaddr;

typedef struct ftp_host_info_s {
    const char *user;
    const char *password;
    struct len_and_sockaddr *lsa;
} ftp_host_info_t;
#if 0
static smallint verbose_flag;
static smallint do_continue;
#endif
/******************************************************************/
#define bb_perror_msg(x) printf(x)
#define bb_msg_read_error "Error: bb_msg_read_error"
#define bb_msg_write_error "Error: bb_msg_write_error"
#define NOT_LONE_DASH(s) ((s)[0] != '-' || (s)[1])
#define bb_perror_msg_and_die printf
#define bb_error_msg_and_die printf
#define bb_perror_nomsg_and_die() assert(0);
int xatou(char *buf)
{
    char c;
    int i;
    int j = 0;
    int retval = 0;
    char mod[3] = {1, 10, 100};

    int len = strlen(buf);
    if ( (!len) || (len > 3) ) {
        return -1;
    }

    for (i = len - 1; i >= 0; i--) {
        c = buf[i];
        retval += atoi(&c) * mod[j++];
    }

    return retval;
}

int xatoul_range(char *buf, int low, int top)
{
    int retval = xatou(buf);
    if (retval < low) {
        retval = low;
    }

    if (retval > top) {
        retval = top;
    }
    printf("buf = %s\n", buf);
    printf("###retval = %d\n", retval);
    return retval;
}

len_and_sockaddr *xhost2sockaddr(const char *ip_addr, int port)
{
    int rc;
    len_and_sockaddr *r = NULL;
    struct addrinfo *result = NULL;
    struct addrinfo hint;

    //r = malloc(offsetof(len_and_sockaddr, sa) + result->ai_addrlen);
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    //hint.ai_flags = ai_flags & ~DIE_ON_ERROR;

    rc = getaddrinfo(ip_addr, NULL, &hint, &result);
    if (rc || !result) {
        free(r);
        r = NULL;
        return r;
    }

    r = malloc(4 + result->ai_addrlen);
    if (r == NULL) {
        return NULL;
    }

    /*  typedef struct len_and_sockaddr {
        socklen_t len;
        union {
            struct sockaddr sa;
            struct sockaddr_in sin;
    #if ENABLE_FEATURE_IPV6
            struct sockaddr_in6 sin6;
    #endif
        };
    } len_and_sockaddr;

    len_and_sockaddr *r = NULL;

    struct addrinfo *result = NULL;
    */

    r->len = result->ai_addrlen;
    memcpy(&r->sa, result->ai_addr, result->ai_addrlen);
    r->sin.sin_port = htons(port);

    freeaddrinfo(result);
    return r;
}

/******************************************************************/

/******************************************************************/
#define IGNORE_PORT NI_NUMERICSERV
static char *sockaddr2str(const struct sockaddr *sa, int flags)
{
    char host[128];
    char serv[16];
    int rc;
    socklen_t salen;

    salen = LSA_SIZEOF_SA;
#if ENABLE_FEATURE_IPV6
    if (sa->sa_family == AF_INET)
        salen = sizeof(struct sockaddr_in);
    if (sa->sa_family == AF_INET6)
        salen = sizeof(struct sockaddr_in6);
#endif
    rc = getnameinfo(sa, salen,
                     host, sizeof(host),
                     /* can do ((flags & IGNORE_PORT) ? NULL : serv) but why bother? */
                     serv, sizeof(serv),
                     /* do not resolve port# into service _name_ */
                     flags | NI_NUMERICSERV
                    );
    if (rc)
        return NULL;
    if (flags & IGNORE_PORT)
        return strdup(host);
#if ENABLE_FEATURE_IPV6
    if (sa->sa_family == AF_INET6) {
        if (strchr(host, ':')) /* heh, it's not a resolved hostname */
            return xasprintf("[%s]:%s", host, serv);
        /*return xasprintf("%s:%s", host, serv);*/
        /* - fall through instead */
    }
#endif
    /* For now we don't support anything else, so it has to be INET */
    /*if (sa->sa_family == AF_INET)*/
    char *retmsg = malloc(2048);
    memset(retmsg, 0, 2048);
    sprintf(retmsg, "%s:%s", host, serv);
    return retmsg;
    /*return xstrdup(host);*/
}

char *xmalloc_sockaddr2dotted(const struct sockaddr *sa)
{
    return sockaddr2str(sa, NI_NUMERICHOST);
}

int xopen3(const char *pathname, int flags, int mode)
{
    int ret;

    ret = open(pathname, flags, mode);
    if (ret < 0) {
        bb_perror_msg_and_die("can't open '%s'", pathname);
    }
    return ret;
}

int xopen(const char *pathname, int flags)
{
    return xopen3(pathname, flags, 0666);
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


size_t full_write(int fd, const void *buf, size_t len)
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

        if (!rd) { /* eof - all done */
            status = 0;
            break;
        }
        if (rd < 0) {
            bb_perror_msg(bb_msg_read_error);
            break;
        }
        /* dst_fd == -1 is a fake, else... */
        if (dst_fd >= 0) {
            ssize_t wr = full_write(dst_fd, buffer, rd);
            if (wr < rd) {
                bb_perror_msg(bb_msg_write_error);
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


off_t bb_copyfd_eof(int fd1, int fd2)
{
    return bb_full_fd_action(fd1, fd2, 0);
}
/******************************************************************/
static void ftp_die(const char *msg, const char *remote)
{
    /* Guard against garbage from remote server */
    const char *cp = remote;
    TRACE();
    while (*cp >= ' ' && *cp < '\x7f') cp++;
    bb_error_msg_and_die("unexpected server response%s%s: %.*s",
                         msg ? " to " : "", msg ? msg : "",
                         (int)(cp - remote), remote);
    assert(0);
}


static int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
    unsigned n;
#if 0
    if (verbose_flag) {
        bb_error_msg("cmd %s %s", s1, s2);
    }
#endif
    if (s1) {
        if (s2) {
            fprintf(stream, "%s %s\r\n", s1, s2);
        } else {
            fprintf(stream, "%s\r\n", s1);
        }
    }
    do {
        char *buf_ptr;

        if (fgets(buf, 510, stream) == NULL) {
            bb_perror_msg_and_die("fgets");
        }
        buf_ptr = strstr(buf, "\r\n");
        if (buf_ptr) {
            *buf_ptr = '\0';
        }
    } while (!isdigit(buf[0]) || buf[3] != ' ');

    buf[3] = '\0';
    n = xatou(buf);
    buf[3] = ' ';
    return n;
}

void set_nport(len_and_sockaddr *lsa, unsigned port)
{
#if ENABLE_FEATURE_IPV6
    if (lsa->sa.sa_family == AF_INET6) {
        lsa->sin6.sin6_port = port;
        return;
    }
#endif
    if (lsa->sa.sa_family == AF_INET) {
        lsa->sin.sin_port = port;
        return;
    }
    /* What? UNIX socket? IPX?? :) */
}

static int xconnect_ftpdata(ftp_host_info_t *server, char *buf)
{
    char *buf_ptr;
    unsigned short port_num;

    printf("buf = %s\n", buf);
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

    printf("#### port_num = %d\n", port_num);
    set_nport(server->lsa, htons(port_num));
    return xconnect_stream(server->lsa);
}

void xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)
{

    if (connect(s, s_addr, addrlen) < 0) {
#if 0
        if (ENABLE_FEATURE_CLEAN_UP)
            close(s);
#endif
        if (s_addr->sa_family == AF_INET)
            bb_perror_msg_and_die("%s (%s)",
                                  "cannot connect to remote host",
                                  inet_ntoa(((struct sockaddr_in *)s_addr)->sin_addr));
        bb_perror_msg_and_die("cannot connect to remote host");
        assert(0);
    }
}
#if 0
void set_nport(len_and_sockaddr *lsa, unsigned port)
{
#if ENABLE_FEATURE_IPV6
    if (lsa->sa.sa_family == AF_INET6) {
        lsa->sin6.sin6_port = port;
        return;
    }
#endif
    if (lsa->sa.sa_family == AF_INET) {
        lsa->sin.sin_port = port;
        return;
    }
    /* What? UNIX socket? IPX?? :) */
}
#endif
// Die with an error message if we can't open a new socket.
int xsocket(int domain, int type, int protocol)
{
    int r = socket(domain, type, protocol);

    if (r < 0) {
        /* Hijack vaguely related config option */
#if ENABLE_VERBOSE_RESOLUTION_ERRORS
        const char *s = "INET";
        if (domain == AF_PACKET) s = "PACKET";
        if (domain == AF_NETLINK) s = "NETLINK";
        USE_FEATURE_IPV6(if (domain == AF_INET6) s = "INET6";)
            bb_perror_msg_and_die("socket(AF_%s)", s);
#else
        bb_perror_msg_and_die("socket");
#endif
    }

    return r;
}

int xconnect_stream(const len_and_sockaddr *lsa)
{
    int fd = xsocket(lsa->sa.sa_family, SOCK_STREAM, 0);
    xconnect(fd, &lsa->sa, lsa->len);
    return fd;
}

static FILE *ftp_login(ftp_host_info_t *server)
{
    FILE *control_stream;
    char buf[512];

    /* Connect to the command socket */
    control_stream = fdopen(xconnect_stream(server->lsa), "r+");
    if (control_stream == NULL) {
        /* fdopen failed - extremely unlikely */
        TRACE();
        bb_perror_nomsg_and_die();
    }

    if (ftpcmd(NULL, NULL, control_stream, buf) != 220) {
        TRACE();
        ftp_die(NULL, buf);
    }

    /*  Login to the server */
    switch (ftpcmd("USER", server->user, control_stream, buf)) {
    case 230:
        break;
    case 331:
        if (ftpcmd("PASS", server->password, control_stream, buf) != 230) {
            TRACE();
            ftp_die("PASS", buf);
        }
        break;
    default:
        TRACE();
        ftp_die("USER", buf);
    }

    ftpcmd("TYPE I", NULL, control_stream, buf);

    return control_stream;
}

static int ftp_send(ftp_host_info_t *server, FILE *control_stream,
                    const char *server_path, char *local_path)
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
    TRACE();
    fd_data = xconnect_ftpdata(server, buf);
    TRACE();
    /* get the local file */
    fd_local = STDIN_FILENO;
    if (NOT_LONE_DASH(local_path)) {
        fd_local = xopen(local_path, O_RDONLY);
        fstat(fd_local, &sbuf);

        sprintf(buf, "ALLO %"OFF_FMT"u", sbuf.st_size);
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

    TRACE();
    response = ftpcmd("STOR", server_path, control_stream, buf);
    switch (response) {
    case 125:
    case 150:
        break;
    /*      case 426:
                TRACE();
            close(fd_local);
            fprintf(stderr,"network break off");
        */
    default:
        close(fd_local);
        ftp_die("STOR", buf);
    }

    TRACE();

    /* transfer the file  */
//    ftpmissions_t *missionlist;
//    ftpbackupmission_info_t *filelist;
//  int  currmission_id;
//  int  len_missionlist;
//  int  currfilelist_id;
//  int  len_filelist;
    /*
    do{
        do{

            if (bb_copyfd_eof(fd_local, fd_data) == -1) {
            exit(EXIT_FAILURE);
          }
        }while(filelist.currfilelist_id++<=filelist.len_filelist);

    }while (missionlist.currmission_id++<=missionlist.len_missionlist);
    */


    TRACE();

    /* close it all down */
    close(fd_data);
    TRACE();
    if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
        ftp_die("close", buf);
    }
    TRACE();
    ftpcmd("QUIT", NULL, control_stream, buf);

    TRACE();
    return EXIT_SUCCESS;
}


static int ftp_rename(FILE *control_stream)
{

    char buf[512];
    int response;

    if (ftpcmd("RNFR", "newftp2.rar", control_stream, buf) != 350) {
        bb_error_msg_and_die("%s\n", buf);
    }

    response = ftpcmd("RNFR", "newftp2.rar", control_stream, buf);
    TRACE();
    printf("%d\n", response);

    if (ftpcmd("RNTO", "newftp3.rar", control_stream, buf) != 250) {
        bb_error_msg_and_die("%s\n", buf);
    }

    response = ftpcmd("RNTO", "newftp3.rar", control_stream, buf);
    TRACE();
    printf("%d\n", response);

    /*     char buf[512];
         int rnfr = ftpcmd("RNFR", "newftp2.rar", control_stream, buf);
    printf("%d\n",rnfr);
         int rnto = ftpcmd("RNTO", "newftp3.rar", control_stream, buf);
    printf("%d\n",rnto);
         return EXIT_SUCCESS;
         */
}

static int ftp_mkdir(FILE *control_stream)
{
    char buf[512];
    int fd_local;
    int response;



    //550 Requested action not taken.
    TRACE();
    response = ftpcmd("MKD", "/中国", control_stream, buf);
    printf("%s\n", buf);
    switch (response) {
    case 257:        //257 "PATHNAME" created.
        break;
    case 521:                   //521 directory already exists,taking no action
        TRACE();
        printf("%s\n", "directory already exists");
        break;
    default:
        close(fd_local);
        //ftp_die("MKD",buf);
        break;
    }
    TRACE();

    //250 Requested file action okay, completed
    response = ftpcmd("CWD", "./NEW", control_stream, buf);
    TRACE();
    printf("%s\n", buf);
    switch (response) {
    case 200:          //200 Working directory changed
        break;
    case 431:            //431 No such directory
        TRACE();
        printf("%s\n", "No such directory");
        break;
    default:
        close(fd_local);
        //ftp_die("CWD",buf);
        break;
    }

    response = ftpcmd("MKD", "/NEW/NEW", control_stream, buf);
    printf("%s\n", buf);
    switch (response) {
    case 257:        //257 "PATHNAME" created.
        break;
    case 521:                   //521 directory already exists,taking no action
        TRACE();
        printf("%s\n", "directory already exists");
        break;
    default:
        close(fd_local);
        //ftp_die("MKD",buf);
        break;
    }

    ftpcmd("QUIT", NULL, control_stream, buf);

    /*   response = ftpcmd("CWD", "/NEW/NEW", control_stream, buf);
         TRACE();
         printf("%s\n",buf);
         switch (response) {
                case 200:          //200 Working directory changed
                    break;
                case 431:            //431 No such directory
                    TRACE();
                    printf("%s\n","No such directory");
                    break;
                default:
                    close(fd_local);
                    //ftp_die("CWD",buf);
                    break;
        }
        */

    return EXIT_SUCCESS;
}


#define FTPGETPUT_OPT_CONTINUE  1
#define FTPGETPUT_OPT_VERBOSE   2
#define FTPGETPUT_OPT_USER  4
#define FTPGETPUT_OPT_PASSWORD  8
#define FTPGETPUT_OPT_PORT  16

#define FTP_IP "10.10.1.2"
#define FTP_PORT 21
int main(int argc, char **argv)
{
    char server_path[1024];
    char local_path[1024];
    const char *port = "ftp";
    FILE *control_stream;
    ftp_host_info_t *server;
// set *server_path,*local_path=0
    memset(server_path, 0, sizeof(server_path));
    memset(local_path, 0, sizeof(local_path));
    sprintf(server_path, "%s",  "newftp");
    sprintf(local_path, "%s",  "testfile");

    server = malloc(sizeof(*server));
    if (server == NULL) {
        return -1;
    }

    server->user = "pub";
    server->password = "pub";

    server->lsa = xhost2sockaddr(FTP_IP, FTP_PORT);
    printf("Connecting to %s (%s)\n", "10.10.1.2",
           xmalloc_sockaddr2dotted(&server->lsa->sa));

    control_stream = ftp_login(server);
//  ftp_rename(control_stream);
//  ftp_mkdir(control_stream);
//  return 0;
    TRACE();
    return ftp_send(server, control_stream, server_path, local_path);
}