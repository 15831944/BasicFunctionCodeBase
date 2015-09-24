#include "UDiskJsCall.h"
#include "UDiskConfig.h"
#include "UDiskDetect.h"
#include "UDiskAssertions.h"
#include "UDiskQuickInstall.h"

extern "C" { 
#include "app_setting.h"
#include "app_sys_setting.h"
#include "sys_msg.h"
#include "mid_middle.h"

#ifdef TVMS_OPEN
#include "tvms_setting.h"
#endif
}

#include <fnmatch.h>
#include <mntent.h>
#include <dirent.h>
#include <errno.h>

namespace Hippo {

/* Insterface in UDiskDetect.cpp */
static int JseRead_unzipinfo(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return sprintf(value, "%d", UDiskUnzipStatusGet());
}/*}}}*/

static int JseRead_usb_unzip_config_detect(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return sprintf(value, "%d", UDiskConfigPacketDetect(0));
}/*}}}*/

static int JseRead_u_config_check_upgrade(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return sprintf(value, "%d", UDiskUpgradeExecute(0, 0));
}/*}}}*/

static int JseRead_u_config_check_config(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return sprintf(value, "%d", UDiskConfigExecute(0));
}/*}}}*/

/* Insterface in UDiskConfig.cpp */
static int JseRead_sys_UDisk_common_cfg(const char *func, const char *param, char *value, int len)
{/*{{{*/
    char wCommonCfg[64] = { 0 };
    sprintf(wCommonCfg,"/mnt/usb%d/usbconfig/common.cfg", UDiskGetMountNumber());
    if (access(wCommonCfg, R_OK | F_OK ) < 0) 
        strcpy(value, "-1");
    else 
        strcpy(value, "0");
    return 0;
}/*}}}*/

static int JseRead_sys_UDisk_common_set(const char *func, const char *param, char *value, int len)
{/*{{{*/
    int flag;

    flag = UDiskReadCommonConfigData();
    if (flag == -1) {
        LogUDiskDebug("parse err,please check the common.cfg file!!!\n");
        sprintf(value, "%d", -1);
        return -1;
    }
    UDiskSetCommonConfigData();
#ifdef TVMS_OPEN
    tvms_config_save();
#endif
    sys_config_save();

    LogUDiskDebug("common set flag [%d]\n", flag);
    return sprintf(value, "%d", flag);
}/*}}}*/

static int JseWrite_sys_UDiskUserInfo_set(const char *func, const char *param, char *value, int len)
{/*{{{*/
    UDiskSetCommonConfigData();
    UDiskSetUserConfigData();
    customer_config_save();
    sys_config_save();
    UDiskChanageUserStatus(value);
    mid_task_delay(1000);
    return 0;
}/*}}}*/

static int JseRead_sys_UDiskUserInfo_count(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return snprintf(value, len, "%d", UDiskReadAccountConfigData());
}/*}}}*/

static int JseRead_sys_UDiskUserInfo(const char *func, const char *param, char *value, int len)
{/*{{{*/
    AccountConfig_s* pAccountCfg = UDiskGetUserConfigByIndex(atoi(param));
    if (!pAccountCfg) {
        LogUDiskDebug( "wrong user account num.\n" );
        return -1;
    }
    LogUDiskDebug("account = %s, user name = %s\n",pAccountCfg->nAccount, pAccountCfg->nUser);
    return snprintf(value, len, "%s^%s^%d", pAccountCfg->nAccount, pAccountCfg->nUser, pAccountCfg->nIsInitialzed);
}/*}}}*/

static int JseRead_sys_UDiskUser_num_by_userid(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return snprintf(value, len, "%d", UDiskGetUserConfigByUserID(param));
}/*}}}*/

static int JseRead_sys_UDiskUser_cfgflag(const char *func, const char *param, char *value, int len)
{/*{{{*/
    return snprintf(value, len, "%d", UDiskReadUserConfigData(param));
}/*}}}*/

/* Insterface in UDiskQuickInstall */
static int JseWriteLogUDiskInstallStart(const char *func, const char *param, char *value, int len)
{/*{{{*/
    if (!func || !param || !value)
        return -1;

    int   uDiskID = -1;
    FILE* pMnt = NULL;
    DIR*  pDir = NULL; 
    struct mntent *pMntEntNext = NULL;  
    struct dirent *pDirEntNext = NULL;

    if (!(pMnt = setmntent("/proc/mounts", "r"))) {
        LogUDiskDebug("setmntent return NULL\n");
        return -1;
    }
    while (uDiskID == -1 && (pMntEntNext = getmntent(pMnt))) {
        if (!strncmp( pMntEntNext->mnt_dir, "/mnt/usb", 8)) {
            LogUDiskDebug("pMntEntNext->mnt_dir is [%s]\n", pMntEntNext->mnt_dir);
            if (!(pDir = opendir(pMntEntNext->mnt_dir))) {
                LogUDiskWarn("error [%s]\n", strerror(errno));
                break;
            }
            while ((pDirEntNext = readdir(pDir))) {
                if (!strncasecmp(pDirEntNext->d_name, (const char*)"CUSTBConfig", 11)) {
                    uDiskID = (char)*(pMntEntNext->mnt_dir + 8) - 0x30;
                    LogUDiskDebug("find CUSTBConfig\n");
                    break;
                }
            }
            if (closedir(pDir) < 0) {
                LogUDiskWarn("close dir\n");
                break;
            }
        }
    }
    endmntent(pMnt);
    return UDiskInstallStart(uDiskID);
}/*}}}*/
} 

