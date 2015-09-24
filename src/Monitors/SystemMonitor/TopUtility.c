#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logMonitor.h"
#include "TopUtility.h"


//===================   top  ==============================

/**
 *  Memory Usage rate
 *      内存使用率计算，使用/proc/meminfo文件，计算算法只有一种，如下所示：
 *  
 **/
int GetRAMInfo(unsigned int *puUsedCount, unsigned int *puTotalCount)
{
	FILE *fp = NULL;
	int TotalSize = 0, FreeSize = 0, BufferSize = 0, CachedSize = 0;

	if(!puUsedCount)
		return CSUDIOS_ERROR_BAD_PARAMETER;
	if((fp = fopen("/proc/meminfo", "r")) == NULL)
		return CSUDIOS_ERROR_FEATURE_NOT_SUPPORTED;
	fscanf(fp, "MemTotal:%d kB\nMemFree:%d kB\nBuffers:%d kB\nCached:%d kB\n", &TotalSize, &FreeSize, &BufferSize, &CachedSize);
	fclose(fp);
	*puUsedCount = (TotalSize - FreeSize) * 1024;
	*puTotalCount = TotalSize * 1024;
	
	printf("GetRAMInfo:TotalSize[%u], FreeSize[%u], BufferSize[%u], CachedSize[%u]\n", TotalSize, FreeSize, BufferSize, CachedSize);
	return CSUDI_SUCCESS;
}


char* judge_data_plus_or_minus(unsigned int data)
{
    static char tempInfo[128] = {0};
    memset(tempInfo, 0, sizeof(tempInfo));
    if (data + 1 == 0)
        strncpy(tempInfo, "Unknown", sizeof(tempInfo));
    else
        snprintf(tempInfo, sizeof(tempInfo), "%u", data);
    tempInfo[strlen(tempInfo)] = '\0';

    return tempInfo;
}


int get_mem_info(char* rate)
{
	unsigned int total, used;

	int ret = GetRAMInfo(&used, &total);
	sprintf(rate, "%s", judge_data_plus_or_minus((double)used / (double)total * 100));

	printf("get_mem_info:rate=[%s],used[%u],total[%u]\n", rate, used, total);
	return 1;
}



/**
 *  CPU Usage rate 
 *      处理器的cpu使用率有多种计算方式，不过所有的算法都是大同小异；
 *      一下只是其中一种算法；
 *  
 *  
 **/
int CSUDIOSGetCPUUsage(unsigned int index, unsigned int *puUsage)
{
	FILE *fp = NULL;
	int total;
	float work;
	int user_changed, nice_changed, sys_changed, idle_changed, iowait_changed;
	float user, nice, sys, idle, iowait;
	
	int cpu_time_old[2][10];    //User\Nice\Sys\Idle\IoWait\HardIrq\SoftIrq, cpu is all, cpu0 is first cpu, cpu1 is secend cpu...
	int cpu_time_new[2][10];
	
	if((puUsage == NULL) || index < 0 || index > 1) {
		return CSUDIOS_ERROR_BAD_PARAMETER;
	}
	if((fp = fopen("/proc/stat", "r")) == NULL) {
		return CSUDIOS_ERROR_FEATURE_NOT_SUPPORTED;
	}
	fscanf(fp, "cpu %d %d %d %d %d %d %d %d %d %d\ncpu0 %d %d %d %d %d %d %d %d %d %d\n",     
		&cpu_time_old[0][0], &cpu_time_old[0][1], &cpu_time_old[0][2], &cpu_time_old[0][3], &cpu_time_old[0][4],  
		&cpu_time_old[0][5], &cpu_time_old[0][6], &cpu_time_old[0][7], &cpu_time_old[0][8], &cpu_time_old[0][9],  
		&cpu_time_old[1][0], &cpu_time_old[1][1], &cpu_time_old[1][2], &cpu_time_old[1][3], &cpu_time_old[1][4],  
		&cpu_time_old[1][5], &cpu_time_old[1][6], &cpu_time_old[1][7], &cpu_time_old[1][8], &cpu_time_old[1][9]
        );

	rewind(fp);
	usleep(1000000);
	
	fscanf(fp, "cpu %d %d %d %d %d %d %d %d %d %d\ncpu0 %d %d %d %d %d %d %d %d %d %d\n",     
		&cpu_time_new[0][0], &cpu_time_new[0][1], &cpu_time_new[0][2], &cpu_time_new[0][3], &cpu_time_new[0][4],  
		&cpu_time_new[0][5], &cpu_time_new[0][6], &cpu_time_new[0][7], &cpu_time_new[0][8], &cpu_time_new[0][9],  
		&cpu_time_new[1][0], &cpu_time_new[1][1], &cpu_time_new[1][2], &cpu_time_new[1][3], &cpu_time_new[1][4],  
		&cpu_time_new[1][5], &cpu_time_new[1][6], &cpu_time_new[1][7], &cpu_time_new[1][8], &cpu_time_new[1][9]
        );
	fclose(fp);

	user_changed = cpu_time_new[0][0] - cpu_time_old[0][0];
	nice_changed = cpu_time_new[0][1] - cpu_time_old[0][1];
	sys_changed = cpu_time_new[0][2] - cpu_time_old[0][2];
	idle_changed = cpu_time_new[0][3] - cpu_time_old[0][3];
	iowait_changed = cpu_time_new[0][4] - cpu_time_old[0][4];

	total = user_changed + nice_changed + sys_changed + idle_changed + iowait_changed;
	total = total ? total : 1;  //the minium total cpu's changed is 1, avoid divide by zero potential 

	user = (float)(user_changed * 1000 + (float)total / 21 * 10) / total;
	nice = (float)(nice_changed * 1000 + (float)total / 21 * 10) / total;
	sys = (float)(sys_changed * 1000 + (float)total / 21 * 10) / total;
	idle = (float)(idle_changed * 1000 + (float)total / 21 * 10) / total;
	iowait = (float)(iowait_changed * 1000 + (float)total / 21 * 10) / total;

	//(1-(idle2-idle1)/((user1+sys1+nice1+idle1)-(user2+sys2+nice2+idle2)))*100;
	work = (1 -  (float)idle_changed / total) * 100;
	
	*puUsage =  (unsigned int)((user + nice + sys + iowait) / 10);
	printf("CSUDIOSGetCPUUsage:puUsage=%u, user[%f], nice[%f], sys[%f], idle[%f], iowait[%f], work[%f]\n", *puUsage, user, nice, sys, idle, iowait, work);
	
	//*puUsage = (unsigned int)((float)((t2 - t1) - (idle2 - idle1)) * 100 / (float)(t2 - t1));
	//*puUsage = (cpu_time[index][0] + cpu_time[index][1] + cpu_time[index][2]) * 100 / (cpu_time[index][0] + cpu_time[index][1] + cpu_time[index][2] + cpu_time[index][3] + cpu_time[index][4] + cpu_time[index][5] + cpu_time[index][6]);
	
    //printf("CSUDIOSGetCPUUsage:puUsage=%ul, ddd=%f\n", *puUsage, ((t2 - t1) - (idle2 - idle1)) * 100 / (float)(t2 - t1);
	return CSUDI_SUCCESS;
}
