#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "KeepingModifyUtilitys.h"
 

using namespace std; 

extern "C" {
ParametersItem ERROR_PARAM_ITEM = {"string error", "string error", "string error", "string error", "string error", "string error"};
std::string gMonthStr[13] = {"Undefined", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
}

void getDateTimeStamp(DateTimeStamp* dt)
{
    settingsLogVerbose("GetDateTime.\n");
    if (dt) {
        struct timeval current;
        struct tm temp_time;
        
        if (!gettimeofday(&current, NULL)){
            localtime_r(&current.tv_sec, &temp_time);
            dt->mYear       = temp_time.tm_year + 1900;
            dt->mMonth      = temp_time.tm_mon + 1;
            dt->mDayOfWeek  = temp_time.tm_wday;
            dt->mDay        = temp_time.tm_mday;
            dt->mHour       = temp_time.tm_hour;
            dt->mMinute     = temp_time.tm_min;
            dt->mSecond     = temp_time.tm_sec;
	    }
    }
    
    return ;
}



/**
 *  @Func: dateTimeCompare
 *  @Param: strBaseDTime, type:: std::string&
 *          strNowDTime, type:: std::string&
 *  @Return: int
 *  
 *  e.g: <2012-Jan-01 08:02:05.521>  <2012-Jan-06 08:04:50.147>.
 **/
int dateTimeCompare(std::string& strBaseDTime, std::string& strNowDTime)  //(const char* dtime1, const char* dtime2)  //
{
    if(strBaseDTime.empty() || strNowDTime.empty()){
         settingsLogError("strBaseDTime is NULL.\n");
        return -1;
    }
    int i = 0;
    std::string  strBaseTime(""), strBaseDate(""), strBaseYear(""), strBaseMonth(""), strBaseDay("");
    std::string  strNowTime(""), strNowDate(""), strNowYear(""), strNowMonth(""), strNowDay("");
    std::string strBaseSubDate("");
    std::string::size_type pos1, pos2;
    std::vector<std::string> vectorMonth; 
    
    settingsLogVerbose("strBaseDTime[%s] strNowDTime[%s].\n", strBaseDTime.c_str(), strNowDTime.c_str());
    for(i = 0; i < 13; i++){
        vectorMonth.push_back(gMonthStr[i]);
    }
    
    //get Date && Time  
    pos1 = strBaseDTime.find(" ");
    strBaseDate = strBaseDTime.substr(0, pos1);
    strBaseTime = strBaseDTime.substr(pos1 + 1, strBaseDTime.length());
    pos1 = strBaseDate.find("-");
    strBaseYear = strBaseDate.substr(0, pos1);  
    strBaseSubDate = strBaseDate.substr(pos1 + 1, strBaseDate.length());
    pos1 = strBaseSubDate.find("-");
    strBaseMonth = strBaseSubDate.substr(0, pos1);     
    strBaseDay = strBaseSubDate.substr(pos1 + 1, strBaseSubDate.length());
    
    pos2 = strNowDTime.find(" ");
    strNowDate = strNowDTime.substr(0, pos2);
    strNowTime = strNowDTime.substr(pos2 + 1, strNowDTime.length());
    pos2 = strNowDate.find("-");
    strNowYear = strNowDate.substr(0, pos2);  
    strBaseSubDate = strNowDate.substr(pos2 + 1, strNowDate.length());
    pos2 = strBaseSubDate.find("-");
    strNowMonth = strBaseSubDate.substr(0, pos2);     
    strNowDay = strBaseSubDate.substr(pos2 + 1, strBaseSubDate.length());

    //compare Date && Time
    if(strBaseYear.compare(strNowYear) == 0){       //year
        if(std::find(vectorMonth.begin(), vectorMonth.end(), strBaseMonth) == std::find(vectorMonth.begin(), vectorMonth.end(), strNowMonth)){       //month
            if(strBaseDay.compare(strNowDay) == 0){       //day
                if(strBaseTime.compare(strNowTime) == 0){       //time
                    settingsLogVerbose("strBaseDTime[%s] == strNowDTime[%s].\n", strBaseDTime.c_str(), strNowDTime.c_str());
                    return 0;
                } else if(strBaseTime.compare(strNowTime) > 0){ //time
                    settingsLogVerbose("strBaseTime[%s] > strNowTime[%s].\n", strBaseTime.c_str(), strNowTime.c_str());
                    return 1;
                } else {                                        //time
                    settingsLogVerbose("strBaseTime[%s] < strNowTime[%s].\n", strBaseTime.c_str(), strNowTime.c_str());
                    return -1;
                }
            } else if(strBaseDay.compare(strNowDay) > 0){ //day
                settingsLogVerbose("strBaseDay[%s] > strNowDay[%s].\n", strBaseDay.c_str(), strNowDay.c_str());
                return 1;
            } else {                                      //day
                settingsLogVerbose("strBaseDay[%s] < strNowDay[%s].\n", strBaseDay.c_str(), strNowDay.c_str());
                return -1;
            }
        } else if(std::find(vectorMonth.begin(), vectorMonth.end(), strBaseMonth) > std::find(vectorMonth.begin(), vectorMonth.end(), strNowMonth)){ //month
            settingsLogVerbose("strBaseMonth[%s] > strNowMonth[%s].\n", strBaseMonth.c_str(), strNowMonth.c_str());
            return 1;
        } else {                                                                                                                                     //month
            settingsLogVerbose("strBaseMonth[%s] < strNowMonth[%s].\n", strBaseMonth.c_str(), strNowMonth.c_str());
            return -1;
        }
    } else if(strBaseYear.compare(strNowYear) > 0){ //year
        settingsLogVerbose("strBaseYear[%s] < strNowYear[%s].\n", strBaseYear.c_str(), strNowYear.c_str());
        return 1;
    } else { //(strBaseYear.compare(strNowDate) < 0)//year
        settingsLogVerbose("strBaseYear[%s] < strNowYear[%s].\n", strBaseYear.c_str(), strNowYear.c_str());
        return -1;
    } 
    settingsLogVerbose("strBaseDTime[%s] strNowDTime[%s], ERROR.\n", strBaseDTime.c_str(), strNowDTime.c_str());
    
    return -2;
}



/**
 *  @Func: getValueByTag
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: str, type:: std::string&
 *          tag, type:: const char *
 *  @Return: std::string
 *  
 *  e.g: <2012-Jan-01 08:02:05.521>
 **/
std::string getValueByTag(std::string& str, const char *tag)   
{
    std::string::size_type position;
    std::string value("");
    std::string strTag = tag;
    
    position = str.find(strTag);
    if (position == std::string::npos) {
        settingsLogError("Str[%s] cannot find strTag[%s], ERROR.\n", str.c_str(), strTag.c_str());
        return "fail";
    }
    value = str.substr(0, position);
    str = str.substr(std::string(strTag).size() + value.size(), str.size());
    
    return value;
}  


/**
 *  @Func: parseRecordLine
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: strLine, type:: std::string&
 *  @Return: ParametersItem*, a structure;
 *  
 *  e.g: Parameter[ hd_aspect_mode ] @<2012-Jan-01 08:02:05.481> modified by[ JS ]from[ 2 ]to[ 0 ]into file[ /root/yx_config_customer.ini ].
 **/
ParametersItem* parseRecordLine(std::string& strLine)
{
    ParametersItem  *paramItem = new ParametersItem();
    
    if(strLine.find("Parameter[") == std::string::npos){
        settingsLogError("parseRecordLine strLine[%s]\n", strLine.c_str());
        return &ERROR_PARAM_ITEM;
    }
    
    strLine = strLine.substr(std::string("Parameter[ ").size(), strLine.size());
    paramItem->paramName = getValueByTag(strLine, " ] @<");
    if (paramItem->paramName == "fail")
        return &ERROR_PARAM_ITEM;
    paramItem->modifyTimeStamp = getValueByTag(strLine, "> modified by[ ");
    if (paramItem->modifyTimeStamp == "fail")
        return &ERROR_PARAM_ITEM;
    paramItem->sourceModule = getValueByTag(strLine, " ]from[ ");
    if (paramItem->sourceModule == "fail")
        return &ERROR_PARAM_ITEM;
    paramItem->oldValue = getValueByTag(strLine, " ]to[ ");
    if (paramItem->oldValue == "fail")
        return &ERROR_PARAM_ITEM;
    paramItem->newValue = getValueByTag(strLine, " ]into file[ ");
    if (paramItem->newValue == "fail")
        return &ERROR_PARAM_ITEM;
    paramItem->modifiedFile = getValueByTag(strLine, " ].");
    if (paramItem->modifiedFile == "fail")
        return &ERROR_PARAM_ITEM;

/*     settingsLogVerbose("parseRecordLine paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
                    (paramItem->paramName).c_str(), (paramItem->newValue).c_str(), (paramItem->sourceModule).c_str(), 
                    (paramItem->oldValue).c_str(), (paramItem->modifyTimeStamp).c_str(), (paramItem->modifiedFile).c_str()
                    );
 */
    return paramItem;
}




 
 
 
 














