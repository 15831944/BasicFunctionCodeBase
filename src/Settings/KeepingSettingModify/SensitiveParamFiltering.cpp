#include <iostream>

#include <stdio.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"
#include "SensitiveParamFiltering.h"
 
using namespace std;

extern "C" {
SensitiveParamFiltering gSensitiveParamFilter;
}

static std::string gSensitiveListStr[] = {"ntvpasswd", "ntvAESpasswd", "defContPwd", "defAESContpwd", 
                                        "netpasswd", "syncntvpasswd", "syncntvAESpasswd", "netAESpasswd", "dhcppasswd", "ipoeAESpasswd", "wifi_password", 
                                        "authbg_md5", "bootlogo_md5", 
                                        "Device.ManagementServer.Password", 
                                        "Device.ManagementServer.ConnectionRequestPassword", 
                                        "Device.ManagementServer.STUNPassword"
                                        };
                                        
SensitiveParamFiltering::SensitiveParamFiltering()
{
    settingsLogVerbose("\n");
    int i = 0;
    for(i = 0; i < 16; i++){
        //m_sensitiveParamFilter.push_back(gSensitiveListStr[i]);
    }    
}


/**
 *  @Func: filteringSensitiveParam
 *  @Param: param, type:: std::string
 *  @Return: bool
 *  
 **/
bool SensitiveParamFiltering::filteringSensitiveParam(std::string param)
{
    std::vector<std::string>::iterator it;
    
    for (it = m_sensitiveParamFilter.begin(); it != m_sensitiveParamFilter.end(); ++it) {
        if ((*it).compare(param) == 0)
            return true;
    }
    
    return false;
}


/**
 *  @Func: addSensitiveParam
 *  @Param: param, type:: std::string
 *  @Return: bool
 *  
 **/
bool SensitiveParamFiltering::addSensitiveParam(std::string& param)
{
    if(param.empty()){
        settingsLogError("param[%s] cannot empty, ERROR.\n", param.c_str());
        return false;
    } 
    m_sensitiveParamFilter.push_back(param);
    
    return true;
}


/**
 *  @Func: sensitiveParamFilterOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int SensitiveParamFiltering::sensitiveParamFilterOutput()
{
    std::vector<std::string>::iterator it;
    
    for (it = m_sensitiveParamFilter.begin(); it != m_sensitiveParamFilter.end(); ++it) 
        cout << "sensitiveParamFilterOutput:: key: " << *it <<endl;

    return   0;
}

 
 
 
 
 
 
 
 
 
 














