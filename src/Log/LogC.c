#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "LogC.h"


/**
    LogLevel:  "Assert", "Fatal!", "Error!", "Warning", "Info", "Verbose", "Debug", "Undefined"
                  无        1         1         3          4        2         2         无
    转义序列相关的常用参数如下(通过man console_codes命令可查看更多的参数描述)：
        显示：0(默认)、1(粗体/高亮)、22(非粗体)、4(单条下划线)、24(无下划线)、5(闪烁)、25(无闪烁)、7(反显、翻转前景色和背景色)、27(无反显)
        颜色：0(黑)、1(红)、2(绿)、 3(黄)、4(蓝)、5(洋红)、6(青)、7(白)
        前景色为30+颜色值，如31表示前景色为红色；背景色为40+颜色值，如41表示背景色为红色。
**/
static const uint8_t color[] = {0, 0x41, 0x41, 0x43, 0x04, 0x02, 0x02, 0x02};
#define setLogColor(x)  \
    {               \
        if(x < 4)   \
            fprintf(stderr, "\033[1;%d;3%dm", color[x]>>4, color[x]&15);  \
        else        \
            fprintf(stderr, "\033[%d;3%dm", color[x]>>4, color[x]&15);    \
    }
#define resetLogColor()   fprintf(stderr, "\033[0m")

/** 日志level名称 **/
static const char *textLogLevel[] = {"Assert", "Fatal!", "Error!", "Warning", "Info", "Verbose", "Debug", "Undefined"};

/** 日期字符串：月份 & 星期 **/
static const char *monthStr[] = {"Reserved", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Undefined"};
static const char *weekStr[]  = {"Sun.", "Mon.", "Tues.", "Wed.", "Thur.", "Fri.", "Sat.", "Undefined"};

/**
* 用于保存模块的列表： 列表大小MODULE_LIST_SIZE指定；
*       g_moduleCount 用于记录下次添加模块是的起始位置
**/
static struct moduleInfo g_moduleList[MODULE_LIST_SIZE] = {};
static int g_moduleCount = 0;

/** g_version 使用——Version结构记录全局的版本号 **/
struct _Version g_version;


/**
* 获取当前时间：年-月-日-星期几   时-分-秒-毫秒
* 输出格式：
*       [month-day-year, weekday][hour:min:sec.msec]
* flag: 0: no output
        1: output date
        2: output time
        3: output date&time
**/
static int get_CurrentTime(char *buf, int flag)
{
    if (flag == 0)
        return 0;

    static DateTime sDTime;
    struct timeval current;
    struct tm tempTime;

    if (!gettimeofday(&current, NULL)) {
        localtime_r(&current.tv_sec, &tempTime);
        sDTime.mYear       = tempTime.tm_year + 1900;
        sDTime.mMonth      = tempTime.tm_mon + 1;
        sDTime.mDayOfWeek  = tempTime.tm_wday;
        sDTime.mDay        = tempTime.tm_mday;
        sDTime.mHour       = tempTime.tm_hour;
        sDTime.mMinute     = tempTime.tm_min;
        sDTime.mSecond     = tempTime.tm_sec;
        sDTime.muSecond    = current.tv_usec;
    }

    if (flag & 0x01)
        sprintf(buf, "[%s-%02d-%04d, %s]", monthStr[sDTime.mMonth], sDTime.mDay, sDTime.mYear, weekStr[sDTime.mDayOfWeek]);
    if (flag & 0x02)
        sprintf(buf + strlen(buf), "[%02d:%02d:%02d.%03d] ", sDTime.mHour, sDTime.mMinute, sDTime.mSecond, sDTime.muSecond / 1000);

    //printf("get_CurrentTime: buf[%s]...\n", buf);
    return strlen(buf);
}

/**
* 获取设备信息： mac，hostname，ip，串号等
* flag: 0: no output
        1: output mac
        2: output ip
        3: output mac&ip
**/
static int get_MachineInfo(const char *device, char *buf, int flag)
{
    if (flag == 0)
        return 0;

    unsigned char macAddr[6] = {0};  //6是MAC地址长度
    unsigned char ipAddr[18] = {0};
    unsigned char hostName[32] = {0};
    int sockfd;
    struct ifreq ifr4dev;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); //internet协议族的数据报类型套接口
    strncpy(ifr4dev.ifr_name, device, sizeof(ifr4dev.ifr_name) - 1);   //将设备名作为输入参数传入

    //获取MAC地址
    ifr4dev.ifr_hwaddr.sa_family = ARPHRD_ETHER;  //此处需要添加协议，网络所传程序没添加此项获取不了mac。
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr4dev) == -1) {
        printf("get_MachineInfo: get device [%s] mac error.\n", device);
        return -1;
    }
    memcpy(macAddr, ifr4dev.ifr_hwaddr.sa_data, ETH_ALEN);

    //获取ip地址
    if (ioctl(sockfd, SIOCGIFADDR, &ifr4dev) == -1) {
        printf("get_MachineInfo: get device [%s] ip error.\n", device);
        return -1;
    }
    /**
        struct sockaddr是通用的套接字地址，而struct sockaddr_in则是internet环境下套接字的地址形式，
        二者长度一样，都是16个字节。二者是并列结构，指向sockaddr_in结构的指针也可以指向sockaddr。
        一般情况下，需要把sockaddr_in结构强制转换成sockaddr结构再传入系统调用函数中。
    **/
    struct sockaddr_in *sin =  (struct sockaddr_in *)&ifr4dev.ifr_addr;
    strcpy((char *)ipAddr, inet_ntoa(sin->sin_addr));
    close(sockfd);

    //get hostName
    gethostname((char *)hostName, sizeof(hostName));
    sprintf(buf, "[%s]", hostName);

    if (flag & 0x01)
        sprintf(buf + strlen(buf), "[%02x:%02x:%02x:%02x:%02x:%02x] ", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    if (flag & 0x02)
        sprintf(buf + strlen(buf) - 1, "[%s] ", ipAddr);

    //printf("get_MachineInfo: mac&ip buf=[%s]\n", buf);
    return (int)strlen(buf);
}

