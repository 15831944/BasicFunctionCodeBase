#include "UDiskAssertions.h"

#include "UdiskUpgrade.h"
#include "UpgradeManager.h"

#include "MessageTypes.h"
#include "MessageValueSystem.h"
#include "NativeHandler.h"
extern "C" {
#include "sys_msg.h"
#include "mid_middle.h"
#include "app_sys_task.h" // for mid_usb_config_task_create
#include "UtilityTools.h"
}

#include <string.h>

#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <fnmatch.h>
#include <mntent.h>
#include <dirent.h>
#include <errno.h>

extern "C" int yos_systemcall_runSystemCMD(char *buf, int *ret);
extern "C" int UDiskJsCallInit(int type);

typedef enum _UDiskStatus_ {
    eStatusUnknow = 0,
    eUnzipUpgradePacket = 1,
    eUnzipConfigPacket = 2,
    eUnzipUsbUninsert = 3,
    eSameUpgradeVersion = 5, //I don't know this meaning
}UDiskStatus_t;

int gUnzipStatus = (int)eStatusUnknow;
static int gUsbMountNumber = -1;

namespace Hippo {

/**
 * @brief UDiskUpgradePacketDetect
 *
 * @param num the NO.
 *
 * @return  STBType_HuaweiVersion.zip
 */
int UDiskUpgradePacketDetect(int num, void *param)
{/*{{{*/
	int ret = -1;
    DIR* pMountDir = NULL;
    char wTmpBuffer[256] = { 0 };
	char uDiskPath[10] = { 0 };
    char wFileName[64] = { 0 };
    char wFilePath[64] = { 0 };
    char wStbType[32] = { 0 };
	char wMatchTypeStr[64] = { 0 };
    time_t wLastModTime = 0;
	struct stat tComprStatus;
	struct dirent* pDirEntNext =NULL;

	mid_sys_stbtype_get(wStbType);
    sprintf(wMatchTypeStr, "%s_*.zip", wStbType);
	LogUDiskDebug("match type zip: %s\n", wStbType);

	/* Open Usb Directory */
    if (num)
        sprintf(uDiskPath, "/mnt/usb%d", num);
    else
        sprintf(uDiskPath, "/mnt/usb%d", gUsbMountNumber);

	if (!(pMountDir = opendir(uDiskPath))) {
		LogUDiskWarn("opendir(%s) error!\n", uDiskPath);
		return -1;
	}
	while ((pDirEntNext = readdir(pMountDir))) {
        if (!fnmatch(wMatchTypeStr, pDirEntNext->d_name, FNM_PERIOD)) {
            LogUDiskDebug("find %s\n", pDirEntNext->d_name);
            memset(&tComprStatus, 0, sizeof(struct stat));
            memset(wFilePath, 0, sizeof(wFilePath));
            sprintf(wFilePath,"%s/%s", uDiskPath, pDirEntNext->d_name);
            if (!lstat(wFilePath, &tComprStatus)) {
                if(tComprStatus.st_mtime >= wLastModTime){
                    memset(wFileName, 0, sizeof(wFileName));
                    strcpy(wFileName, pDirEntNext->d_name);
                    wLastModTime = tComprStatus.st_mtime;
                }
                LogUDiskDebug("tComprStatus.st_mtime = %d, wLastModTime = %d\n", tComprStatus.st_mtime, wLastModTime);
            }
		}
	}
	if (closedir(pMountDir) < 0)
        LogUDiskDebug("closedir [%s]\n", strerror(errno));
	if(!wFileName[0]){
		LogUDiskDebug ( "UDiskUpgradePacketDetect : no find fit file.zip!\n" );
		return -1;
	}
    memset(wMatchTypeStr, 0, sizeof(wMatchTypeStr));
    strncpy(wMatchTypeStr, wFileName, strlen(wFileName) - strlen(".zip"));
	memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
    sprintf(wTmpBuffer, "%s/upgrade", uDiskPath);
    if (!access(wTmpBuffer, F_OK)) {
        memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
		sprintf(wTmpBuffer, "rm -rf %s/upgrade/%s*", uDiskPath, wStbType);
            yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
    } else {
        memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
        sprintf(wTmpBuffer, "mkdir %s/upgrade", uDiskPath);
        yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
    }

    if (ret)
        LogUDiskWarn("exec %s error.\n", wTmpBuffer);

    gUnzipStatus = eUnzipUpgradePacket;

	if (param) {
        LogUDiskDebug("USB UPGRADE FILE:%s/%s, OSD SHOW\n",uDiskPath, wFileName);
	    UdiskUpgradeStateSet(UpgradeManager::UPGRADE_UNZIP);
	} else {
	    LogUDiskDebug("USB UPGRADE FILE:%s/%s, OPEN UNZIP-SHOW.HTML\n",uDiskPath, wFileName);
        sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzippingPage, 0, 0);
    }
	memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
#if defined(hi3560e)
	sprintf(wTmpBuffer, "/home/yx5532/bin/unzip.dat  -o  \"%s/%s\"  -d  \"%s/upgrade/%s/\"", uDiskPath, wFileName, uDiskPath, wMatchTypeStr);
#elif defined(hi3716m)
	sprintf(wTmpBuffer, "/home/hybroad/bin/unzip.dat  -o  \"%s/%s\"  -d  \"%s/upgrade/%s/\"", uDiskPath, wFileName, uDiskPath, wMatchTypeStr);
#else
    sprintf(wTmpBuffer, "%s/upgrade/%s", uDiskPath, wMatchTypeStr);
    ret = mkdir(wTmpBuffer, 0755);
    if (ret)
        LogUDiskDebug("mkdir %s error !\n", wTmpBuffer);
    memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
	sprintf(wTmpBuffer, "unzip  -o  \"%s/%s\"  -d  \"%s/upgrade/%s/\"", uDiskPath, wFileName, uDiskPath, wMatchTypeStr);
#endif

