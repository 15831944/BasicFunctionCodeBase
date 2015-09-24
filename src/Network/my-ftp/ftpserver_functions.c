#include "ftpserver_functions.h"
#define _XOPEN_SOURCE

#define ERROUTS(x)         outs("550 %s: " x, param)

static char genbuf[BUFSIZE];
static char rename_file[PATH_MAX];
static char username[100];
static char basedir[PATH_MAX];
static char transfer_type='i';
static int input_user = 0; //boolean
static int anonymous_login = 0; //boolean

static struct _port remote_port;
static int port_base = 3072;


static char *crypto(const char *key, const char *salt)
{
    printf("crypto=====\n");
    return NULL;
}


/** 输出到客户端 */
void outs(char *fmt,...)
{
    static char tmp[80];

    va_list ap;
    va_start(ap, fmt);
    sprintf(tmp, "%srn", fmt);
    vsprintf(genbuf, tmp, ap);
    send(connfd, genbuf, strlen(genbuf), 0);
    va_end(ap);
}

/** 将buf以空格做分隔符分解成两个字符串 */
void explode(char *buf, char **cmd, char **param)
{
    char *p;

    while ((p = &buf[strlen(buf) - 1]) && (*p == 'r' || *p == 'n'))
        * p = 0;
    p = strchr(buf, ' ');
    *cmd = buf;
    if (!p)
        *param = NULL;
    else {
        *p = 0;
        *param = p + 1;
    }
}

static int MakePath(char *new_path, char *path, char *filename)
{
    return sprintf(new_path, "%s%s/%s", basedir, path, filename);
}

static void Error(char *param)
{
    switch (errno) {
    case ENOTEMPTY:
        ERROUTS("Directory not empty");
        break;
    case ENOSPC:
        ERROUTS("disk full");
        break;
    case EEXIST:
        ERROUTS("File exists");
        break;
    case ENAMETOOLONG:
        ERROUTS("path is too long");
        break;
    case ENOENT:
        ERROUTS("No such file or directory");
        break;
    case ENOTDIR:
        ERROUTS("Not a directory");
        break;
    case EISDIR:
        ERROUTS("Is a directory");
    default:
        ERROUTS("Permission denied");
    }
}

static void DataConnection(int mode, int transfer_type, char *filename)
{
    int datafd;
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    char buf[BUFSIZE], *p;
    int readlen;
    FILE *tmp;
    
    
    if (data_pid != 0 && kill(data_pid, 0) == 0) {
        outs("%d Try later, data connection in use.", ACK_425);
        return;
    }

    if ((data_pid = fork()) == 0) {
        if (pasv_mode) {
            if ((datafd = accept(pasvfd, (struct sockaddr *)&sa, &len)) == -1) {
                close(pasvfd);
                exit(-1);
            }
        } else {
            memset(&sa, 0, len);

            sa.sin_family      = AF_INET;
            sa.sin_addr.s_addr = remote_port.host;
            sa.sin_port        = remote_port.port;

            if ((datafd = socket(AF_INET, SOCK_STREAM, 0)) == -1 || connect(datafd, (struct sockaddr *)&sa, len) == -1) {
                exit(-1);
            }
        }

        outs("%d Opening %s mode data connection for %s.", ACK_150, (transfer_type == 'i') ? "BINARY" : "ASCII", filename);

        if (mode) {
            /* write */
            if (transfer_type == 'i') {
                while ((readlen = fread(buf, 1, BUFSIZE, data_file)) > 0 ) {
                    send(datafd, buf, readlen, 0);
                }
            } else {
                while ((fgets(buf, BUFSIZE, data_file) != NULL)) {
                    while ((p = &buf[strlen(buf) - 1]) && (*p == 'r' || *p == 'n'))
                        * p = 0;
                    strcat(buf, "rn");
                    send(datafd, buf, strlen(buf), 0);
                }
            }
        } else {
            /* read */
            if (transfer_type == 'i') {
                while ((readlen = recv(datafd, buf, BUFSIZE, 0)) > 0 ) {
                    if (fwrite(buf, 1, readlen, data_file) == -1)
                        break;
                }
            } else {
                tmp = fdopen(datafd, "r");
                while ((fgets(buf, BUFSIZE, tmp) != NULL)) {
                    while ((p = &buf[strlen(buf) - 1]) && (*p == 'r' || *p == 'n'))
                        * p = 0;
                    strcat(buf, "n");
                    if (fwrite(buf, 1, strlen(buf), data_file) == -1)
                        break;
                }
            }
        }

        close(datafd);
        exit(0);
    }
}