/**
* 获取版本号： Major.Minor.BuildVersion:currentModuleVersion
               主版本号 . 子版本号 [. 修正版本号 或 编译版本号 ] : 当前模块版本号
* flag: 0: no version output
        1: output Major_Version_Number
        2: output Minor_Version_Number
        4: output Revision_Number[Build_Number]
        8: output currentModule_Version_Number

        3: output Major_Version_Number & Minor_Version_Number
        7: output Major_Version_Number & Minor_Version_Number & Revision_Number[Build_Number]
        15: output all
*
* 注： 需要预先将版本号填充到 _Version 结构体
**/
static int get_Version(char *buf, int module, int flag)
{
    if ((flag == 0) || (flag == 5) || (flag == 6) || ((8 < flag) && (flag < 15)) || (flag > 15))
        return 0;
    char tmp_currentModuleVersion[12] = {};

    if (g_version.BuildVersion == 0) {
        g_version.Major = 0;
        g_version.Minor = 0;
        g_version.BuildVersion = 0;
    }

    sprintf(buf, "%s", "[");
    if (flag & 0x01)
        sprintf(buf + strlen(buf), "%d", g_version.Major);
    if (flag & 0x02)
        sprintf(buf + strlen(buf), ".%d", g_version.Minor);
    if (flag & 0x04)
        sprintf(buf + strlen(buf), ".%d", g_version.BuildVersion);
    if (flag & 0x08) {
        strcpy(tmp_currentModuleVersion, g_moduleList[module].version);
        sprintf(buf + strlen(buf), ":%s", tmp_currentModuleVersion);
    }
    sprintf(buf + strlen(buf), "%s", "]");

    //printf("get_Version: buf[%s]\n", buf);
    return strlen(buf);
}


/***
* 注册模块到 g_moduleList 列表
*       返回 当前模块在g_moduleList列表中的位置；
*   g_moduleCount 表示当前列表中已注册的（已拥有的）module数量；
**/
int registerModule(char *name, char *version, int logtype)
{
    int k = 0;
    if (g_moduleCount < 0 || g_moduleCount >= MODULE_LIST_SIZE)
        return -1;

    strncpy((char *)&g_moduleList[g_moduleCount].name, name, 12);
    strncpy((char *)&g_moduleList[g_moduleCount].version, version, 8);
    g_moduleList[g_moduleCount].logType = logtype;

    k = g_moduleCount;
    g_moduleCount++;

    return k;
}


/**
* 从模块列表中获取当前模块名：
* flag: 0: no output
        1: output CurrentModule_Name
*
* 注： 需要预先将模块注册
**/
static int get_CurrentModuleName(char *buf, int module, int flag)
{
    if (flag == 0)
        return 0;
    if (module < 0 || module >= MODULE_LIST_SIZE)
        return -1;

    if (0 != strcmp(g_moduleList[module].name, "")) {
        sprintf(buf, "[%s] ", g_moduleList[module].name);
    } else {
        sprintf(buf, "[%s] ", "0000000");
    }

    //printf("get_CurrentModuleName: buf[%s]\n", buf);
    return strlen(buf);
}