    ret = -1;
	yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
	if (!ret) { // OK
		sync();
		memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
		sprintf(wTmpBuffer, "rm -rf %s/%s_*.zip", uDiskPath, wStbType);
		yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
		LogUDiskDebug("system : %s, return = %d\n", wTmpBuffer, ret);
        ret = 0;
    } else { // Error
		sync();
		memset(wTmpBuffer,0,sizeof(wTmpBuffer));
		sprintf(wTmpBuffer, "rm -rf \"%s/upgrade\"", uDiskPath);
		yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
        LogUDiskDebug("system : %s, return = %d\n", wTmpBuffer, ret);
        if (param) {
            sleep(1);
            UdiskUpgradeStateSet(UpgradeManager::UPGRADE_UNZIP_FAILED);
        }
#if defined(hi3716m) || defined(brcm7405)
        ret = 50;
#endif
	}
	sync();
	return ret;
}/*}}}*/

/**
 * @brief UDiskUpgradeExecute
 *
 * @param num the NO.
 *
 * @return
 */
int UDiskUpgradeExecute(int num, void *param)
{/*{{{*/
	LogUDiskDebug("U Disk Upgrade [%d]\n", num);
    if (!num)
	    num = gUsbMountNumber;
    char wUpgradePath[64] = { 0 };
    sprintf(wUpgradePath, "/mnt/usb%d/upgrade", num);
    if (0 != access(wUpgradePath, F_OK))
        return -1;
    int ret = UdiskUpgradeStart(wUpgradePath);
    switch (ret ) {
    case 0:
        if (param) {
            UdiskUpgradeBurn();
            return 0;
        } else {
#ifdef INCLUDE_LITTLESYSTEM
            sleep(1);
            exit(0);
#else
            sendMessageToNativeHandler(MessageType_System, USB_UPGRADE_OK, 0, 0);
            UdiskUpgradeBurn();
            return 0;
#endif
        }
    case 1:
        // 1: no upgrade file;
        //  gUnzipStatus = 0;
        return -1;
    case -1:
	 // -1 : upgrade file is invalid
        gUnzipStatus = eSameUpgradeVersion;
        return -1;
    default:
        return -1;
    }
}/*}}}*/

/**
 * @brief UDiskConfigPacketDetect
 *
 * @param num
 * *
 * @return  stbUsbConfig_operatorID_YYYYMMDDHHMMSS.zip
 */
