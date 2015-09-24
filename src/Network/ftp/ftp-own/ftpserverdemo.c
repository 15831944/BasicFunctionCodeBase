
#define _XOPEN_SOURCE
#include <unistd.h>

#include "FTPd.h"

char *crypt(const char *key, const char *salt)
{
    printf("crypt=====\n");
    return NULL;
}

void do_child();

/* 输出到客户端 */
void outs(char *fmt, ...)
{
    static char tmp[80];
    
    va_list ap;
    va_start(ap, fmt);
    sprintf(tmp, "%srn", fmt);
    vsprintf(genbuf, tmp, ap);
    send(connfd, genbuf, strlen(genbuf), 0);
    va_end(ap);
};

/* 将buf以空格做分隔符分解成两个字符串 */
void explode(char *buf, char **cmd, char **param)
{
    char *p;
    
    while ((p = &buf[strlen(buf)-1]) && (*p == 'r' || *p == 'n')) 
		*p = 0;
    p = strchr(buf, ' ');
    *cmd = buf;
    if (!p) 
		*param = NULL;
    else {
        *p = 0;
        *param = p+1;
    };
};

int MakePath(char *new_path, char* path, char* filename)
{
    return sprintf(new_path, "%s%s/%s", basedir, path, filename);
};

void Error(char *param) 
{
    switch (errno) {
        case ENOTEMPTY:
            ERRS("Directory not empty");
            break;
        case ENOSPC:
            ERRS("disk full");
            break;          
        case EEXIST:
            ERRS("File exists");
            break;
        case ENAMETOOLONG:
            ERRS("path is too long");
            break;
        case ENOENT:
            ERRS("No such file or directory");
            break;
        case ENOTDIR:
            ERRS("Not a directory");
            break;
        case EISDIR:
            ERRS("Is a directory");
        default:
            ERRS("Permission denied");
    };
}

int Check(char *cmd, char *param, int check)
{
    if (check & CHECK_LOGIN && !user_valid) {
        outs("530 Please login with USER and PASS");
        return 0;
    };
    
    if (check & CHECK_NOLOGIN && user_valid) {
        outs("503 You are already logged in!");
        return 0;
    };
    
    if (check & NEED_PARAM) {
        if (!param) 
			outs("501 Invalid number of arguments, check more arguments.");
        return (param != NULL);
    };
    
    if (check & NO_PARAM) {
        if (param) 
			outs("501 Invalid number of arguments.");
        return (param == NULL);
    };
    return 1;
};

void DataConnection(int mode, int transfer_type, char *filename)
{
    int datafd;
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    char buf[BUFSIZE], *p;
    int readlen;
    FILE *tmp;
    if (data_pid != 0 && kill(data_pid, 0) == 0) {
        outs("425 Try later, data connection in use.");
        return;
    };
            
    if ((data_pid = fork()) == 0) {
        if (pasv_mode) {
            if ((datafd = accept(pasvfd, (struct sockaddr *)&sa, &len)) == -1) {
                close(pasvfd);
                exit(-1);
            };
            
        } else {
            memset(&sa, 0, len);
        
            sa.sin_family      = AF_INET;
            sa.sin_addr.s_addr = remote_port.host;
            sa.sin_port        = remote_port.port;
            
            if ((datafd = socket(AF_INET, SOCK_STREAM, 0)) == -1 || connect(datafd, (struct sockaddr *)&sa, len) == -1) {
                exit(-1);
            };
        };
        
        outs("150 Opening %s mode data connection for %s.", (transfer_type == 'i')?"BINARY":"ASCII", filename);
        
        if (mode) {
            /* write */
            if (transfer_type == 'i') {
                while ((readlen = fread(buf, 1, BUFSIZE, data_file)) >0 ) {
                    send(datafd, buf, readlen, 0);
                };
            } else {
                while ((fgets(buf, BUFSIZE, data_file) != NULL)) {
                    while ((p = &buf[strlen(buf)-1]) && (*p == 'r' || *p == 'n')) 
						*p = 0;
                    strcat(buf, "rn");
                    send(datafd, buf, strlen(buf), 0);
                };
            };
        } else {
            /* read */
            if (transfer_type == 'i') {
                while ((readlen = recv(datafd, buf, BUFSIZE, 0)) >0 ) {
                    if (fwrite(buf, 1, readlen, data_file) == -1) 
						break;
                };
            } else {
                tmp = fdopen(datafd, "r");
                while ((fgets(buf, BUFSIZE, tmp) != NULL)) {
                    while ((p = &buf[strlen(buf)-1]) && (*p == 'r' || *p == 'n'))
						*p = 0;
                    strcat(buf, "n");
                    if (fwrite(buf, 1, strlen(buf), data_file) == -1) 
						break;
                };
            };
        };          
            
        close(datafd);
        exit(0);
    };
};

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
    };
};