extern "C" { 
static int bJsCallIsInit = 0;
int 
UDiskJsCallInit(int type) 
{/*{{{*/
    LogUDiskDebug("usb disk js call register.\n");
    if (!bJsCallIsInit) {
        a_Hippo_API_JseRegister("sys_UDiskUserInfo_set", NULL, Hippo::JseWrite_sys_UDiskUserInfo_set,(ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDisk_common_cfg", Hippo::JseRead_sys_UDisk_common_cfg, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDisk_common_ret", Hippo::JseRead_sys_UDisk_common_set, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDiskUserInfo_count", Hippo::JseRead_sys_UDiskUserInfo_count, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDiskUserInfo", Hippo::JseRead_sys_UDiskUserInfo, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDiskUser_num_by_userid", Hippo::JseRead_sys_UDiskUser_num_by_userid, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("sys_UDiskUser_cfgflag", Hippo::JseRead_sys_UDiskUser_cfgflag, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("unzipinfo", Hippo::JseRead_unzipinfo, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("usb_unzip_config_detect", Hippo::JseRead_usb_unzip_config_detect, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("u_config_check_upgrade", Hippo::JseRead_u_config_check_upgrade, NULL, (ioctl_context_type_e)type);
        a_Hippo_API_JseRegister("u_config_check_config", Hippo::JseRead_u_config_check_config, NULL, (ioctl_context_type_e)type);

        a_Hippo_API_JseRegister("u_config_CUtr069_set", NULL, Hippo::JseWriteLogUDiskInstallStart, (ioctl_context_type_e)type);
        bJsCallIsInit = 1;
    }
    return 0;
}/*}}}*/

int 
UDiskJsCallQuit(int type) 
{/*{{{*/
    LogUDiskDebug("usb disk js call unregister.\n");
    if (bJsCallIsInit) {
        a_Hippo_API_UnJseRegister("sys_UDiskUserInfo_set", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDisk_common_cfg", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDisk_common_ret", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDiskUserInfo_count", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDiskUserInfo", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDiskUser_num_by_userid", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("sys_UDiskUser_cfgflag", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("unzipinfo", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("usb_unzip_config_detect", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("u_config_check_upgrade", (ioctl_context_type_e)type);
        a_Hippo_API_UnJseRegister("u_config_check_config", (ioctl_context_type_e)type);
        bJsCallIsInit = 0;
    }
    return 0;
}/*}}}*/

}