int UDiskConfigPacketDetect(int num)
{/*{{{*/
	int ret = -1;
	DIR* pMountDir =NULL;
	char* p = NULL;
	char* q = NULL;
    char wFileName[256] = { 0 };
    char wFilePath[256] = { 0 };
    char wTmpBuffer[256] = { 0 };
	char uDiskPath[10] = { 0 };
    char wDateOnFile[16] = { 0 };
    long long wLastRecTime = 0;
    long long wDateStrConv = 0;
    time_t wLastModTime = 0;
	struct stat tComprStatus;
	struct dirent* pDirEntNext = NULL;

	if (num)
        sprintf(uDiskPath, "/mnt/usb%d", num);
    else
        sprintf(uDiskPath, "/mnt/usb%d", gUsbMountNumber);
	if (!(pMountDir = opendir(uDiskPath))) {
		LogUDiskDebug("UDiskConfigPacketDetect : opendir(%s) error!\n", uDiskPath);
		return -1;
	}

	while ((pDirEntNext = readdir(pMountDir))) {
		LogUDiskDebug("pDirEntNext->d_name:%s\n", pDirEntNext->d_name);
		p = strstr(pDirEntNext->d_name,"stbUsbConfig_");
		if (p) {
			q = strstr(pDirEntNext->d_name,".zip");
			if (q) {
                memset(wFilePath, 0, sizeof(wFilePath));
                sprintf(wFilePath, "%s/%s", uDiskPath, pDirEntNext->d_name);
				LogUDiskDebug("P STR=%s\n",p);
				// q = strstr(pDirEntNext->d_name + 13,"_");
				q = strrchr(pDirEntNext->d_name + 13, '_');
				if (!q)
					continue;
				LogUDiskDebug("Q STR=%s\n",q);
				memset(wDateOnFile, 0, sizeof(wDateOnFile));
				strncpy(wDateOnFile, q+1, 14);
                wDateStrConv = atoll(wDateOnFile);
				LogUDiskDebug("wDateOnFile STR=%s LLong=%lld\n",wDateOnFile, wDateStrConv);
				if (wDateStrConv > wLastRecTime){
                    wLastRecTime = wDateStrConv;
					memset(wFileName, 0, sizeof(wFileName));
					strcpy(wFileName, pDirEntNext->d_name);
					if (!lstat(wFilePath, &tComprStatus))
                        wLastModTime = tComprStatus.st_mtime;
				} else if (wDateStrConv == wLastRecTime) {
					if (!lstat(wFilePath, &tComprStatus)) {
                        if (tComprStatus.st_mtime > wLastModTime) {
                            memset(wFileName, 0, sizeof(wFileName));
                            strcpy(wFileName, pDirEntNext->d_name);
                            wLastModTime = tComprStatus.st_mtime;
                        }
                    }
				}
			}
		}
	}
	if (closedir(pMountDir) < 0)
        LogUDiskWarn("closedir\n");
	if(!wFileName[0]) {
		LogUDiskDebug("no find fit zip file.\n");
		return -1;
	}
	memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
	sprintf(wTmpBuffer, "%s/usbconfig/", uDiskPath);
	ret = removeFile(wTmpBuffer);
    if (ret)
        LogUDiskDebug("system : rm -rf /mnt/usb%d/usbconfig/ return is %d\n",num,ret);
	LogUDiskDebug("USB CONFIG FILE :%s/%s\n", uDiskPath, wFileName);

	gUnzipStatus = eUnzipConfigPacket;
    sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzippingPage, 0, 0);

	memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
#if defined(hi3560e)
	sprintf(wTmpBuffer, "/home/yx5532/bin/unzip.dat  -o  %s/%s  -d  %s/usbconfig/", uDiskPath, wFileName, uDiskPath);
#elif defined(hi3716m)
	sprintf(wTmpBuffer, "/home/hybroad/bin/unzip.dat  -o  %s/%s  -d  %s/usbconfig/", uDiskPath, wFileName, uDiskPath);
#else
    sprintf(wTmpBuffer, "%s/usbconfig/", uDiskPath);
    ret = mkdir(wTmpBuffer, 0755);
    if (ret)
        LogUDiskDebug("mkdir %s error !\n", wTmpBuffer);
    memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
	sprintf(wTmpBuffer, "unzip  -o  %s/%s  -d  %s/usbconfig/", uDiskPath, wFileName, uDiskPath);
