/**
 * @file LogUDiskInstall.cpp
 * @brief
 *      中国联通家庭宽带多媒体应用业务平台技术规范第五分册 -与盒端接口分册
 *      10.1 快速放装 U 盘的要求
 * @author Michael
 * @version 1.0
 * @date 2012-09-13
 */

#include "UDiskQuickInstall.h"
#include "NativeHandler.h"
#include "MessageTypes.h"
#include "OSCommon.h"
#include "OSOpenSSL.h"
#include "Hippo_api.h"
#include "app_sys.h"
extern "C" {
#include "mid_sys.h"
}
#include <unistd.h>
#include <arpa/inet.h>

static string gQuickInstallPath("");

namespace Hippo {

typedef enum _UDiskInstallEvent_ {
    eFileCanNotFound = 0x9504,
    eFileDecrypError = 0x9505,                  /* webpage not get this event */
    eFileParserError = 0x9505,
    eUInstallSuccess = 0x9506
}UDiskMsg;

/**
 * @brief UDiskOpenManageEpg open the u disk quick install management webpage
 */
inline void 
UDiskOpenManageEpg()
{/*{{{*/
    LogUDiskDebug("Open U webpage!\n");
    epgBrowserAgent().openUrl(LOCAL_WEBPAGE_PATH_PREFIX"/usbInstall_config.html");
}/*}}}*/

/**
 * @brief UDiskSendMsg send status message to epg
 *
 * @param msg message event to epg
 */
inline void 
UDiskSendMsg(UDiskMsg msg)
{/*{{{*/
    sendMessageToEPGBrowser(MessageType_Unknow, (int)msg, 0, 0);
}/*}}}*/

/**
 * @brief UDiskGetSTBID get stb serial id
 *
 * @return id 
 */
static string
UDiskGetSTBID()
{/*{{{*/
    char serial[64] = { 0 };
    mid_sys_serial(serial);
    LogUDiskDebug("stbid is %s\n", serial);
    return string(serial);
}/*}}}*/

/**
 * @brief LogUDiskInstallStart
 *
 * @return
 */
int
UDiskInstallStart(int uDiskID)
{/*{{{*/
    gQuickInstallPath = "/mnt/usb" + Hippo::OSI2S(uDiskID) + "/" + Hippo::kQuickInstallDir + "/" + Hippo::UDiskGetSTBID() + "_UNICOM_CONFIG.STB";
    if (uDiskID < 0 || uDiskID > 9 || access(gQuickInstallPath.c_str(), F_OK) < 0) {
        LogUDiskError("%d %s\n", uDiskID, gQuickInstallPath.c_str());
        UDiskSendMsg(eFileCanNotFound);
        return -1;
    }
    LogUDiskDebug("QuickInstall path [%s]\n", gQuickInstallPath.c_str());

    string lPubKeyPath("");
	if(PLATFORM_ZTE == sys_platform_get()){
	    lPubKeyPath.append("-----BEGIN PUBLIC KEY-----\n");
        lPubKeyPath.append("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAknqvsAOxiMFxRAGBjiZ9\n");
        lPubKeyPath.append("gbvxpF/Do0lZApT/6XJ+v6PGkcycPaFX+AXUEKBe1whvsfxrubeiv8klRxczt6Nx\n");
        lPubKeyPath.append("qUJ1DscAOYwQgkRyTefbR2LNvzShvxiJBjjmTOP6XUrG/qzIjk7/Bv8+zO2F3YoS\n");
        lPubKeyPath.append("jNQ4i5D5EQMv9vKfgmPLZ5/ZyhiHlyQQz4Wku84zbv5yls4afxLVSb5jgntRKVkJ\n");
        lPubKeyPath.append("QReo2MBOOE8YrfkyuByUg7Vb0pbEkDM/YcZ4BWb5jUiqoYtlw6fpWTXGfC9VvSY+\n");
        lPubKeyPath.append("WBd6CGslNV0bT5WAoF1MQubGFKWPMEygeMwu32LMHJ95wu2zLYjFRZeexBZgQ4e4\n");
        lPubKeyPath.append("sQIDAQAB\n");
        lPubKeyPath.append("-----END PUBLIC KEY-----");
	}else{
#ifdef NEIMENGGU_HD
    lPubKeyPath.append("-----BEGIN PUBLIC KEY-----\n");
    lPubKeyPath.append("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAknqvsAOxiMFxRAGBjiZ9\n");
    lPubKeyPath.append("gbvxpF/Do0lZApT/6XJ+v6PGkcycPaFX+AXUEKBe1whvsfxrubeiv8klRxczt6Nx\n");
    lPubKeyPath.append("qUJ1DscAOYwQgkRyTefbR2LNvzShvxiJBjjmTOP6XUrG/qzIjk7/Bv8+zO2F3YoS\n");
    lPubKeyPath.append("jNQ4i5D5EQMv9vKfgmPLZ5/ZyhiHlyQQz4Wku84zbv5yls4afxLVSb5jgntRKVkJ\n");
    lPubKeyPath.append("QReo2MBOOE8YrfkyuByUg7Vb0pbEkDM/YcZ4BWb5jUiqoYtlw6fpWTXGfC9VvSY+\n");
    lPubKeyPath.append("WBd6CGslNV0bT5WAoF1MQubGFKWPMEygeMwu32LMHJ95wu2zLYjFRZeexBZgQ4e4\n");
    lPubKeyPath.append("sQIDAQAB\n");
    lPubKeyPath.append("-----END PUBLIC KEY-----");
#else
    lPubKeyPath.append("-----BEGIN PUBLIC KEY-----\n");
    lPubKeyPath.append("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs6k5Wl/vl23AsIGp/6+F\n");
    lPubKeyPath.append("prVcUNw9CBgjBUyhnCMq9JSG/oNIa2vNcHT/20RNfzB18guX8a2NF56Yr84/CZPE\n");
    lPubKeyPath.append("md7O52fqXguylA0aMOb16TiMNkSw7O54PdOPNtF5++lBcsuw1FeKKsIWaZ3AnrJT\n");
    lPubKeyPath.append("B9oUIXSnwQIQ/UvVb4/Hf5LRoLpMH1q+f6MlA2/K+pnYHsUeLr4hfgXJHDh3ODgT\n");
    lPubKeyPath.append("poeoBKKM1WFauR8L2yT+C7PdCq9KHz8ySmy4rRxceqeo4hr92+ipa6Hl7XFfnfx3\n");
    lPubKeyPath.append("XgYJ3u1zlP0ECufi32E0JI4wD9PfkqysNn1mnD5TjX8RQSrkHQG70XqXSDgnFWID\n");
    lPubKeyPath.append("1QIDAQAB\n");
    lPubKeyPath.append("-----END PUBLIC KEY-----");
#endif
	}
    const string lDecRsaFile("/var/__UDiskRsaDecrypto.tmp");
    const string lDecMd5File("/var/__UDiskMd5Decrypto.tmp");
    unsigned char lBuf[sizeof(struct in6_addr)] = { 0 };
    /*-----------------------------------------------------------------------------
     *                          RSA and MD5 decryption
     *-----------------------------------------------------------------------------*/

    /* 
    const string lPubKeyPath("/home/hybroad/public.pem");
    LogUDiskDebug("determine whether public key exist.\n");
    if (access(lPubKeyPath.c_str(), R_OK | F_OK) < 0) {
        UDiskSendMsg(eFileDecrypError);
        LogUDiskError("can not find public key.\n");
        return -1;
    }
    */

    LogUDiskDebug("start rsa decrypto\n");
    OpenSSL::RSACrypto lRsa(OpenSSL::RSACrypto::ePRIEncrypt, OpenSSL::RSACrypto::eRsaNO);
    // lRsa.SetPubKey(lPubKeyPath, OpenSSL::RSACrypto::eKeyInFile);
    lRsa.SetPubKey(lPubKeyPath, OpenSSL::RSACrypto::eKeyInMem);
    if (lRsa.DecryptoFile(gQuickInstallPath, lDecRsaFile) < 0) {
        UDiskSendMsg(eFileDecrypError);
        remove(lDecRsaFile.c_str());
        LogUDiskError("decrypto.\n");
        return -1;
    }

    LogUDiskDebug("md5 check:\n");
    OpenSSL::MD5Crypto lMd5(kMD5Link[0], kMD5Link[1], kMD5Link[2], kMD5Link[3]);
    string lDigest1 = lMd5.ObtainDigest(lDecRsaFile, OpenSSL::MD5Crypto::eHead);
    lMd5.RemoveDigest(lDecRsaFile, lDecMd5File, OpenSSL::MD5Crypto::eHead);
    string lDigest2 = lMd5.CreateDigest(lDecMd5File);
    if (lDigest1.empty() || lDigest2.empty() || lDigest1 != lDigest2) {
        UDiskSendMsg(eFileDecrypError);
        remove(lDecRsaFile.c_str());
        remove(lDecMd5File.c_str());
        LogUDiskError("digest1[%s] != digest2[%s]\n", lDigest1.c_str(), lDigest2.c_str());
        return -1;
    }

    /*-----------------------------------------------------------------------------
     *                          Parser configure file (name=value)
     *-----------------------------------------------------------------------------*/
    LogUDiskDebug("parser configure file:\n");
    UDisk::QuickInstallData lCfgData;
    const string kFixField("begin");
    ConfigFileParser lCfgParser;
    /* Open configure file */
    if (lCfgParser.fileOpen(lDecMd5File) < 0) {
        UDiskSendMsg(eFileParserError);
        LogUDiskError("open and store data using map error.");
        goto Err;
    }
    /* STBID: Not set, but use it to verify whether the file match the STB */
    if (UDiskGetSTBID() != lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.STBID")) {
        UDiskSendMsg(eFileParserError);
        LogUDiskError("stbid not match[%s] vs [%s].\n", UDiskGetSTBID().c_str(), lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.STBID").c_str());
        goto Err;
    }

    /* get AuthServiceInfo */
    lCfgData.mAuthServiceInfo.mPPPOEEnable     = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.PPPOEEnable");
    lCfgData.mAuthServiceInfo.mPPPOEID         = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.PPPOEID");
    lCfgData.mAuthServiceInfo.mPPPOEPassword   = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.PPPOEPassword");
    lCfgData.mAuthServiceInfo.mPPPOEID2        = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.PPPOEID2");
    lCfgData.mAuthServiceInfo.mPPPOEPassword2  = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.PPPOEPassword2");
    lCfgData.mAuthServiceInfo.mUserID          = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.UserID");
    lCfgData.mAuthServiceInfo.mUserIDPassword  = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.UserIDPassword");
    lCfgData.mAuthServiceInfo.mUserID2         = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.UserID2");
    lCfgData.mAuthServiceInfo.mUserIDPassword2 = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.AuthServiceInfo.UserIDPassword2");

    /* get StbInfo */
    lCfgData.mStbInfo.mSTBID         = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.STBID");
    lCfgData.mStbInfo.mOperatorInfo  = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.OperatorInfo");
    lCfgData.mStbInfo.mUserPassword  = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.UserPassword");
    lCfgData.mStbInfo.mUpgradeURL    = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.UpgradeURL");
    lCfgData.mStbInfo.mBrowserURL1   = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.BrowserURL1");
    lCfgData.mStbInfo.mBrowserURL2   = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.BrowserURL2");
    lCfgData.mStbInfo.mUserProvince  = lCfgParser.GetVarStr(kFixField, "Device.X_CU_STB.STBInfo.UserProvince");

    /* get ManagementServer */
    lCfgData.mManagementServer.mManagementServerURL       = lCfgParser.GetVarStr(kFixField, "Device.ManagementServer.URL");
    lCfgData.mManagementServer.mSTUNServerAddress         = lCfgParser.GetVarStr(kFixField, "Device.ManagementServer.STUNServerAddress");
    lCfgData.mManagementServer.mSTUNServerPort            = lCfgParser.GetVarStr(kFixField, "Device.ManagementServer.STUNServerPort");
    lCfgData.mManagementServer.mConnectionRequestUsername = lCfgParser.GetVarStr(kFixField, "Device.ManagementServer.ConnectionRequestUsername");
    lCfgData.mManagementServer.mConnectionRequestPassword = lCfgParser.GetVarStr(kFixField, "Device.ManagementServer.ConnectionRequestPassword");

    /* get Lan Link info */
    lCfgData.mLanLinkInfo.mAddressingType = lCfgParser.GetVarStr(kFixField, "Device.LAN.AddressingType");
    lCfgData.mLanLinkInfo.mIPAddress      = lCfgParser.GetVarStr(kFixField, "Device.LAN.IPAddress");
    lCfgData.mLanLinkInfo.mSubnetMask     = lCfgParser.GetVarStr(kFixField, "Device.LAN.SubnetMask");
    lCfgData.mLanLinkInfo.mDefaultGateway = lCfgParser.GetVarStr(kFixField, "Device.LAN.DefaultGateway");
    lCfgData.mLanLinkInfo.mDNSServers     = lCfgParser.GetVarStr(kFixField, "Device.LAN.DNSServers");
    lCfgData.mLanLinkInfo.mDNSServers2    = lCfgParser.GetVarStr(kFixField, "Device.LAN.DNSServers2");

    /* get Other info */
    lCfgData.mNTPServer  = lCfgParser.GetVarStr(kFixField, "NTPServer");
    lCfgData.mNTPServer2 = lCfgParser.GetVarStr(kFixField, "NTPServer2");

    /*-----------------------------------------------------------------------------
     *                          System configure to STB
     *-----------------------------------------------------------------------------*/
    LogUDiskDebug("system setting\n");
    /* pppoe enable */
    if (!lCfgData.mAuthServiceInfo.mPPPOEEnable.empty()) {
        if (lCfgData.mAuthServiceInfo.mPPPOEEnable != "0" && lCfgData.mAuthServiceInfo.mPPPOEEnable != "1")
            LogUDiskErrorOut("Device.X_CU_STB.AuthServiceInfo.PPPOEEnable is null or not 0 or 1\n");
        else 
            UDisk::DataPPPOEEnableSet(lCfgData.mAuthServiceInfo.mPPPOEEnable.c_str());
    }
    /* pppoe account */
    if (!lCfgData.mAuthServiceInfo.mPPPOEID.empty())
        UDisk::DataPPPOEIDSet(lCfgData.mAuthServiceInfo.mPPPOEID.c_str());
    /* pppoe password */
    if (!lCfgData.mAuthServiceInfo.mPPPOEPassword.empty())
        UDisk::DataPPPOEPasswordSet(lCfgData.mAuthServiceInfo.mPPPOEPassword.c_str());
    /* stb user ID */
    if (!lCfgData.mAuthServiceInfo.mUserID.empty())
        UDisk::DataUseIDSet(lCfgData.mAuthServiceInfo.mUserID.c_str());
    /* stb user password */
    if (!lCfgData.mAuthServiceInfo.mUserIDPassword.empty())
        UDisk::DataUserIDPasswordSet(lCfgData.mAuthServiceInfo.mUserIDPassword.c_str());
    /* upgrade url */
    if (!lCfgData.mStbInfo.mUpgradeURL.empty()) { 
        if (lCfgData.mStbInfo.mUpgradeURL.size() < 8)
            LogUDiskErrorOut("Device.X_CU_STB.STBInfo.UpgradeURL is null or invalid(http://)\n");
        else 
            UDisk::DataUpgradeURLSet(lCfgData.mStbInfo.mUpgradeURL.c_str());
    }
    /* eds url */
    if (!lCfgData.mStbInfo.mBrowserURL1.empty()) {
        if (lCfgData.mStbInfo.mBrowserURL1.size() < 8)
            LogUDiskErrorOut("Device.X_CU_STB.STBInfo.BrowserURL1 is null or invalid(http://)\n");
        else 
            UDisk::DataBrowserURLSet(lCfgData.mStbInfo.mBrowserURL1.c_str());
    }
    /* eds2 url */
    if (!lCfgData.mStbInfo.mBrowserURL2.empty()) {
        if (lCfgData.mStbInfo.mBrowserURL2.size() < 8)
            LogUDiskErrorOut("Device.X_CU_STB.STBInfo.BrowserURL2 is null or invalid(http://)\n");
        else 
            UDisk::DataBrowserURL2Set(lCfgData.mStbInfo.mBrowserURL2.c_str());
    }
    /* province */
    if (!lCfgData.mStbInfo.mUserProvince.empty()) {
        if (lCfgData.mStbInfo.mUserProvince.size() > 3)
            LogUDiskErrorOut("Device.X_CU_STB.STBInfo.UserProvince is null or invalid(3bytes)\n");
    }

    /* tr069 url */
#ifdef INCLUDE_TR069
    UDisk::DataManagementServerURL((char*)lCfgData.mManagementServer.mManagementServerURL.c_str());
    if (!lCfgData.mManagementServer.mManagementServerURL.empty()) {
        if (lCfgData.mManagementServer.mManagementServerURL.size() < 8)
            LogUDiskErrorOut("Device.ManagementServer.URL is null or invalid\n");
        else 
            UDisk::DataManagementServerURL((char*)lCfgData.mManagementServer.mManagementServerURL.c_str());
    }
    /* tr069 IP */
    if (!lCfgData.mManagementServer.mSTUNServerAddress.empty()) {
        if (inet_pton(AF_INET, lCfgData.mManagementServer.mSTUNServerAddress.c_str(), lBuf) <= 0 
            && inet_pton(AF_INET6, lCfgData.mManagementServer.mSTUNServerAddress.c_str(), lBuf) <= 0)
            LogUDiskErrorOut("Device.ManagementServer.STUNServerAddress is null or invalid\n");
    }
    /* tr069 Port */
    if (!lCfgData.mManagementServer.mSTUNServerPort.empty()) {
        if (atoi(lCfgData.mManagementServer.mSTUNServerPort.c_str()) > 65535)
            LogUDiskErrorOut("Device.ManagementServer.STUNServerPort is null or invalid(0 - 65535)\n");
    }

    /* tr069 connection request username */
    if (!lCfgData.mManagementServer.mConnectionRequestUsername.empty())
        UDisk::DataConnectionRequestUsername((char*)lCfgData.mManagementServer.mConnectionRequestUsername.c_str());
    /* tr069 connection request password */
    if (!lCfgData.mManagementServer.mConnectionRequestPassword.empty())
        UDisk::DataConnectionRequestPassword((char*)lCfgData.mManagementServer.mConnectionRequestPassword.c_str());
#endif
    /* network connection type */
    if (!lCfgData.mLanLinkInfo.mAddressingType.empty() && lCfgData.mAuthServiceInfo.mPPPOEEnable == "0")
        UDisk::DataAddressingTypeSet(lCfgData.mLanLinkInfo.mAddressingType.c_str());
    /* lan info */
    if ((lCfgData.mLanLinkInfo.mAddressingType == "static" || lCfgData.mLanLinkInfo.mAddressingType == "Static") && lCfgData.mAuthServiceInfo.mPPPOEEnable == "0") {
        /* ip */
        if (!lCfgData.mLanLinkInfo.mIPAddress.empty()) {
            if (inet_pton(AF_INET, lCfgData.mLanLinkInfo.mIPAddress.c_str(), lBuf) <=0 
                    && inet_pton(AF_INET6, lCfgData.mLanLinkInfo.mIPAddress.c_str(), lBuf) <=0)
                LogUDiskErrorOut("Device.LAN.IPAddress is null or invalid\n");
            else 
                UDisk::DataIPAddressSet(lCfgData.mLanLinkInfo.mIPAddress.c_str());
        }
        /* mask */
        if (!lCfgData.mLanLinkInfo.mSubnetMask.empty()) {
            if (inet_pton(AF_INET, lCfgData.mLanLinkInfo.mSubnetMask.c_str(), lBuf) <=0 
                && inet_pton(AF_INET6, lCfgData.mLanLinkInfo.mSubnetMask.c_str(), lBuf) <=0)
                LogUDiskErrorOut("Device.LAN.SubnetMask is null or invalid\n");
            else 
                UDisk::DataSubnetMaskSet(lCfgData.mLanLinkInfo.mSubnetMask.c_str());
        }
        /* gateway */
        if (!lCfgData.mLanLinkInfo.mDefaultGateway.empty()) {
            if (inet_pton(AF_INET, lCfgData.mLanLinkInfo.mDefaultGateway.c_str(), lBuf) <=0 
                && inet_pton(AF_INET6, lCfgData.mLanLinkInfo.mDefaultGateway.c_str(), lBuf) <=0)
                LogUDiskErrorOut("Device.LAN.DefaultGateway is null or invalid\n");
            else 
                UDisk::DataDefaultGatewaySet(lCfgData.mLanLinkInfo.mDefaultGateway.c_str());
        }
        /* dns */
        if (!lCfgData.mLanLinkInfo.mDNSServers.empty()) {
            if (inet_pton(AF_INET, lCfgData.mLanLinkInfo.mDNSServers.c_str(), lBuf) <=0 
                && inet_pton(AF_INET6, lCfgData.mLanLinkInfo.mDNSServers.c_str(), lBuf) <=0)
                LogUDiskErrorOut("Device.LAN.DNSServers is null or invalid\n");
            else 
                UDisk::DataDNSServersSet(lCfgData.mLanLinkInfo.mDNSServers.c_str());
        }
        /* dns2 */
        if (!lCfgData.mLanLinkInfo.mDNSServers2.empty()) {
            if (inet_pton(AF_INET, lCfgData.mLanLinkInfo.mDNSServers2.c_str(), lBuf) <=0 
                && inet_pton(AF_INET6, lCfgData.mLanLinkInfo.mDNSServers2.c_str(), lBuf) <=0)
                LogUDiskErrorOut("Device.LAN.DNSServers2 is null or invalid\n");
            else 
                UDisk::DataDNSServers2Set(lCfgData.mLanLinkInfo.mDNSServers2.c_str());
        }
    }
    /* ntp */
    if (!lCfgData.mNTPServer.empty()) {
        if (inet_pton(AF_INET, lCfgData.mNTPServer.c_str(), lBuf) <=0 
            && inet_pton(AF_INET6, lCfgData.mNTPServer.c_str(), lBuf) <=0)
            LogUDiskErrorOut("NTPServer is null or invalid\n");
        else 
            UDisk::DataNTPServerSet(lCfgData.mNTPServer.c_str());
    }
    /* ntp2 */
    if (!lCfgData.mNTPServer2.empty()) {
        if (inet_pton(AF_INET, lCfgData.mNTPServer2.c_str(), lBuf) <=0 
            && inet_pton(AF_INET6, lCfgData.mNTPServer2.c_str(), lBuf) <=0)
            LogUDiskErrorOut("NTPServer2 is null or invalid\n");
        else 
            UDisk::DataNTPServer2Set(lCfgData.mNTPServer2.c_str());
    }

    UDisk::DataSaveToFlash();
    remove(lDecRsaFile.c_str());
    remove(lDecMd5File.c_str());
    UDiskSendMsg(eUInstallSuccess);
    LogUDiskDebug("UdiskQuickInstall is successfull!\n");
    return 0;
Err:
    remove(lDecRsaFile.c_str());
    remove(lDecMd5File.c_str());
    UDiskSendMsg(eFileParserError);
    LogUDiskError("UdiskQuickInstall is fail!\n");
    return -1;
}/*}}}*/

/**
 * @brief LogUDiskInstallDetect
 *
 * @param uDiskID
 * @param stbID
 *
 * @return
 */
int 
UDiskQuickInstallDetect(int uDiskID)
{/*{{{*/
    string lDiskDir = "/mnt/usb" + Hippo::OSI2S(uDiskID) + "/" + Hippo::kQuickInstallDir;
    if (access(lDiskDir.c_str(), R_OK | F_OK) < 0) 
        return -1;
    Hippo::UDiskOpenManageEpg(); /* Open the administrator login webpage */
    return 0;
}/*}}}*/

} //End Hippo
