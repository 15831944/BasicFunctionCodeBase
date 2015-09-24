#ifndef _UDISKCONFIG_H
#define _UDISKCONFIG_H
extern "C" { 
#include "sys_basic_macro.h"
}

#ifdef __cplusplus

namespace Hippo {

typedef struct _UserConfigNode {
	char 	nNtvUserAccount[USER_LEN];
	char 	nNtvUserPassword[USER_LEN+4];
	int 	nConnectType;
	char 	nNetUserAccount[USER_LEN];
	char 	nNetUserPassword[USER_LEN];
	char    nStbIP[NET_LEN];
	char    nNetmask[NET_LEN];
	char    nGateway[NET_LEN];
	char    nDns[NET_LEN];
	char    nDns2[NET_LEN];
	int     nFlag;
}UserConfig_s;

typedef struct _CommonConfigNode {
	char nEdsAddr[URL_LEN];
	char nEdsAddr2[URL_LEN];
	char nUpgradeURL[URL_LEN];
	char nNtpServer[USER_LEN];
	char nNtpServer2[USER_LEN];
	int  nTimezone;
	int  nEpgOffSet_x;
	int  nEpgOffSet_y;
	int  nStandardMode;
	int  nHDMode;
	int  nNetMode;
	char nStbIP[NET_LEN];
	char nNetmask[NET_LEN];
	char nGateway[NET_LEN];
	char nDns[NET_LEN];
	char nDns2[NET_LEN];
	char nTvmsHeartbeatUrl[512];
	char nTvmsVodUrl[512];
	int  nTvmsHeartbeatPeriod;
	int  nTvmsHeartbeatDelay;
	int  nFlag;
}CommonConfig_s;

typedef struct _AccountConfigNode {
	char nAccount[USER_LEN];
	char nUser[32]; // peaple name
	int  nIsInitialzed;
	UserConfig_s *nConfig;
	struct _AccountConfigNode *pNext;
}AccountConfig_s;

AccountConfig_s* UDiskGetUserConfigByIndex(int idx);
void UDiskSetUserConfigData(void);
void UDiskSetCommonConfigData(void);
int UDiskGetUserConfigByUserID(const char* pUserAccount);
int UDiskReadAccountConfigData(void);
int UDiskReadCommonConfigData(void);
int UDiskReadUserConfigData(const char* pAccount);
int UDiskChanageUserStatus(const char* account);

}
#endif

#endif