void User(char *param)
{
    if (user_valid) {
        outs("503 You are already logged in!");
        return;
    };
    
    if (!param) {
        outs("500 'USER': command requires a parameter.");
        return;
    };
    
    strncpy(username, param, 100);
    if (strcasecmp(username, "anonymous") == 0) {
        anonymous_login = 1;
        strncpy(username, "ftp", 100);
        outs("331 Anonymous login ok, send your complete e-mail address as password.");
    } else 
		outs("331 Password required for %s", username);
    
    input_user = 1;
};

void Pass(char *param)
{
    struct passwd *pw;
    struct spwd *spw;
    char *passwd, salt[13];
    
    if (!input_user) {
        outs("503 Login with USER first.");
        return;
    };
    
    /* judge and chdir to its home directory */
    if (!anonymous_login) {
        if (system_uid != 0) {
            outs("530 Login incorrect.");
            input_user = 0;
            return;
        };
                
        if ((pw = getpwnam(username)) == NULL) {
            outs("530 Login incorrect.");
            input_user = 0;
            return;
        };
    
        passwd = pw->pw_passwd;
        if (passwd == NULL || strcmp(passwd, "x") == 0) {
            spw = getspnam(username);
            if (spw == NULL || (passwd = spw->sp_pwdp) == NULL) {
                outs("530 Login incorrect.");
                input_user = 0;
                return;
            };
        };
        strncpy(salt, passwd, 12);
        if (strcmp(passwd, crypt((const char*)param, (const char*)salt)) != 0) {
            outs("530 Login incorrect.");
            input_user = 0;
            return;
        };
        strcpy(path, "");
        setuid(pw->pw_uid);
        if (pw->pw_dir) 
			strncpy(path, pw->pw_dir, PATH_MAX);
        else 
			strcpy(path, "/");
        outs("230 User %s logged in.", username);
        chdir(path);
        getcwd(path, PATH_MAX);
        user_valid = 1;
    
    } else {
        if ((pw = getpwuid(system_uid)) == NULL) {
            outs("530 Login incorrect.");
            input_user = 0;
            return;
        };
        if (pw->pw_dir) 
			strncpy(basedir, pw->pw_dir, PATH_MAX); 
        else 
			strcpy(basedir, "");
        
        strcpy(path, "/");
        chdir(basedir);
        getcwd(basedir, PATH_MAX);
        user_valid = 1;
        outs("230 Anonymous access granted, restrictions apply.");
    };
    user_valid = 1;
    outs("230 Anonymous access granted, restrictions apply.");
};

void Pwd(char *param)
{
    outs("257 \"%s\" is current directory.", path);
};

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
			ERRS("Permission denied");
        else {
            if (anonymous_login) {
                if (strncmp(basedir, tmp, strlen(basedir)) != 0) {
                    chdir(basedir);
                    strcpy(path, "/");
                } else {
                    strncpy(path, &tmp[strlen(basedir)], PATH_MAX);
                    strcat(path, "/");
                };
            } else strcpy(path, tmp);
            outs("250 CWD command successful.");
        };
    };
};

void Cdup(char *param)
{
    Cwd("..");
};

