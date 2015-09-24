/**
 *  InitPlayer.c文件
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 *
 **/

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>


#include "log/LogC.h"
#include "../LogPlayer.h"
#include "Player.h"
#include "UtilityTime.h"




int TimeDelay(const int delayMode, int delaySecond, int delayMicroSecond)
{
    if (delayMode < TIME_DELAY_MODE_RESERVED || delayMode > TIME_DELAY_MODE_MAX) {
        printf("TimeDelay\n");
        return -1;
    }
    fd_set rset;
    struct timeval tv;
    int fd = 1;
    int ret;


    switch (delayMode) {
    case TIME_DELAY_MODE_SELECT:  /*select实现的延时可以被捕捉到的信号中断，精确度为 10毫秒。在10毫秒以上很精确*/

        FD_ZERO(&rset);
        FD_SET((uint32_t)fd, &rset);
        tv.tv_sec = delaySecond;
        tv.tv_usec = delayMicroSecond;
        ret = select(fd + 1, &rset , NULL,  NULL, &tv);
        printf("testCommon ret = [%d].\n", ret);
        if ( ret != 1) {
            printf("TimeDelay:::select delay time return value = [%d].\n", ret);
            return -1;
        }
        break;
    case TIME_DELAY_MODE_SLEEP:  /*sleep函数是能够进行秒级的延时操作，可以在多线程中使用，只阻塞本线程，不影响所属进程中的其它线程*/
        ret = sleep (delaySecond);
        if (-1 == ret) {
            fprintf (stderr, " sleep error . errno=%d [%s]\n", errno, strerror (errno));
        }
        break;
    case TIME_DELAY_MODE_USLEEP:/*usleep是能够进行微秒级别的延时，linux下的usleep， nanosleep  很不精确*/
        ret = usleep (delayMicroSecond);
        if (-1 == ret) {
            fprintf (stderr, " usleep error . errno=%d [%s]\n", errno, strerror (errno));
        }
        break;
    case TIME_DELAY_MODE_UDELAY:    /*软件循环指定的微妙数,函数只能用于获取较短的时间延迟，*/
        /*因为loops_per_second值的精度只有8位,计算更长的延迟时会积累出相当大的误差,函数的参数的最大值是取1000微秒(1毫秒)*/
        /*udelay 是个忙等待函数（所以 mdelay 也是），在延迟的时间段内无法运行其他的任务*/
        break;
    case TIME_DELAY_MODE_MDELAY:  /*调用前者达到延迟毫秒级,延迟大于 11 毫秒时可以使用函数 mdelay, mdelay 也是个忙等待函数，在延迟的时间段内无法运行其他的任务*/
        break;
    default:
        break;
    }

    return 0;
}




//=================================================================================================
//=================================================================================================
/**

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


//用条件变量来实现延时的操作；
int MutexTimeDelay()
{
    pthread_cond_t mycond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
    struct timespec ts;
    int rv;
    ts.tv_sec = 0;
    ts.tv_nsec = 500000; // 500,000 nanoseconds = 500 ms


    pthread_mutex_lock(&mymutex);
    rv = pthread_cond_timedwait(&mycond, &mymutex, &ts);
    switch(rv) {
    case ETIMEDOUT:
            // Handle timeout //
    case EINTR:
            // Interupted by signal //
    case EBUSY:
    default:
            // Handle errors //
            // condition received a condition signal //
    }
    pthread_mutex_unlock(&mymutex);

}
**/




















