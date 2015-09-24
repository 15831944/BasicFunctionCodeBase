#ifndef _UDISKDETECT_H_
#define _UDISKDETECT_H_
namespace Hippo {
int UDiskGetMountNumber(void);
int UDiskUnzipStatusGet(void);
int UDiskUpgradeExecute(int num, void *param);
int UDiskConfigExecute(int num);
int UDiskUpgradePacketDetect(int num, void *param);
int UDiskConfigPacketDetect(int num);
int UDiskUpgradeInEmergency(int parm);

}
#endif