void Mkd(char *param)
{
    if (mkdir(param, 0755) == -1) {
        Error(param);
    } else {
        outs("257 \"%s/%s\" - Directory successfully created.", path, param);
    };
};

void Rmd(char *param)
{
    if (rmdir(param) == -1) {
        Error(param);
    } else {
        outs("250 RMD command successful.");
    };
};

void Type(char *param)
{
    if (strcasecmp(param, "i") == 0) {
        outs("200 Type set to I");
        transfer_type = 'i';
        return;
    };
    
    if (strcasecmp(param, "a") == 0) {
        outs("200 Type set to A");
        transfer_type = 'a';
        return;
    };
    
    outs("500 'TYPE %s' not understood.", param);
};

void Size(char *param)
{
    struct stat s;
    if (stat(param, &s) == -1) {
        Error(param);
    } else {
        if (S_ISREG(s.st_mode)) 
			outs("213 %d", s.st_size);
        else 
			outs("550 %s: not a regular file.", param);
    };
};

void Stat(char *param)
{
    FILE *f;
    static char tmp[PATH_MAX], tmp2[PATH_MAX];
    static char statbuf[BUFSIZE];
    
    outs("211-status of \"%s\":", param);
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
        };
        pclose(f);
    };
    outs("211 End of Status");
};

void Dele(char *param)
{
    if (unlink(param) == -1) {
        Error(param);
    } else {
        FIXME: outs("250 DELE command successful.", param);
    };
};


    
void Quit(char *param)
{
    outs("221 Goodbye");
    fclose(file);
    exit(0);
};

void Noop(char *param)
{
    outs("200 NOOP command successful");
};

void DoList(char *param, char *cmd)
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
    };
    
    DataConnection(1, 'a', "/bin/ls");
};

void List(char *param)
{
    DoList(param, "-l");
};

void Nlst(char *param)
{
    DoList(param, "-1");
};

void PreConnection(char *param, char open_mode, int data_mode)
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
    };
    if ((data_file = fopen(tmp, mode)) == NULL) {
        Error(param);
        return;
    };
    
    if (open_mode != 'a') 
		fseek(data_file, file_rest, SEEK_SET);
    if (data_mode) {
        sprintf(tmp, "%s (%d bytes)", param, (int)(s.st_size - file_rest));
    } else {
        strcpy(tmp, param);
    };
    
    DataConnection(data_mode, transfer_type, tmp);
};

void Retr(char *param)
{
    PreConnection(param, 'r', 1);
};

void Stor(char *param)
{
    PreConnection(param, 'w', 0);
};

void Appe(char *param)
{
    PreConnection(param, 'a', 0);
};

void Abor(char *param)
{
    if (data_pid != 0) {
        kill(data_pid, SIGTERM);
    };
    
    outs("226 ABOR command successful.");
};

void Mdtm(char *param)
{
    struct stat s;
    struct tm *tms;
    
    if (stat(param, &s) == -1) {
        Error(param);
    } else {
        tms = localtime((const time_t *)&s.st_mtime);
        outs("213 %4d%2d%2d%2d%2d%2d", tms->tm_year, tms->tm_mon, tms->tm_mday, tms->tm_hour, tms->tm_min, tms->tm_sec);
    };
};

void Syst(char *param)
{
    outs("215 UNIX Type: L8");
};

void Rest(char *param)
{
    int r;
    
    r = atoi(param);
    
    if (r < 0 || (r == 0 && param[0] != '0')) {
        outs("501 REST requires a value greater than or equal to 0.");
        return;
    };
    
    file_rest = r;
    outs("350 Restarting at %u. Send STORE or RETRIEVE to initiate transfer.", file_rest);
};

void Rnfr(char *param)
{
    char tmp[PATH_MAX];
    struct stat s;
    
    MakePath(tmp, path, param);
    if (stat(tmp, &s) == -1) {
        Error(tmp);
    } else {
        outs("350 File or directory exists, ready for destination name.");
        strncpy(rename_file, param, PATH_MAX);
    };
};