/**
* 计算出错误码值：
*       计算出的 errorcode 是大于 ErrorCode_Base 的
*       计算出的 errorcode 区间（数字从小到大）： system--security--running--operation
**/
static int get_ErrorCode(char *buf, int module, int level)
{
    int logtype = 0;
    int errcode = 0;

    if ((module < 0 || module >= MODULE_LIST_SIZE) || (0 == strcmp(g_moduleList[module].name, "")))
        return -1;

    logtype = g_moduleList[module].logType;

    //TODO: 根据一定规则，由logtype和loglevel计算出errorcode值；
    switch (logtype) {
    case LogType_OPERATION: /** operation type error **/
        errcode = ErrorCode_Base + LogType_OPERATION * 8 + level;
        break;
    case LogType_RUNNING:   /** running type error **/
        errcode = ErrorCode_Base + LogType_RUNNING * 8 + level;
        break;
    case LogType_SECURITY:  /** security type error **/
        errcode = ErrorCode_Base + LogType_SECURITY * 8 + level;
        break;
    case LogType_SYSTEM: /** system type error **/
        errcode = ErrorCode_Base + LogType_SYSTEM * 8 + level;
        break;
    case LogType_ALL:    /** all type error **/
        errcode = ErrorCode_Base + LogType_ALL * 8 + level;
        break;
    default:
        printf("get_ErrorCode: logtype [%d] is error.\n", logtype);
        break;
    }
    sprintf(buf, "<<%d>> ", errcode);

    //printf("get_ErrorCode: buf[%s]\n", buf);
    return strlen(buf);
}


/**
* 设置log输出颜色并输出：
*       日志level :         颜色及输出设置：
        "Fatal!"            1(粗体/高亮)、4(单条下划线)、1(红)
        "Error!"            1(粗体/高亮)、4(单条下划线)、1(红)
        "Warning"           1(粗体/高亮)、4(单条下划线)、3(黄)
        "Info"              4(蓝)
        "Verbose"           2(绿)
        "Debug"             2(绿)
**/
static int useColor = -1;
static int colored_LogOutput(int level, char *str)
{
    if (useColor < 0) {
        useColor = !getenv("NO_COLOR") && !getenv("LOG_FORCE_NOCOLOR") && ((getenv("TERM") && isatty(1)) || getenv("LOG_FORCE_COLOR"));
    }

    if (useColor)
        setLogColor(level);
    if ((level <= LOG_LEVEL_Assert) || (level >= LOG_LEVEL_Undefined)) {
        printf("colored_LogOutput: level is [%d],error.\n", level);
        fputs(str, NULL);
    } else
        fputs(str, stdout);  //输出到stdout
    if (useColor)
        resetLogColor();

    return 0;
}

/**
* 净化log输出信息：
*        可输出的字符范围：   0x08(退格)———— 0x0D(回车键)； 0x20(空格)———— 0x7F(删除)
*        其他字符均使用 ‘？’替代输出；
**/
static void logLineSanitize(uint8_t *str)
{
    int k = 0;
    while (*str) {
        if (*str < 0x08 || (*str > 0x0D && *str < 0x20)) {
            *str = '?';
            k++;
        }
        str++;
    }
    return ;
}



/***
输出格式:
    [month-day-year, weekday][hour:min:sec.msec] [HostMac][HostIP] [Major.Minor.BuildVersion:ModuleVersion][ModuleName] [ErrorCode][file:line][function:LogLevel] : logBuffer_info.
***/
void logVerboseCStyle(const char *file, int line, const char *function, int module, int level, const char *fmt, ...)
{
    va_list args;
    char currTime[48] = {0};
    char machineInfo[64] = {0};
    char version[16] = {0};
    char moduleName[16] = {0};
    char errorCode[16] = {0};
    static char sLogBuffer[512] = { 0 };
    static char str[1024] = {0};

    // date & time
    get_CurrentTime((char *)&currTime, OUTPUTFlag_DateTime);

    // mac & ip
    get_MachineInfo("eth0", (char *)&machineInfo, OUTPUTFlag_MachineInfo);

    // version
    get_Version((char *)&version, module, OUTPUTFlag_Version);

    // module
    get_CurrentModuleName((char *)&moduleName, module, OUTPUTFlag_ModuleName);

    // errorcode
    get_ErrorCode((char *)&errorCode, module, level);

    va_start(args, fmt);
    vsnprintf(sLogBuffer, 512, fmt, args);
    va_end(args);

    snprintf(str, strlen(currTime) + 1, "%s", currTime);
    snprintf(str + strlen(str), strlen(machineInfo) + 1, "%s", machineInfo);
    snprintf(str + strlen(str), strlen(version) + 1, "%s", version);
    snprintf(str + strlen(str), strlen(moduleName) + 1, "%s", moduleName);
    snprintf(str + strlen(str), strlen(errorCode) + 1, "%s", errorCode);
    sprintf(str + strlen(str), "[%s:%d][%s:%s] ", (strchr(file, '/') ?  (strchr(file, '/') + 1) : file), line, function, textLogLevel[level]);
    sprintf(str + strlen(str), ": %s", sLogBuffer);

    logLineSanitize((uint8_t *)str);
    colored_LogOutput(level, str);
    return ;
}