void Help(char *param)
{
    if (!param) {
        outs("214 The following commands are recognized (* =>'s unimplemented).");
        outs("214   USER    PASS    ACCT*   CWD     XCWD    CDUP    XCUP    SMNT*   ");
        outs("214   QUIT    REIN*   PORT    PASV    TYPE    STRU*   MODE*   RETR    ");
        outs("214   STOR    STOU*   APPE    ALLO*   REST    RNFR    RNTO    ABOR    ");
        outs("214   DELE    MDTM    RMD     XRMD    MKD     XMKD    PWD     XPWD    ");
        outs("214   SIZE    LIST    NLST    SITE    SYST    STAT    HELP    NOOP    ");
        outs("214 Direct comments to nolove@263.net");
    } else {
        outs("214 Sorry, I haven't write the topic help.");
    }
}

void User(char *param)
{
    if (user_valid) {
        outs("%d You are already logged in!", ACK_503);
        return;
    }

    if (!param) {
        outs("%d 'USER': command requires a parameter.", ACK_500);
        return;
    }

    strncpy(username, param, 100);
    if (strcasecmp(username, "anonymous") == 0) {
        anonymous_login = 1;
        strncpy(username, "ftp", 100);
        outs("%d Anonymous login ok, send your complete e-mail address as password.", ACK_331);
    } else
        outs("%d Password required for %s", ACK_331, username);

    input_user = 1;
}

void Pass(char *param)
{
    struct passwd *pw;
    struct spwd *spw;
    char *passwd, salt[13];

    if (!input_user) {
        outs("%d Login with USER first.", ACK_503);
        return;
    }

    /* judge and chdir to its home directory */
    if (!anonymous_login) {
        if (system_uid != 0) {
            outs("%d Login incorrect.", ACK_530);
            input_user = 0;
            return;
        }

        if ((pw = getpwnam(username)) == NULL) {
            outs("%d Login incorrect.", ACK_530);
            input_user = 0;
            return;
        }

        passwd = pw->pw_passwd;
        if (passwd == NULL || strcmp(passwd, "x") == 0) {
            spw = getspnam(username);
            if (spw == NULL || (passwd = spw->sp_pwdp) == NULL) {
                outs("%d Login incorrect.", ACK_530);
                input_user = 0;
                return;
            }
        }
        strncpy(salt, passwd, 12);
        if (strcmp(passwd, crypto((const char *)param, (const char *)salt)) != 0) {
            outs("%d Login incorrect.", ACK_530);
            input_user = 0;
            return;
        }
        strcpy(path, "");
        setuid(pw->pw_uid);
        if (pw->pw_dir)
            strncpy(path, pw->pw_dir, PATH_MAX);
        else
            strcpy(path, "/");
        outs("%d User %s logged in.", ACK_230, username);
        chdir(path);
        getcwd(path, PATH_MAX);
        user_valid = 1;

    } else {
        if ((pw = getpwuid(system_uid)) == NULL) {
            outs("%d Login incorrect.", ACK_530);
            input_user = 0;
            return;
        }
        if (pw->pw_dir)
            strncpy(basedir, pw->pw_dir, PATH_MAX);
        else
            strcpy(basedir, "");

        strcpy(path, "/");
        chdir(basedir);
        getcwd(basedir, PATH_MAX);
        user_valid = 1;
        outs("%d Anonymous access granted, restrictions apply.", ACK_230);
    }
    user_valid = 1;
    outs("%d Anonymous access granted, restrictions apply.", ACK_230);
}

void Pwd(char *param)
{
    outs("%d \"%s\" is current directory.", ACK_257, path);
}

void Cwd(char *param)
{
    char tmp[PATH_MAX];

    if (param[0] == '/')
        MakePath(tmp, "", param);
    else
        MakePath(tmp, path, param);
    if (chdir(tmp) == -1) {
        Error(param);
    } else {
        if (getcwd(tmp, PATH_MAX) == NULL)
            ERROUTS("Permission denied");
        else {
            if (anonymous_login) {
                if (strncmp(basedir, tmp, strlen(basedir)) != 0) {
                    chdir(basedir);
                    strcpy(path, "/");
                } else {
                    strncpy(path, &tmp[strlen(basedir)], PATH_MAX);
                    strcat(path, "/");
                }
            } else strcpy(path, tmp);
            outs("%d CWD command successful.", ACK_250);
        }
    }
}