void Rnto(char *param)
{
    char tmp[PATH_MAX];
    
    if (strcmp(rename_file, "") == 0) {
        outs("503 Bad sequence of commands.");
        return;
    };
    MakePath(tmp, path, param);
    if (rename(rename_file, tmp) == -1) {
        Error(tmp);
    } else {
        outs("250 RNTO command successful.");
        strcpy(rename_file, "");
    };
};


    
void Site(char *param)
{
    int i, mode;
    static char buf[BUFSIZE] = "250 ";
    FILE *f;
    char *p1, *p2, *p3, *p4, tmp[PATH_MAX], tmp2[PATH_MAX];
        
    explode(param, &p1, &p2);
    if (p1 == NULL || p2 == NULL || strncasecmp(p1, "CHMOD", 5) != 0) {
        outs("501 SITE option not supported.");
        return;
    };
    
    explode(p2, &p3, &p4);
    if (p3 == NULL || p4 == NULL) {
        outs("501 SITE option not supported.");
        return;
    };
    mode = strtol(p3, (char **)NULL, 8);
    if (chmod(p4, mode) == -1) {
        outs("501 SITE option not supported.");
    } else {
        outs("250 SITE command successful.");
    };
};

void Port(char *param)
{
    int i;
    struct _port tmp;
    unsigned char r, *ptr = (unsigned char *)&tmp;
    char *p, *ori = param;
    
    for (i=0;i<6;i++) {
        r = atoi(ori);
        if (r == 0 && *ori != '0') {
            outs("501 Syntax error in parameters or arguments.");
            return;
        };
        ptr[i] = r;
        p = strchr(ori, ',');
        if (p == NULL) {
            if (i != 5) {
                outs("501 Syntax error in parameters or arguments.");
                return;
            };
        } else {
            *p = 0;
            ori = p+1;
        };
    };
        
    memcpy(&remote_port, &tmp, sizeof(remote_port));
    pasv_mode = 0;
    outs("200 PORT Command successful.");
};

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
        outs("550 PASV command failed.");
        return;
    };
    memset(&sa, 0, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    do {
        port++;
        if (port == 65535) {
            outs("550 Can't bind any port.");
            return;
        };
        sa.sin_port = htons(port);
    } while (bind(pasvfd, (struct sockaddr *)&sa, sizeof(sa)) == -1 || listen(pasvfd, 1) == -1);
    local_port.port = htons(port);
    memset(tmp, 0, 18);
    for (i=0;i<6;i++) {
        sprintf(t2, "%u", ((unsigned char *)&local_port)[i]);
        strcat(tmp, t2);
        if (i!=5) 
			strcat(tmp, ",");
    };
    
    pasv_mode = 1;
    outs("227 Entering Passive Mode (%s)", tmp);
};

struct _cmd_list commandList[] = {
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
    {"STRU"},
    {"MODE"},
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
    {""}
};  

void do_alarm()
{
    outs("421 No Transfer Timeout (%d seconds): closing control connection", timeout);
    fclose(file);
    close(connfd);
    exit(0);
};

void do_child()
{
    int pid, ret;
    
    while ((pid = waitpid(-1, &ret, WNOHANG|WUNTRACED)) > 0) {
        if (pid == data_pid) {
            if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
				outs("226 Transfer complete.");
            } else if (WIFSIGNALED(ret) && WTERMSIG(ret) == SIGTERM) {
				outs("425 Transfer aborted.");
            } else {
				outs("425 Transfer error.");
            }
            fclose(data_file);
            if (pasv_mode) 
				close(pasvfd);
            pasv_mode = 0;
            transfer = 0;
            data_pid = 0;
            file_rest = 0;
        };
    };
};
    
	
	
void do_term()
{
    outs("425 Transfer aborted.");
    exit(0);
};