#endif

	yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
	LogUDiskDebug("RET = 0x%x\n",ret);
	if (!ret){ // Ok
		sync();
		memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
		sprintf(wTmpBuffer, "rm -rf %s/stbUsbConfig_*.zip", uDiskPath );
		yos_systemcall_runSystemCMD(wTmpBuffer, &ret);
        if (ret)
            LogUDiskDebug("system : %s, return = %d\n", wTmpBuffer, ret);
        ret = 0;
	} else { // Error
		sync();
		memset(wTmpBuffer, 0, sizeof(wTmpBuffer));
		sprintf(wTmpBuffer, "%s/usbconfig", uDiskPath);
		ret = removeFile(wTmpBuffer);
        if (ret)
            LogUDiskDebug("system : %s, return = %d\n", wTmpBuffer, ret);
        ret = 50;
	}
	sync();
	return ret;
}/*}}}*/

/**
 * @brief UDiskConfigExecute
 *
 * @param num
 *
 * @return
 */
int UDiskConfigExecute(int num)
{/*{{{*/
    LogUDiskDebug("ENTER U_CONFIG_config_CHECK\n");
    if(!num)
        num = gUsbMountNumber;

    int flag = -1;
    char wAccountCfg[64] = { 0 };
    char wCommonCfg[64] = { 0 };
    sprintf(wAccountCfg,"/mnt/usb%d/usbconfig/account.ini", num);
    sprintf(wCommonCfg,"/mnt/usb%d/usbconfig/common.cfg", num);
    if (access(wAccountCfg, R_OK | F_OK ) < 0 && access(wCommonCfg, R_OK | F_OK)) {
        LogUDiskDebug("no this file!--%s--\n", wAccountCfg);
        flag = -1;
    } else {
        LogUDiskDebug("find this file\n");
        flag = 0;
    }
	if (!flag) {
		sendMessageToNativeHandler(MessageType_System, USB_CONFIG_OK, 0, 0);
		return 0;
	}
	return -1;
}/*}}}*/

/**
 * @brief UDiskDetectTask main task of udisk
 *
 */