void Cdup(char *param)
{
    Cwd("..");
}

void Mkd(char *param)
{
    if (mkdir(param, 0755) == -1) {
        Error(param);
    } else {
        outs("%d \"%s/%s\" - Directory successfully created.", ACK_257, path, param);
    }
}

void Rmd(char *param)
{
    if (rmdir(param) == -1) {
        Error(param);
    } else {
        outs("%d RMD command successful.", ACK_250);
    }
}

void Type(char *param)
{
    if (strcasecmp(param, "i") == 0) {
        outs("%d Type set to I", ACK_200);
        transfer_type = 'i';
        return;
    }

    if (strcasecmp(param, "a") == 0) {
        outs("%d Type set to A", ACK_200);
        transfer_type = 'a';
        return;
    }

    outs("%d 'TYPE %s' not understood.", ACK_500, param);
}

void Size(char *param)
{
    struct stat s;
    if (stat(param, &s) == -1) {
        Error(param);
    } else {
        if (S_ISREG(s.st_mode))
            outs("%d %d", ACK_213, s.st_size);
        else
            outs("%d %s: not a regular file.", ACK_550, param);
    }
}

void Stat(char *param)
{
    FILE *f;
    static char tmp[PATH_MAX], tmp2[PATH_MAX];
    static char statbuf[BUFSIZE];

    outs("%d-status of \"%s\":", ACK_211, param);
    if (strchr(param, ' ') != NULL) {
        sprintf(tmp2, "\"%s\"", param);
    } else {
        sprintf(tmp2, "%s", param);
    }
    sprintf(tmp, "/bin/ls -l --color=never %s", tmp2);
    if ((f = popen(tmp, "r")) != NULL) {
        while (fgets(statbuf, BUFSIZE, f) != NULL) {
            if (statbuf[0] == 't')
                continue;
            send(connfd, statbuf, strlen(statbuf), 0);
        }
        pclose(f);
    }
    outs("%d End of Status", ACK_211);
}

void Dele(char *param)
{
    if (unlink(param) == -1) {
        Error(param);
    } else {
FIXME:
        outs("%d DELE command successful.", ACK_250, param);
    }
}



void Quit(char *param)
{
    outs("%d Goodbye", ACK_221);
    fclose(file);
    exit(0);
}

void Noop(char *param)
{
    outs("%d NOOP command successful", ACK_200);
}

static void DoList(char *param, char *cmd)
{
    char tmp[PATH_MAX], tmp2[PATH_MAX];
    char *mode = "r";

    MakePath(tmp, path, param);
    if (param) {
        if (strchr(param, ' ') != NULL) {
            sprintf(tmp2, "\"%s\"", param);
        } else {
            sprintf(tmp2, "%s", param);
        }
    } else {
        strcpy(tmp2, "");
    }
    sprintf(tmp, "/bin/ls %s --color=never %s", cmd, tmp2);
    if ((data_file = popen(tmp, mode)) == NULL) {
        Error("LIST");
        return;
    }

    DataConnection(1, 'a', "/bin/ls");
}

void List(char *param)
{
    DoList(param, "-l");
}

void Nlst(char *param)
{
    DoList(param, "-1");
}

static void PreConnection(char *param, char open_mode, int data_mode)
{
    char tmp[PATH_MAX];
    char mode[3];
    struct stat s;

    MakePath(tmp, path, param);
    mode[0] = open_mode;
    if (transfer_type == 'i')
        mode[1] = 'b';
    else
        mode[1] = 't';
    mode[2] = 0;

    if (data_mode && stat(tmp, &s) == -1) {
        Error(param);
        return;
    }
    if ((data_file = fopen(tmp, mode)) == NULL) {
        Error(param);
        return;
    }

    if (open_mode != 'a')
        fseek(data_file, file_rest, SEEK_SET);
    if (data_mode) {
        sprintf(tmp, "%s (%d bytes)", param, (int)(s.st_size - file_rest));
    } else {
        strcpy(tmp, param);
    }

    DataConnection(data_mode, transfer_type, tmp);
}

void Stru(char *param)
{
    printf("Stru\n");
    return;
}

void Mode(char *param)
{
    printf("Mode\n");
    return;
}

void Retr(char *param)
{
    PreConnection(param, 'r', 1);
}

void Stor(char *param)
{
    PreConnection(param, 'w', 0);
}

void Appe(char *param)
{
    PreConnection(param, 'a', 0);
}

void Abor(char *param)
{
    if (data_pid != 0) {
        kill(data_pid, SIGTERM);
    }

    outs("%d ABOR command successful.", ACK_226);
}