int init()
{
    int n, i;
    struct sockaddr_in sa;
    struct hostent *host;
    struct in_addr ia;
    
    /* register the signal handler */
    signal(SIGCHLD, do_child);
    signal(SIGTERM, do_term);
    
    /* init network */  
    memset(&sa, 0, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port        = htons(ftp_port);
    
    if (gethostname(hostname, BUFSIZE) == -1) 
		strcpy(hostname, "");
        
    i = 1;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 || 
			setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&i, sizeof(int)) == -1) {
        perror("Pacific FTP:");
        return -1;
    }
    
    if (bind(listenfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        fprintf(stderr, "Pacific FTP: Can not bind port %dn", ftp_port);
        return -1;
    }
    if (listen(listenfd, max_conn) == -1) {
        perror("Pacific FTP:");
        return -1;
    }
    /* set the program a daemon */
    if(fork()) 
		exit(0);
    for (n = 0; n<3; n++) 
		close(n);
    open("/dev/null", O_RDONLY);
    dup2(0,1);
    dup2(0,2);
    if((n=open("/dev/tty",O_RDWR)) > 0) {
        ioctl(n, TIOCNOTTY, 0) ;
        close(n);
    }
    setsid();
    
    if(fork()) 
		exit(0);
    /* get the uid */
    system_uid = getuid();
}




//USE:: ftps -p:21 -m:4 -t:12
int main(int argc, char **argv)
{
    struct sockaddr_in sa;
    int len, cur;
    int okay=1, i;
    char *cmd, *param;
	
	
    /* get the param */
    for (i=1; i<argc; i++) {
        if (argv[i][0] != '-' || argv[i][2] != ':')
			okay = 0;
        switch (argv[i][1]) {
            case 'p':
                ftp_port = atoi(&argv[i][3]);
                if (ftp_port == 0) 
					okay = 0;
                break;
            case 'm':
                max_conn = atoi(&argv[i][3]);
                if (max_conn == 0) 
					okay = 0;
                break;
            case 't':
                timeout = atoi(&argv[i][3]);
                if (timeout == 0) 
					okay = 0;
                break;
            default:
                okay = 0;
        };
        if (!okay) 
			break;
    };      
        
		
    if (!okay) {    
        printf("Usage: %s [-p:port] [-m:maxconn] [-t:timeout]ntport: default is 21.ntmaxconn: the max client num connected.n", argv[0]);
        return -1;
    };
    if (init() == -1) 
		return -1;
    
	
	
    /* main loop */
    while (1) 
    {
        len = sizeof(sa);
        connfd = accept(listenfd, (struct sockaddr *)&sa, &len);
        
        if (connfd == -1)
			continue;
        
        if (fork() == 0) 
        {
            len = sizeof(sa);
            getsockname(connfd, (struct sockaddr *)&sa, &len);
            local_port.host = sa.sin_addr.s_addr;
            file = fdopen(connfd, "rt+");
            setbuf(file, (char *)0);
            getcwd(path, PATH_MAX);
            outs("220 PacificFTP ready, hostname: %s", hostname);
            signal(SIGALRM, do_alarm);
            alarm(timeout);
            /* child process's main loop */
            while (fgets(inbuf, BUFSIZE, file) != NULL) {
                explode(inbuf, &cmd, &param);
                if (strcmp(cmd, "") == 0) 
					continue;
                /* Make cmd UPPERCASE */
                for (i=0;i<strlen(cmd);i++) 
					cmd[i]=toupper(cmd[i]);
                cur = 0;
                okay = 0;
                while (strcmp(commandList[cur].cmd, "") != 0) {
                    if (strcmp(commandList[cur].cmd, cmd) == 0) {
                        if (commandList[cur].func == NULL) {
							outs("502 %s command unimplemented.", cmd);
                        } else {
                            if ((commandList[cur].check & NO_CHECK) || Check(cmd, param, commandList[cur].check))
                                (*commandList[cur].func)(param);
                        }
                        okay = 1;
                        break;
                    };
                    cur++;
                };
                if (!okay) 
					outs("500 %s not understood", cmd);
            };                                  

            fclose(file);
            exit(0);
        };
        close(connfd);
    };
}

