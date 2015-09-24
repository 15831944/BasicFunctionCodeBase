#include "UDiskConfig.h"
#include "UDiskJsCall.h"
#include "UDiskDetect.h"
#include "UDiskAssertions.h"

extern "C" {
#include "app_setting.h"
#include "app_sys_setting.h"
#include "sys_msg.h"
#include "mid_middle.h"
#include "app_sys_task.h"

#include "b200_3des_api.h"
#include "telecom_config.h"
#include "app_sys.h"

#ifdef TVMS_OPEN
#include "tvms_setting.h"
#endif
}

#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

namespace Hippo {

static int gUserNum = 0;
static int gCommonParserFlag = 0;
static int gParseAccountInited = 0;

static AccountConfig_s* gAccountCfgLinkHead = NULL;
static CommonConfig_s gCommonCfg;
static UserConfig_s gUserCfg;

static int _Decrypt3DES(char *in, char *out);
static void _CopyRightValue(char *out, char *in, unsigned int len);
static int _ParseAccountConfigData(const char* filepath);
static int _ParseCommonConfigData(const char* filepath);
static int _ParseUserConfigData(const char* filepath);

static void _CopyRightValue(char* out, char* in, unsigned int outlen)
{/*{{{*/
    if(out == NULL || in == NULL) {
        LogUDiskDebug("_CopyRightValue error\n");
        return;
    }
    int len = 0;
    len = strlen(in);
    if(outlen < strlen(in))
        len = outlen;
    memcpy(out, in, len);
    out[len] = '\0';
}/*}}}*/

static int _Decrypt3DES(char* in, char* out)
{/*{{{*/
    static int init_decrypt_key = 0;
    //设置3DES的密钥
    if(init_decrypt_key == 0) {
        char tempout[256] = {0};
        char array[16] = "huaweiuser";;

        b200_md5_get_key(tempout, array, (unsigned char*)"99991231", 1, 1);
        b200_set_3des_key(tempout);
        init_decrypt_key = 1;
    }
    int outlen = 0;
    int ret = 0;
    char *temp = NULL;
    ret = b200_decode_base64_3des(in, out , &outlen);
    if(ret == -1) {
        LogUDiskDebug("the string is error ,not a 3des\n");
        return -1;
    }
    temp = strstr(out, "\r");
    if(temp != NULL) {
        *temp = '\0';
    }
    LogUDiskDebug("the decrpt buffer =%s\n", out);
    return 0;
}/*}}}*/

static int _ParseAccountConfigData(const char* filepath)
{/*{{{*/
    LogUDiskDebug("parse account data\n");
    FILE* fAccountCfg = NULL;
    char  wLocalBuffer[528] = { 0 };
    char *pRightValue = NULL;
    char *pStr = NULL;
    short wParseSign = 0;
    AccountConfig_s* pAccountNode = NULL;
    AccountConfig_s* tAccountNode = NULL;
    fAccountCfg = fopen(filepath, "rb");
    if(!fAccountCfg) {
        LogUDiskDebug("open account file failed\n");
        return -1;
    }
    gAccountCfgLinkHead = (AccountConfig_s*)calloc(1, sizeof(AccountConfig_s)); /* head node */
    if(!gAccountCfgLinkHead) {
        LogUDiskWarn("create head node.\n");
        goto Err;
    }

    while(true) {
        pAccountNode = (AccountConfig_s*)calloc(1, sizeof(AccountConfig_s));
        if(!pAccountNode)
            goto Err;
        for(wParseSign = 0; wParseSign < 3; ++wParseSign) {
            if(fgets(wLocalBuffer, 528, fAccountCfg)) {
                // be compatible with sign '\n'
                pStr = strstr(wLocalBuffer, "\r\n");
                if(!pStr) {
                    pStr = strstr(wLocalBuffer, "\n");
                    if(!pStr)
                        continue;
                    else
                        *pStr = '\0';
                } else
                    *pStr = '\0';

                switch(wParseSign) {
                    case 0:
                        pRightValue = strstr(wLocalBuffer, "[");
                        if(!pRightValue)
                            goto Err;
                        _CopyRightValue(pAccountNode->nAccount, pRightValue + 1, 31);
                        pRightValue = strstr(pAccountNode->nAccount, "]");
                        if(pRightValue)
                            *pRightValue = '\0';
                        break;
                    case 1:
                        pRightValue = strstr(wLocalBuffer, "=");
                        if(!pRightValue)
                            goto Err;
                        _CopyRightValue(pAccountNode->nUser, pRightValue + 1, 31);
                        break;
                    case 2:
                        pRightValue = strstr(wLocalBuffer, "=");
                        if(!pRightValue)
                            goto Err;
                        pAccountNode->nIsInitialzed = atoi(pRightValue + 1);
                        break;
                    default:
                        break;
                }
            } else {
                LogUDiskDebug("file end\n");
                free(pAccountNode);
                fclose(fAccountCfg);
                return 0;
            }
            memset(wLocalBuffer, 0, 528);
        }
        //Use tail insert
        tAccountNode = gAccountCfgLinkHead;
        while (tAccountNode->pNext)
            tAccountNode = tAccountNode->pNext;
        tAccountNode->pNext = pAccountNode;
        //pAccountNode->pNext = gAccountCfgLinkHead->pNext;
        //gAccountCfgLinkHead->pNext = pAccountNode;
        gUserNum++;
    }
Err:
    fclose(fAccountCfg);
    if(pAccountNode)
        free(pAccountNode);
    return -1;
}/*}}}*/

static int _ParseCommonConfigData(const char* filepath)
{/*{{{*/
    LogUDiskDebug("parse common data\n");
    FILE* fCommonCfg = NULL;
    char* pStr = NULL;
    char* pRightValue = NULL;
    char wLocalBuffer[528];
    int wModFlag = 0;
    int wFullErr = 1;
    char tempstr[64] = {0};
    if(gCommonParserFlag) {
        LogUDiskDebug("parse common config is parse already\n");
        return 0;
    }
    fCommonCfg = fopen(filepath, "rb");
    if(!fCommonCfg) {
        LogUDiskDebug("open config info file failed\n");
        return -1;
    }
    memset(&gCommonCfg, 0, sizeof(CommonConfig_s));
    gCommonCfg.nTimezone = (int)0x80000000;
    while(fgets(wLocalBuffer, 528, fCommonCfg)) {
        // be compatible with sign '\n'
        pStr = strstr(wLocalBuffer, "\r\n");
        if(!pStr) {
            pStr = strstr(wLocalBuffer, "\n");
            if(!pStr)
                continue;
            else
                *pStr = '\0';
        } else
            *pStr = '\0';

        LogUDiskDebug("fgets = [%s]\n", wLocalBuffer);
        pRightValue = strstr(wLocalBuffer, "=");
        if(!pRightValue)
            continue;
        int str_len_right = pStr - pRightValue;

        pRightValue += 1;
        str_len_right -= 1;
        if(str_len_right == 0) {
            wModFlag = -1;
            continue;
        }

        while(*(pRightValue) == 0x20 || *(pRightValue) == 0) {
            pRightValue += 1;
            str_len_right -= 1;
            if(str_len_right == 0 || *(pRightValue) == 0) {
                wModFlag = -1;
                continue;
            }
        }
        LogUDiskDebug(" pRightValue=%s\n", pRightValue);

        if(!strncmp("stbIP", wLocalBuffer, strlen("stbIP"))) {
            _CopyRightValue(gCommonCfg.nStbIP, pRightValue, 15);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("netmask", wLocalBuffer, strlen("netmask"))) {
            _CopyRightValue(gCommonCfg.nNetmask, pRightValue, 15);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("gateway", wLocalBuffer, strlen("gateway"))) {
            _CopyRightValue(gCommonCfg.nGateway, pRightValue, 15);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("dns2", wLocalBuffer, sizeof("dns2") - 1)) {
            _CopyRightValue(gCommonCfg.nDns2, pRightValue, 15);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("dns", wLocalBuffer, sizeof("dns") - 1)) {
            _CopyRightValue(gCommonCfg.nDns, pRightValue, 15);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("edsAddr2", wLocalBuffer, sizeof("edsAddr2") - 1)) {
            _CopyRightValue(gCommonCfg.nEdsAddr2, pRightValue, 255);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("edsAddr", wLocalBuffer, sizeof("edsAddr") - 1)) {
            _CopyRightValue(gCommonCfg.nEdsAddr, pRightValue, 255);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("upgradeURL", wLocalBuffer, sizeof("upgradeURL") - 1)) {
            _CopyRightValue(gCommonCfg.nUpgradeURL, pRightValue, 255);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("ntpServer2", wLocalBuffer, sizeof("ntpServer2") - 1)) {
            _CopyRightValue(gCommonCfg.nNtpServer2, pRightValue, 31);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("ntpServer", wLocalBuffer, sizeof("ntpServer") - 1)) {
            _CopyRightValue(gCommonCfg.nNtpServer, pRightValue, 31);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("timezone", wLocalBuffer, sizeof("timezone") - 1)) {
            gCommonCfg.nTimezone = atoi(pRightValue);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("epgOffset_x", wLocalBuffer, sizeof("epgOffset_x") - 1)) {
            gCommonCfg.nEpgOffSet_x = atoi(pRightValue);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("epgOffset_y", wLocalBuffer, sizeof("epgOffset_y") - 1)) {
            gCommonCfg.nEpgOffSet_y = atoi(pRightValue);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("standardMode", wLocalBuffer, sizeof("standardMode") - 1)) {
            _CopyRightValue(tempstr, pRightValue, 63);
            if(!strcasecmp(tempstr, "pal")) {
                gCommonCfg.nStandardMode = VideoFormat_PAL;
            } else if(!strcasecmp(tempstr, "ntsc")) {
                gCommonCfg.nStandardMode = VideoFormat_NTSC ;
            }
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("HDMode", wLocalBuffer, sizeof("HDMode") - 1)) {
            _CopyRightValue(tempstr, pRightValue, 63);
            if(!strcasecmp(tempstr, "pal")) {
                gCommonCfg.nStandardMode = VideoFormat_PAL;
            } else if(!strcasecmp(tempstr, "ntsc")) {
                gCommonCfg.nStandardMode = VideoFormat_NTSC ;
            } else if(!strcasecmp(tempstr, "480p")) {
                gCommonCfg.nStandardMode = VideoFormat_480P ;
            } else if(!strcasecmp(tempstr, "576p")) {
                gCommonCfg.nStandardMode = VideoFormat_576P ;
            } else if(!strcasecmp(tempstr, "720p-60hz")) {
                gCommonCfg.nStandardMode = VideoFormat_720P60HZ ;
            } else if(!strcasecmp(tempstr, "720p-50hz")) {
                gCommonCfg.nStandardMode = VideoFormat_720P50HZ ;
            } else if(!strcasecmp(tempstr, "1080i-60hz")) {
                gCommonCfg.nStandardMode = VideoFormat_1080I60HZ ;
            } else if(!strcasecmp(tempstr, "1080i-50hz")) {
                gCommonCfg.nStandardMode = VideoFormat_1080I50HZ ;
            } else if(!strcasecmp(tempstr, "1080p-30hz")) {
                gCommonCfg.nStandardMode = VideoFormat_1080P30HZ ;
            } else if(!strcasecmp(tempstr, "1080p-25hz")) {
                gCommonCfg.nStandardMode = VideoFormat_1080P25HZ ;
            }

            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("netMode", wLocalBuffer, sizeof("netMode") - 1)) {
            int			nNetMode = 0;

            if((nNetMode = atoi(pRightValue)) == 0)  //DHCP
                nNetMode = 2;
            else if(1 == nNetMode)//PPPOE
                nNetMode = 1;
            else if(2 == nNetMode)//static IP
                nNetMode = 3;
            else
                nNetMode = 2;
            gCommonCfg.nNetMode = nNetMode;
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("connecttype", wLocalBuffer, sizeof("connecttype") - 1)) {
            int	nNetMode =  atoi(pRightValue);
            gCommonCfg.nNetMode = nNetMode;
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        }
#ifdef TVMS_OPEN
        else if(!strncmp("tvms_heartbeat_url", wLocalBuffer, sizeof("tvms_heartbeat_url") - 1)) {
            _CopyRightValue(gCommonCfg.nTvmsHeartbeatUrl, pRightValue, 511);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("tvms_vod_url", wLocalBuffer, sizeof("tvms_vod_url") - 1)) {
            _CopyRightValue(gCommonCfg.nTvmsVodUrl, pRightValue, 511);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if((!strncmp("tvms_heartbeat_period", wLocalBuffer, sizeof("tvms_heartbeat_period") - 1))
            || (!strncmp("tvms_heartbeat_ period", wLocalBuffer, sizeof("tvms_heartbeat_ period") - 1))) {  //规格文档中已拼错，应没空格，故在此兼容两种拼写
            gCommonCfg.nTvmsHeartbeatPeriod = atoi(pRightValue);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        } else if(!strncmp("tvms_heartbeat_delay", wLocalBuffer, sizeof("tvms_heartbeat_delay") - 1)) {
            gCommonCfg.nTvmsHeartbeatDelay = atoi(pRightValue);
            memset(wLocalBuffer, 0, 528);
            wFullErr = 0;
            continue;
        }
#endif
        else {
            LogUDiskDebug("this is param is unkown\n");
        }
        memset(wLocalBuffer, 0, 528);
    }
    gCommonCfg.nFlag = 1;
    fclose(fCommonCfg);
    gCommonParserFlag = 1;
    LogUDiskDebug("wFullErr[%d] wModFlag[%d]\n", wFullErr, wModFlag);
    if(wFullErr)
        return -1;
    else if(wModFlag == -1)
        return -2;
    else
        return 0;
}/*}}}*/

static int _ParseUserConfigData(const char* filepath)
{/*{{{*/
    LogUDiskDebug("parse user encrypt data\n");
    FILE *fUserCfg = NULL;
    char wLocalBuffer[528];
    char wTempBuffer[528];
    char *pRightValue = NULL;
    char *p = NULL;
    int flag = 0;
    int ret = 0;
    UserConfig_s* pUserCfg = &gUserCfg;

    fUserCfg = fopen(filepath, "rb");
    if(!fUserCfg) {
        LogUDiskDebug("open config info file failed\n");
        return -2;
    }
    memset(pUserCfg, 0, sizeof(pUserCfg));
    while(fgets(wTempBuffer, 528, fUserCfg)) {
        //去掉fgets的最后一个换行符 "\r\n", 兼容\n
        p = strstr(wTempBuffer, "\r\n");
        if(p == NULL) {
            p = strstr(wTempBuffer, "\n");
            if(p == NULL)
                continue;
            if(p != NULL)
                *p = '\0';
        } else {
            *p = '\0';
        }

        memset(wLocalBuffer, 0, 528);
        ret = _Decrypt3DES(wTempBuffer, wLocalBuffer);
        if(ret == -1)
            continue;

        pRightValue = strstr(wLocalBuffer, "=");
        if(pRightValue != NULL)
            pRightValue++;
        else {
            memset(wTempBuffer, 0, 528);
            continue;
        }

        if(*(pRightValue) == 0x20)
            pRightValue++;

        if(!strncmp("ntvuseraccount", wLocalBuffer, sizeof("ntvuseraccount") - 1))
            _CopyRightValue(pUserCfg->nNtvUserAccount, pRightValue, USER_LEN - 1);
        else if(!strncmp("ntvuserpassword", wLocalBuffer, sizeof("ntvuserpassword") - 1))
            _CopyRightValue(pUserCfg->nNtvUserPassword, pRightValue, USER_LEN + 4 - 1);
        else if(!strncmp("netuseraccount", wLocalBuffer, sizeof("netuseraccount") - 1))
            _CopyRightValue(pUserCfg->nNetUserAccount, pRightValue, USER_LEN - 1);
        else if(!strncmp("netuserpassword", wLocalBuffer, sizeof("netuserpassword") - 1))
            _CopyRightValue(pUserCfg->nNetUserPassword, pRightValue, USER_LEN - 1);
        else if(!strncmp("connecttype", wLocalBuffer, sizeof("connecttype") - 1))
            pUserCfg->nConnectType = atoi(pRightValue);
        else if(!strncmp("stbIP", wLocalBuffer, sizeof("stbIP") - 1))
            _CopyRightValue(pUserCfg->nStbIP, pRightValue, 15);
        else if(!strncmp("netmask", wLocalBuffer, sizeof("netmask") - 1))
            _CopyRightValue(pUserCfg->nNetmask, pRightValue, 15);
        else if(!strncmp("gateway", wLocalBuffer, sizeof("gateway") - 1))
            _CopyRightValue(pUserCfg->nGateway, pRightValue, 15);
        else if(!strncmp("dns2", wLocalBuffer, sizeof("dns2") - 1))
            _CopyRightValue(pUserCfg->nDns2, pRightValue, 15);
        else if(!strncmp("dns", wLocalBuffer, sizeof("dns") - 1))
            _CopyRightValue(pUserCfg->nDns, pRightValue, 15);
        else
            LogUDiskDebug("error, this localbuf =%s=is unknow\n", wLocalBuffer);
        memset(wTempBuffer, 0, 528);
    }

    LogUDiskDebug("connecttype=%d, ntvuseraccount=%s, ntvuserpassword=%s, netuseraccount=%s, netuserpassword=%s\n",
        pUserCfg->nConnectType, pUserCfg->nNtvUserAccount, pUserCfg->nNtvUserPassword, pUserCfg->nNetUserAccount, pUserCfg->nNetUserPassword);

    if(pUserCfg->nConnectType < 1 || pUserCfg->nConnectType > 3)
        flag = -1;
    if(strlen(pUserCfg->nNtvUserAccount) <= 0)
        flag = -1;
    if(pUserCfg->nConnectType == 1 && strlen(pUserCfg->nNetUserAccount) <= 0)
        flag = -1;

    fclose(fUserCfg);
    return flag;
}/*}}}*/

int UDiskReadAccountConfigData(void)
{/*{{{*/
    LogUDiskDebug("read account.ini file.\n");
    if(!gParseAccountInited) {
        char wAccountCfg[64] = { 0 };
        sprintf(wAccountCfg, "/mnt/usb%d/usbconfig/account.ini", UDiskGetMountNumber());
        if(!access(wAccountCfg, R_OK | F_OK)) {
            if(!_ParseAccountConfigData(wAccountCfg))
                gParseAccountInited = 1;
        }
    }
    LogUDiskDebug("user account num : [%d] \n", gUserNum);
    return gUserNum;
}/*}}}*/

int UDiskReadCommonConfigData(void)
{/*{{{*/
    LogUDiskDebug("read common.cfg file.\n");
    if(!gCommonCfg.nFlag) {
        char wCommonCfg[64] = { 0 };
        sprintf(wCommonCfg, "/mnt/usb%d/usbconfig/common.cfg", UDiskGetMountNumber());
        if(access(wCommonCfg, R_OK | F_OK) < 0) {
            LogUDiskDebug("no this file! : COMON_FILE \n");
            return -1;
        }
        return _ParseCommonConfigData(wCommonCfg);
    }
    return 0;
}/*}}}*/

int UDiskReadUserConfigData(const char* pUserAccount)
{/*{{{*/
    int ret = 0;
    AccountConfig_s* pAccountCfg = (NULL != gAccountCfgLinkHead) ? gAccountCfgLinkHead->pNext : NULL;
    if(!pUserAccount || !pAccountCfg || !(pAccountCfg->pNext)) {
        LogUDiskDebug("error : pAccountCfg->pNext is NULL\n");
        return -2;
    }
    LogUDiskDebug("account is %s\n", pUserAccount);
    while (pAccountCfg) {
        LogUDiskDebug("account = %s, user = %s, filenameLen =%d\n", pAccountCfg->nAccount, pAccountCfg->nUser, strlen(pUserAccount));
        if((strlen(pAccountCfg->nAccount)) && (!strcmp(pUserAccount, pAccountCfg->nAccount))) {
            char wUserCfg[64] = { 0 };
            sprintf(wUserCfg, "/mnt/usb%d/usbconfig/%s.cfg", UDiskGetMountNumber(), pAccountCfg->nAccount);
            LogUDiskDebug("read %s.cfg file\n", pAccountCfg->nAccount);
            ret = _ParseUserConfigData(wUserCfg);
            if(-1 == ret) {
                LogUDiskWarn("this key file is error\n");
                return -1;
            } else if(-2 == ret) {
                LogUDiskWarn("there is no this file\n");
                return -2;
            }
            gUserCfg.nFlag = 1;
            break;
        }
        pAccountCfg = pAccountCfg->pNext;
    }
    return 0;
}/*}}}*/

AccountConfig_s* UDiskGetUserConfigByIndex(int idx)
{/*{{{*/
    AccountConfig_s* pAccountCfg = gAccountCfgLinkHead;
    if(idx >= gUserNum || idx < 0)
        return NULL;
    LogUDiskDebug("idx [%d]\n", idx);
    idx++;
    for(int i = 0; i < idx && pAccountCfg; i++) {
        pAccountCfg = pAccountCfg->pNext;
        LogUDiskDebug("account[%d] = [%s]\n", i, pAccountCfg->nAccount);
    }

    return pAccountCfg;
}/*}}}*/

int UDiskGetUserConfigByUserID(const char* pUserAccount)
{/*{{{*/
    if(!gAccountCfgLinkHead)
        return -1;

    AccountConfig_s* pAccountCfg = (NULL != gAccountCfgLinkHead) ? gAccountCfgLinkHead->pNext : NULL;
    int wIdx = 0;

    while (pAccountCfg) {
        if (!strcmp(pAccountCfg->nAccount, pUserAccount)) {
            LogUDiskDebug("DO find UserAccount, Index:%d, Account:%s\n", wIdx, pAccountCfg->nAccount);
            return wIdx;
        }
        wIdx++;
        pAccountCfg = pAccountCfg->pNext;
    }

    LogUDiskError("DON'T find UserAccount(%s)\n", pUserAccount);

    return -1;
}/*}}}*/

int UDiskChanageUserStatus(const char* account)
{/*{{{*/
    if(!account)
        return -1;
    char wAccountField[64] = { 0 };
    snprintf(wAccountField, 63, "[%s]", account);
    int  wRcount = 0, wLength = 0;
    FILE* fAccountCfg = NULL;
    char wLocalBuffer[4 * 1024];
    char *pTmp = NULL;
    //在这里重新打开文件,目的是修改U盘中account.ini中的initialize项
    //sleep(1);
    char wAccountCfg[64] = { 0 };
    sprintf(wAccountCfg, "/mnt/usb%d/usbconfig/account.ini", UDiskGetMountNumber());
    fAccountCfg = fopen(wAccountCfg, "r+");
    if(!fAccountCfg)
        return -1;

    int flag = 0;
    memset(wLocalBuffer, 0, 4 * 1024);
    while((wRcount = fread(wLocalBuffer, sizeof(char), 4 * 1024, fAccountCfg)) > 0) {
        wLength = wRcount;
        pTmp = strstr(wLocalBuffer, wAccountField);
        if(!pTmp) {
            LogUDiskDebug("should continue to read more data.");
            memset(wLocalBuffer, 0, 4 * 1024);
            continue;
        } else {
            flag = 1;
            LogUDiskDebug("ok,find this accout in account.ini\n");
            break;
        }
    }
    if(!flag) {
        LogUDiskDebug("error,there is not ths accout in accout.ini");
        fclose(fAccountCfg);
        return -1;
    }

    if(pTmp)
        pTmp = strstr(pTmp, "initialize");
    if(!pTmp) {
        LogUDiskDebug("account.ini file error!\n");
        fclose(fAccountCfg);
        return -1;
    }

    *(pTmp + 11) = '1';
    fseek(fAccountCfg, 0 - wLength, SEEK_CUR);
    fwrite(wLocalBuffer, sizeof(char), wLength, fAccountCfg);
    fclose(fAccountCfg);
    sync();
    sleep(1);
    return 0;
}/*}}}*/

void UDiskSetUserConfigData()
{/*{{{*/
    LogUDiskDebug("set and save user config data\n");
    if(gUserCfg.nFlag == 0) {
        LogUDiskDebug("the ***.cfg file is not know\n");
        return ;
    }
    sysSettingSetInt("connecttype", gUserCfg.nConnectType, 0);
    sysNtvuserSet(gUserCfg.nNtvUserAccount, 1, 0);
    sysNtvpasswdSet(gUserCfg.nNtvUserPassword, 1, 0);
    if(gUserCfg.nConnectType == 1) {
        sysSettingSetString("netuser", gUserCfg.nNetUserAccount, 0);
        sys_passwd_set(gUserCfg.nNetUserPassword, 0);
    } else if(gUserCfg.nConnectType == 2) {
        sysSettingSetString("dhcpuser", gUserCfg.nNetUserAccount, 0);
        sys_dhcppasswd_set(gUserCfg.nNetUserPassword, 0);
    } else if(gUserCfg.nConnectType == 3) {
        sysSettingSetString("ip", gUserCfg.nStbIP, 0);
        sysSettingSetString("netmask", gUserCfg.nNetmask, 0);
        sysSettingSetString("gateway", gUserCfg.nGateway, 0);
        sys_dns_set(gUserCfg.nDns, 0, 0);
        sys_dns_set(gUserCfg.nDns2, 1, 0);
    } else if(gUserCfg.nConnectType == 4) {
        sysSettingSetString("dhcpuser", gUserCfg.nNetUserAccount, 0);
        sys_dhcppasswd_set(gUserCfg.nNetUserPassword, 0);
    }
}/*}}}*/

void UDiskSetCommonConfigData()
{/*{{{*/
    LogUDiskDebug("set and save common config data\n");
    if(gCommonCfg.nFlag != 1) {
        char wCommonCfg[64] = { 0 };
        sprintf(wCommonCfg, "/mnt/usb%d/usbconfig/common.cfg", UDiskGetMountNumber());
        if(access(wCommonCfg, R_OK | F_OK) < 0) {
            LogUDiskDebug("no this file! : COMON_FILE \n");
            return;
        }
        _ParseCommonConfigData(wCommonCfg);
    }

    if(gCommonCfg.nFlag != 1) {
        LogUDiskDebug("the common file is not init\n");
        return;
    }
    if('\0' != gCommonCfg.nEdsAddr[0])
        sys_eds_set(gCommonCfg.nEdsAddr, 0, 0);
    if('\0' != gCommonCfg.nEdsAddr2[0])
        sys_eds_set(gCommonCfg.nEdsAddr2, 1, 0);
    if('\0' != gCommonCfg.nUpgradeURL[0])
	    sysSettingSetString("upgradeUrl", gCommonCfg.nUpgradeURL, 0);
    if('\0' != gCommonCfg.nNtpServer[0])
        sys_ntp_set(gCommonCfg.nNtpServer, 0, 0);
    if('\0' != gCommonCfg.nNtpServer2[0])
        sys_ntp_set(gCommonCfg.nNtpServer2, 1, 0);
    if(gCommonCfg.nTimezone != (int)0x80000000)
        sysSettingSetInt("timezone", gCommonCfg.nTimezone, 0);
    if((gCommonCfg.nEpgOffSet_x >= 0) || (gCommonCfg.nEpgOffSet_y >= 0))
        ; //sys_plane_pos_set(gCommonCfg.nEpgOffSet_x, gCommonCfg.nEpgOffSet_y);
    if(gCommonCfg.nStandardMode != 0)
	    sysSettingSetInt("videoformat", gCommonCfg.nStandardMode, 0);
    if(gCommonCfg.nHDMode != 0)
	    sysSettingSetInt("videoformat", gCommonCfg.nHDMode, 0);
    if(gCommonCfg.nNetMode >= 0)
        sysSettingSetInt("connecttype", gCommonCfg.nNetMode, 0);
    if('\0' != gCommonCfg.nStbIP[0])
        sysSettingSetString("ip", gCommonCfg.nStbIP, 0);
    if('\0' != gCommonCfg.nNetmask[0])
        sysSettingSetString("netmask", gCommonCfg.nNetmask, 0);
    if('\0' != gCommonCfg.nGateway[0])
        sysSettingSetString("gateway", gCommonCfg.nGateway, 0);
    if('\0' != gCommonCfg.nDns[0])
        sys_dns_set(gCommonCfg.nDns, 0, 0);
    if('\0' != gCommonCfg.nDns2[0])
        sys_dns_set(gCommonCfg.nDns2, 1, 0);
#ifdef TVMS_OPEN
    if('\0' != gCommonCfg.nTvmsHeartbeatUrl[0])
        tvms_conf_tvmsheartbiturl_set(gCommonCfg.nTvmsHeartbeatUrl);
    if('\0' != gCommonCfg.nTvmsVodUrl[0])
        tvms_conf_tvmsvodheartbiturl_set(gCommonCfg.nTvmsVodUrl);
    if(gCommonCfg.nTvmsHeartbeatPeriod >= 0)
        tvms_conf_tvmsheartbitinterval_set(gCommonCfg.nTvmsHeartbeatPeriod);
    if(gCommonCfg.nTvmsHeartbeatDelay >= 0)
        tvms_conf_tvmsdelaylength_set(gCommonCfg.nTvmsHeartbeatDelay);
#endif
}/*}}}*/

} // End Hippo