void Mdtm(char *param)
{
    struct stat s;
    struct tm *tms;

    if (stat(param, &s) == -1) {
        Error(param);
    } else {
        tms = localtime((const time_t *)&s.st_mtime);
        outs("%d %4d%2d%2d%2d%2d%2d", ACK_213, tms->tm_year, tms->tm_mon, tms->tm_mday, tms->tm_hour, tms->tm_min, tms->tm_sec);
    }
}

void Syst(char *param)
{
    outs("%d UNIX Type: L8", ACK_215);
}

void Rest(char *param)
{
    int r;

    r = atoi(param);

    if (r < 0 || (r == 0 && param[0] != '0')) {
        outs("%d REST requires a value greater than or equal to 0.", ACK_501);
        return;
    }

    file_rest = r;
    outs("%d Restarting at %u. Send STORE or RETRIEVE to initiate transfer.", ACK_350, file_rest);
}

void Rnfr(char *param)
{
    char tmp[PATH_MAX];
    struct stat s;

    MakePath(tmp, path, param);
    if (stat(tmp, &s) == -1) {
        Error(tmp);
    } else {
        outs("%d File or directory exists, ready for destination name.", ACK_350);
        strncpy(rename_file, param, PATH_MAX);
    }
}

void Rnto(char *param)
{
    char tmp[PATH_MAX];

    if (strcmp(rename_file, "") == 0) {
        outs("%d Bad sequence of commands.", ACK_503);
        return;
    }
    MakePath(tmp, path, param);
    if (rename(rename_file, tmp) == -1) {
        Error(tmp);
    } else {
        outs("%d RNTO command successful.", ACK_350);
        strcpy(rename_file, "");
    }
}



void Site(char *param)
{
    int i, mode;
    static char buf[BUFSIZE] = "250 ";
    FILE *f;
    char *p1, *p2, *p3, *p4, tmp[PATH_MAX], tmp2[PATH_MAX];

    explode(param, &p1, &p2);
    if (p1 == NULL || p2 == NULL || strncasecmp(p1, "CHMOD", 5) != 0) {
        outs("%d SITE option not supported.", ACK_501);
        return;
    }

    explode(p2, &p3, &p4);
    if (p3 == NULL || p4 == NULL) {
        outs("%d SITE option not supported.", ACK_501);
        return;
    }
    mode = strtol(p3, (char **)NULL, 8);
    if (chmod(p4, mode) == -1) {
        outs("%d SITE option not supported.", ACK_501);
    } else {
        outs("%d SITE command successful.", ACK_250);
    }
}

void Port(char *param)
{
    int i;
    struct _port tmp;
    unsigned char r, *ptr = (unsigned char *)&tmp;
    char *p, *ori = param;

    for (i = 0; i < 6; i++) {
        r = atoi(ori);
        if (r == 0 && *ori != '0') {
            outs("%d Syntax error in parameters or arguments.", ACK_501);
            return;
        }
        ptr[i] = r;
        p = strchr(ori, ',');
        if (p == NULL) {
            if (i != 5) {
                outs("%d Syntax error in parameters or arguments.", ACK_501);
                return;
            }
        } else {
            *p = 0;
            ori = p + 1;
        }
    }

    memcpy(&remote_port, &tmp, sizeof(remote_port));
    pasv_mode = 0;
    outs("%d PORT Command successful.", ACK_200);
}

void Pasv(char *param)
{
    int port = port_base;
    struct sockaddr_in sa;
    char tmp[30], t2[10];
    int len, i;

    len = 1;
    close(pasvfd);
    if ((pasvfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
        setsockopt(pasvfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&len, sizeof(int)) == -1) {
        outs("%d PASV command failed.", ACK_550);
        return;
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    do {
        port++;
        if (port == 65535) {
            outs("%d Can't bind any port.", ACK_550);
            return;
        }
        sa.sin_port = htons(port);
    } while (bind(pasvfd, (struct sockaddr *)&sa, sizeof(sa)) == -1 || listen(pasvfd, 1) == -1);
    local_port.port = htons(port);
    memset(tmp, 0, 18);
    for (i = 0; i < 6; i++) {
        sprintf(t2, "%u", ((unsigned char *)&local_port)[i]);
        strcat(tmp, t2);
        if (i != 5)
            strcat(tmp, ",");
    }

    pasv_mode = 1;
    outs("%d Entering Passive Mode (%s)", ACK_227, tmp);
}














