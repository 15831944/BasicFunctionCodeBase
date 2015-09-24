#ifndef __TOP_UTILITY_H__
#define __TOP_UTILITY_H__



#define CSUDI_SUCCESS                                1
#define CSUDIOS_ERROR_BAD_PARAMETER                 -1
#define CSUDIOS_ERROR_FEATURE_NOT_SUPPORTED         -2


/**
 *  Memory Usage rate
 *      内存使用率计算，使用/proc/meminfo文件，计算算法只有一种，如下所示：
 *  
 **/
int GetRAMInfo(unsigned int *puUsedCount, unsigned int *puTotalCount);

int get_mem_info(char* rate);



/**
 *  CPU Usage rate 
 *      处理器的cpu使用率有多种计算方式，不过所有的算法都是大同小异；
 *      一下只是其中一种算法；
 **/
int CSUDIOSGetCPUUsage(unsigned int index, unsigned int *puUsage);




#endif //__TOP_UTILITY_H__
