#ifndef __UTILITY_TIME_H__
#define __UTILITY_TIME_H__




enum delayMode{
    TIME_DELAY_MODE_RESERVED,
    TIME_DELAY_MODE_SELECT,     /*select实现的延时可以被捕捉到的信号中断，精确度为 10毫秒。在10毫秒以上很精确*/
    TIME_DELAY_MODE_SLEEP,      /*sleep函数是能够进行秒级的延时操作，可以在多线程中使用，只阻塞本线程，不影响所属进程中的其它线程*/
    TIME_DELAY_MODE_USLEEP,     /*usleep是能够进行微秒级别的延时，linux下的usleep， nanosleep  很不精确*/
    TIME_DELAY_MODE_UDELAY,     /*软件循环指定的微妙数,函数只能用于获取较短的时间延迟，因为loops_per_second值的精度只有8位,计算更长的延迟时会积累出相当大的误差,函数的参数的最大值是取1000微秒(1毫秒),udelay 是个忙等待函数（所以 mdelay 也是），在延迟的时间段内无法运行其他的任务*/
    TIME_DELAY_MODE_MDELAY,     /*调用前者达到延迟毫秒级,延迟大于 11 毫秒时可以使用函数 mdelay, mdelay 也是个忙等待函数，在延迟的时间段内无法运行其他的任务*/
    TIME_DELAY_MODE_MAX
};


int TimeDelay(const int delayMode, int delaySecond, int delayMicroSecond);















#endif  //__UTILITY_TIME_H__

