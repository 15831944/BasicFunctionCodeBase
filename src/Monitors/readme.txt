
monitor server端启动过程：



init ——> 初始化，循环等待创建链接，进行身份校验；

loop ——> 创建消息接收循环线程，等待接收消息；

    parse ——> 解析消息和命令，

    execute ——> 执行命令；

返回





命令注册：


#define MONITOR_MSGLEN 2560
#define MONITOR_CMDLEN 64
#define MONITOR_PAEALEN 64

struct moni_buf {
    int len;
    char buf[MONITOR_MSGLEN + 1];
    char *extend;
    char remoteip[20];
};

typedef struct moni_buf* moni_buf_t;
typedef int (*cmd_fn)(moni_buf_t, int);

struct monitorCommand {
    char comand[MONITOR_CMDLEN];
    cmd_fn fn;
};

struct monitor_para {
    char para[MONITOR_PAEALEN];
    cmd_fn read;
    cmd_fn write;
};


struct monitor_ioctl {
    char para[MONITOR_PAEALEN];
    cmd_fn cmd_ioctl;
};

struct monitor_connect {
    char para[MONITOR_PAEALEN];
    cmd_fn cmd_connect;
};


struct monitor_inform {
    char para[MONITOR_PAEALEN];
    cmd_fn cmd_inform;
};
struct monitor_initialize{
	char para[MONITOR_PAEALEN];
	cmd_fn cmd_inform;
};


1-服务器端消息解析：
    struct monitorCommand moni_cmd[] = {
        {"read", monitorCommandRead},
        {"write", monitorCommandWrite},
        {"ioctl", monitorCommandIoctl},
        {"inform", monitorCommandInform},
        {"connect", monitorCommandConnect},
        {"initialize", monitorCommandInit},
        {"", NULL }
    };

    
2-服务器端解析命令：
    struct monitor_para moni_para[] = {
        {"Main_HomepageUrl", stb_homepage_read, stb_homepage_write}, // Main_HomepageUrl 主认证地址 支持读取、写入。
        {"Secondary_HomepageUrl", stb_homepage1_read, stb_homepage1_write}, // Secondary_HomepageUrl 备认证地址 参数导出时如果不存在，则为空
        
        {"netuseraccount", stb_netuseraccount_read, stb_netuseraccount_write}, // netuseraccount pppoe接入帐号 接入账号, 支持读取、写入。
        {"netuserpassword", stb_netuserpassword_read, stb_netuserpassword_write}, // netuserpassword pppoe接入密码 支持写入。
        {"ntvuseraccount", stb_ntvuseraccount_read, stb_ntvuseraccount_write}, // ntvuseraccount iptv业务帐号 支持读取、写入。
        {"ntvuserpassword", stb_ntvuserpassword_read, stb_ntvuserpassword_write}, // ntvuserpassword iptv业务密码 支持写入。
        {"", NULL }
    };

    
    
    
    
    
    
    
    
命令格式：
inform^dataUpload^/debugInfo^2000000

E8785A408765AE328765AE32ioctl^getAllDebugInfo^null\0

5)	正常情况下，STB返回应如下
200ioctl^getAllDebugInfo^null\0
机顶盒收到200后，表示STB已收到命令，


TCP建立后，请求文件的报文命令使用“inform”，如下样例表示将要发送长度为2000000byte的数据： 
inform^dataUpload^/debugInfo^2000000





    
    
    
    