static void UDiskDetectTask(void* param)
{/*{{{*/
	LogUDiskDebug("u disk thread start\n");
    int flag = 0;
	int ret = -2;

    FILE* pMnt = NULL;
    DIR*  pDir = NULL;
	struct mntent *pMntEntNext = NULL;
    struct dirent *pDirEntNext = NULL;

    char wMatchTypeStr[64] = { 0 };
    mid_sys_stbtype_get(wMatchTypeStr);
    LogUDiskDebug("STB Type [%s]\n", wMatchTypeStr);
    strcat(wMatchTypeStr, "_*.zip");
	for (int i = 0; i < 10; i++) {
        flag = 0;
		gUsbMountNumber = -1;
		if (!(pMnt = setmntent("/proc/mounts", "r"))) {
			LogUDiskDebug("setmntent return NULL\n");
			continue;
		}
		while ((pMntEntNext = getmntent(pMnt))) {
            gUsbMountNumber = -1;
			if (!strncmp( pMntEntNext->mnt_dir, "/mnt/usb", 8)) {
                LogUDiskDebug("pMntEntNext->mnt_dir is [%s]\n", pMntEntNext->mnt_dir);
                gUsbMountNumber = (char)*(pMntEntNext->mnt_dir+8) - 0x30;

                if (!(pDir = opendir(pMntEntNext->mnt_dir))) {
                    LogUDiskWarn("error [%s]\n", strerror(errno));
                    break;
                }
                while ((pDirEntNext = readdir(pDir))) {
                    if (pDirEntNext->d_type == DT_DIR) {
                        if (!strncasecmp(pDirEntNext->d_name, (const char*)"upgrade", 7)) {
                            LogUDiskDebug("find upgrade\n");
                            flag = 1;
                            break;
                        }
                        if (!strncasecmp(pDirEntNext->d_name, (const char*)"usbconfig", 9)) {
                            LogUDiskDebug("find usbconfig\n");
                            flag = 1;
                            break;
                        }
                    } else if (pDirEntNext->d_type == DT_REG) {
                        if (!fnmatch(wMatchTypeStr, pDirEntNext->d_name, FNM_PERIOD)) {
                            LogUDiskDebug("find %s\n", pDirEntNext->d_name);
                            flag = 1;
                            break;
                        }
                        if (!fnmatch("stbUsbConfig_*.zip", pDirEntNext->d_name, FNM_PERIOD)) {
                            LogUDiskDebug("find %s\n", pDirEntNext->d_name);
                            flag = 1;
                            break;
                        }
                    }
                }
                if (closedir(pDir) < 0) {
                    LogUDiskWarn("close dir\n");
                    break;
                }
                if (flag)
                    break;
            }
        }
        endmntent(pMnt);
		LogUDiskDebug("UDiskTask i = %d\n",i);
		if (flag)
            break;
		mid_task_delay(500);
	}
	if (!flag || -1 == gUsbMountNumber) {
            if (param || (Hippo::defNativeHandler().getState() == NativeHandler::Recovery)) {
                sendMessageToNativeHandler(MessageType_Unknow, LITTLE_SYSTEM_UDISK_UPGRADE_FILAID, 0, 0);
                //dinglei
            }
            LogUDiskDebug( "UDiskTask : Cannot find usefull U disk!\n" );
            goto End;
	}
	mid_task_delay(1000);	//add delay for event USB_CONFIG_OK  can't be disposed before boot.html. zp 2012.3.15

    /* step1. Check if have upgrade zip! And unzip. */
    ret = UDiskUpgradePacketDetect(gUsbMountNumber, param);
    if (ret == -1 || ret == 0) { // no progress of unzip  or unzip success
        /* step2. Check if have upgrade directory! And upgrade */
        if (-1 == UDiskUpgradeExecute(gUsbMountNumber, param)) {
            /* step2.5 Check if unzip but no need to upgrade */
            if(eSameUpgradeVersion == gUnzipStatus) {
                if (param) {
                    //sendMessageToNativeHandler(MessageType_Unknow, LITTLE_SYSTEM_UDISK_UPGRADE_FILAID, 0, 0);
                    UdiskUpgradeStateSet(UpgradeManager::UPGRADE_SAME_VERSION);
                    goto End;
                } else {
                    LogUDiskDebug( "OPEN UNZIP-ERROR.HTML config\n" );
                    sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzipErrorPage, 0, 0);
                    goto End;
                }
            } else {
                if (param)
                    goto End;
            }
            /* step3. Check if have config zip! And unzip. */
            ret = UDiskConfigPacketDetect(gUsbMountNumber);
            if (ret == -1 || ret == 0) {
                /* step4. Check if have config directory! */
                if (-1 == UDiskConfigExecute(gUsbMountNumber)) {
                    LogUDiskDebug("don't have any file about usb config and upgrade!\n");
                    if (ret == 0)
                        sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzipErrorPage, 0, 0);
                    goto End;
                }
            } else {
                /* unzip config error using system() */
                LogUDiskDebug("OPEN UNZIP-ERROR.HTML config\n");
                sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzipErrorPage, 0, 0);
            }
        }
    } else {
        /* unzip upgrade error using system() */
        LogUDiskDebug("OPEN UNZIP-ERROR.HTML upgrade\n");
        sendMessageToNativeHandler(MessageType_System, MV_System_OpenUnzipErrorPage, 0, 0);
    }
End:
    LogUDiskDebug("u disk thread end\n");
    return ;
}/*}}}*/

/**
 * @brief UDiskUnzipStatusGet
 *
 * @return
 */
int UDiskUnzipStatusGet(void)
{/*{{{*/
	return gUnzipStatus;
}/*}}}*/

/**
 * @brief UDiskGetMountNumber
 *
 * @return
 */
int UDiskGetMountNumber(void)
{/*{{{*/
    return gUsbMountNumber;
}/*}}}*/

int UDiskUpgradeInEmergency(int param)
{
    if (param == 1)
        mid_usb_config_task_create((mid_func_t)Hippo::UDiskDetectTask, (void *)1);
    else
        mid_usb_config_task_create((mid_func_t)Hippo::UDiskDetectTask, 0);
 
    return 0;
} 
}// End Hippo

extern "C" {
int UDiskConfigInit()
{/*{{{*/
    LogUDiskDebug("start usb disk checking.\n");
#ifdef HISI_U_CONFIG
	if (access("/dev/scsi/host0/bus0/target0/lun0/part1", R_OK | F_OK ) < 0) {
		LogUDiskWarn("none usb device find!\n");
		return -1;
	}
	yos_systemcall_runSystemCMD("mount -t vfat /dev/scsi/host0/bus0/target0/lun0/part1 /mnt/usb0", &ret);
	if(ret){
		LogUDiskWarn("create process failed!\n");
		return -1;
	}
#endif
    mid_usb_config_task_create((mid_func_t)Hippo::UDiskDetectTask, 0);

    UDiskJsCallInit(IoctlContextType_eHWBase);
	return 0;
}/*}}}*/

}