#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>


#include "SysTime.h"
#include "telecom_config.h"
#include "Assertions.h"
#include "ParamChangeRecordKeep.h"
 
 
#define     PARAM_CHANGE_RECORD_KEEPING_VERSION     "1.0"
#define     PARAM_CHANGE_RECORD_KEEPING_FILE        CONFIG_FILE_DIR"/config_param_modify.record"

 
namespace Hippo {
    
using namespace std; 

static SettingModifyRecordKeeping g_settingModifyRecordKeeping(PARAM_CHANGE_RECORD_KEEPING_FILE);
static std::string gMonthStr[] = {"Undefined", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

SensitiveParamFiltering gSensitiveParamFilter;

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
         LogUserOperError("strBaseDTime is NULL.\n");
        return -1;
    }
    int i = 0;
    std::string  strBaseTime(""), strBaseDate(""), strBaseYear(""), strBaseMonth(""), strBaseDay("");
    std::string  strNowTime(""), strNowDate(""), strNowYear(""), strNowMonth(""), strNowDay("");
    std::string strBaseSubDate("");
    std::string::size_type pos1, pos2;
    std::vector<std::string> vectorMonth; 
    
    LogUserOperDebug("strBaseDTime[%s] strNowDTime[%s].\n", strBaseDTime.c_str(), strNowDTime.c_str());
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
                    LogUserOperDebug("strBaseDTime[%s] == strNowDTime[%s].\n", strBaseDTime.c_str(), strNowDTime.c_str());
                    return 0;
                } else if(strBaseTime.compare(strNowTime) > 0){ //time
                    LogUserOperDebug("strBaseTime[%s] > strNowTime[%s].\n", strBaseTime.c_str(), strNowTime.c_str());
                    return 1;
                } else {                                        //time
                    LogUserOperDebug("strBaseTime[%s] < strNowTime[%s].\n", strBaseTime.c_str(), strNowTime.c_str());
                    return -1;
                }
            } else if(strBaseDay.compare(strNowDay) > 0){ //day
                LogUserOperDebug("strBaseDay[%s] > strNowDay[%s].\n", strBaseDay.c_str(), strNowDay.c_str());
                return 1;
            } else {                                      //day
                LogUserOperDebug("strBaseDay[%s] < strNowDay[%s].\n", strBaseDay.c_str(), strNowDay.c_str());
                return -1;
            }
        } else if(std::find(vectorMonth.begin(), vectorMonth.end(), strBaseMonth) > std::find(vectorMonth.begin(), vectorMonth.end(), strNowMonth)){ //month
            LogUserOperDebug("strBaseMonth[%s] > strNowMonth[%s].\n", strBaseMonth.c_str(), strNowMonth.c_str());
            return 1;
        } else {                                                                                                                                     //month
            LogUserOperDebug("strBaseMonth[%s] < strNowMonth[%s].\n", strBaseMonth.c_str(), strNowMonth.c_str());
            return -1;
        }
    } else if(strBaseYear.compare(strNowYear) > 0){ //year
        LogUserOperDebug("strBaseYear[%s] < strNowYear[%s].\n", strBaseYear.c_str(), strNowYear.c_str());
        return 1;
    } else { //(strBaseYear.compare(strNowDate) < 0)//year
        LogUserOperDebug("strBaseYear[%s] < strNowYear[%s].\n", strBaseYear.c_str(), strNowYear.c_str());
        return -1;
    } 
    
    LogUserOperDebug("strBaseDTime[%s] strNowDTime[%s], ERROR.\n", strBaseDTime.c_str(), strNowDTime.c_str());
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
        LogUserOperError("Str[%s] cannot find strTag[%s], ERROR.\n", str.c_str(), strTag.c_str());
        return "fail";
    }
    value = str.substr(0, position);
    str = str.substr(std::string(strTag).size() + value.size(), str.size());
    
    return value;
}  

#define     STRING_ERROR    "string error"
ParametersItem ERROR_PARAM_ITEM = {"string error", "string error", "string error", "string error", "string error", "string error"};

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
        LogUserOperError("parseRecordLine strLine[%s]\n", strLine.c_str());
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

/*     LogUserOperDebug("parseRecordLine paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
                    (paramItem->paramName).c_str(), (paramItem->newValue).c_str(), (paramItem->sourceModule).c_str(), 
                    (paramItem->oldValue).c_str(), (paramItem->modifyTimeStamp).c_str(), (paramItem->modifiedFile).c_str()
                    );
 */
    return paramItem;
}





//==============================================================================================================================
//==============================================================================================================================
#define STATISTAC_RECORD_COUNT_FLAG_ADD         1
#define STATISTAC_RECORD_COUNT_FLAG_MINUS       0

static int gIsDeleteFlag = 0;


ParamStatisticsMapping::ParamStatisticsMapping(std::string fileName)
    : m_filePath(fileName)
{
    m_RecordFileAvailable = 0;
    LogUserOperDebug(" initial.\n");
    
}


/**
 *  @Func: loadStatisticsRecord
 *          ps.: 逐行从 config_param_modify.record 文件加载到 m_paramStatisticsMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamStatisticsMapping::loadStatisticsRecord()
{
    FILE *fp = NULL;
    char ch;
    std::string line("");
    
    LogUserOperDebug("\n");
    std::ifstream fin(m_filePath.c_str(), std::ios::in);
    while(std::getline(fin, line)) {
        if (!m_RecordFileAvailable) {   //清除第一行
            m_RecordFileAvailable = 1;
            std::string::size_type position = line.find('=');
            if (position != std::string::npos) {
                LogUserOperDebug("Will jump the line[%s].\n", line.c_str());
                line.clear();
                continue;
            }
            LogSafeOperWarn("The available information is not found.(%s)\n", line.c_str());
            break;
        }
        LogUserOperDebug("Start parse the line[%s].\n", line.c_str());
        updateStatisticsRecord(parseRecordLine(line));   //逐行添加
        line.clear();
    }
    LogUserOperDebug("Load \"%s\" ok.\n", m_filePath.c_str());
    
    return 0;
}



/**
 *  @Func: updateStatisticsRecord
 *         ps. :将逐行解析到的 添加到 m_paramStatisticsMap
 *  @Param: paramItem, type:: ParametersItem *
 *  @Return: int
 *  
 **/
int ParamStatisticsMapping::updateStatisticsRecord(ParametersItem *paramItem)
{
    ParamStatistics *paramStatistic = new ParamStatistics();
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(paramItem->paramName); 
    
    if( (paramItem->paramName).compare(ERROR_PARAM_ITEM.paramName) == 0 
        || (paramItem->paramName).empty() || (paramItem->modifyTimeStamp).empty()){
        LogUserOperError("paramItem[%p], paramName[%s] modifyTimeStamp[%s].\n", paramItem, paramItem->paramName.c_str(), paramItem->modifyTimeStamp.c_str());
        return -1;
    }
    paramStatistic->paramName = paramItem->paramName;
    paramStatistic->earliestTimeStamp = paramItem->modifyTimeStamp;
    LogUserOperDebug("INPUT: paramName[%s] modifyTimeStamp[%s].\n", paramStatistic->paramName.c_str(), paramStatistic->earliestTimeStamp.c_str());

    //判断字段是否在统计列表 m_paramStatisticsMap
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到        
        LogUserOperDebug("ISFIND: recordCount[%d], earliestTimeStamp[%s]\n", (paramStatisticIt->second).recordCount, (paramStatisticIt->second).earliestTimeStamp.c_str());
        if(dateTimeCompare((paramStatisticIt->second).earliestTimeStamp,  paramStatistic->earliestTimeStamp) > 0){  //时间早于m_paramStatisticsMap中的时间戳   
            paramStatistic->recordCount = (paramStatisticIt->second).recordCount;
            m_paramStatisticsMap[paramStatistic->paramName] = *paramStatistic;
        } 
    } else { //未找到
        paramStatistic->recordCount = 0;
        m_paramStatisticsMap[paramStatistic->paramName] = *paramStatistic;
        LogUserOperError("\n");
    }
    modifyStatisticsRecordCount(paramStatistic->paramName, STATISTAC_RECORD_COUNT_FLAG_ADD, 1);
    LogUserOperDebug("Has update OK.\n");
    
/*     LogUserOperDebug("updateStatisticsRecord paramStatistic->paramName[%s] paramStatistic->earliestTimeStamp[%s] paramStatistic->recordCount[%d].\n", 
                    (paramStatistic->paramName).c_str(), (paramStatistic->earliestTimeStamp).c_str(), paramStatistic->recordCount
                    );
 */                    
    return 0;
}

/**
 *  @Func: unloadStatisticsRecord
 *  @Param: 
 *  @Return: std::string
 *  
 **/
int ParamStatisticsMapping::unloadStatisticsRecord()
{
    if(m_paramStatisticsMap.empty()){
        LogUserOperError("unloadStatisticsRecord:: m_paramStatisticsMap has been empty, ERROR.\n");
        return -1;
    }
    m_paramStatisticsMap.erase(m_paramStatisticsMap.begin(), m_paramStatisticsMap.end());
    LogUserOperDebug("Has unload OK.\n");
    
    return 0;
}

/**
 *  @Func: modifyStatisticsRecordTimeStamps
 *         ps. :从文件 m_fileName 的每一行中得到所需的字段值
 *  @Param: name, type:: std::string&
 *          value, type:: std::string&
 *  @Return: bool
 *  
 **/
bool ParamStatisticsMapping::modifyStatisticsRecordTimeStamps(std::string& name, std::string& timeStamp)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 
        || name.empty() || timeStamp.empty()){
        LogUserOperError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str(), timeStamp.c_str());
        return false;
    }
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 

    LogUserOperDebug("SET: name[%s], TimeStamp[%s]\n", name.c_str(), timeStamp.c_str());
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        LogUserOperDebug("HASFIND: earliestTimeStamp[%s]\n", (paramStatisticIt->second).earliestTimeStamp.c_str());
        m_paramStatisticsMap[name].earliestTimeStamp = timeStamp;
    } else { //未找到
        LogUserOperDebug("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return false;
    } 
    LogUserOperDebug("Has modify TimeStamps OK.\n");
    
    return true;
}

/**
 *  @Func: getStatisticsRecordTimeStamps
 *  @Param: name, type:: std::string&
 *  @Return: std::string
 *  
 **/
std::string ParamStatisticsMapping::getStatisticsRecordTimeStamps(std::string& name)
{
    std::string timeStamp("");
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 

    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        timeStamp = m_paramStatisticsMap[name].earliestTimeStamp;
    } else { //未找到
        LogUserOperError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
    } 
    LogUserOperDebug("GET: name[%s], TimeStamp[%s]\n", name.c_str(), timeStamp.c_str());

    return timeStamp;
}


/**
 *  @Func: modifyStatisticsRecordCount
 *  @Param: name, type:: std::string&
 *          IsFlagAdd, type:: int
 *          step, type:: int
 *  @Return: int
 *  
 **/
int ParamStatisticsMapping::modifyStatisticsRecordCount(std::string& name, int IsFlagAdd, int step)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 || name.empty()){
        LogUserOperError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str());
        return -1;
    }
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 
    
    LogUserOperDebug("SET: name[%s], flag[%d], step[%d]\n", name.c_str(), IsFlagAdd, step);
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        if(IsFlagAdd == STATISTAC_RECORD_COUNT_FLAG_ADD){         //add 
            LogUserOperDebug("HASFIND: recordCount[%d].\n", (paramStatisticIt->second).recordCount);
            (paramStatisticIt->second).recordCount = (paramStatisticIt->second).recordCount + 1;
        }else if(IsFlagAdd == STATISTAC_RECORD_COUNT_FLAG_MINUS){ //minus
            LogUserOperDebug("HASFIND: recordCount[%d] step[%d].\n",(paramStatisticIt->second).recordCount, step);
            (paramStatisticIt->second).recordCount = (paramStatisticIt->second).recordCount - step;
        }else{
            LogUserOperError("m_paramStatisticsMap IsFlagAdd is[%d] ERROR.\n", IsFlagAdd);
            return -1;
        }
    } else {
        LogUserOperError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return -1;
    }
    LogUserOperDebug("Has modify RecordCount OK.\n");
    
    return 0;
}

/**
 *  @Func: getStatisticsRecordCount
 *  @Param: name, type:: std::string&
 *  @Return: int
 *  
 **/
int ParamStatisticsMapping::getStatisticsRecordCount(std::string& name)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 || name.empty()){
        LogUserOperError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str());
        return -1;
    }
    int count = 0;
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 
    
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        count = (paramStatisticIt->second).recordCount;
    } else {
        LogUserOperError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return -1;
    }
    LogUserOperDebug("GET: name[%s], count[%d]\n", name.c_str(), count);

    return count;
}


/**
 *  @Func: getStatisticsRecordMap
 *  @Param: 
 *  @Return: std::map<std::string, ParamStatistics>
 *  
 **/
std::map<std::string, ParamStatistics> ParamStatisticsMapping::getStatisticsRecordMap()
{
    LogUserOperDebug("getStatisticsRecordMap.\n");
    return m_paramStatisticsMap;
}


/**
 *  @Func: statisticsRecordMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamStatisticsMapping::statisticsRecordMapOutput()
{
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt;
    
    for(paramStatisticIt = m_paramStatisticsMap.begin(); paramStatisticIt != m_paramStatisticsMap.end(); ++paramStatisticIt)
        cout << "statisticsRecordMapOutput:: key:"<< paramStatisticIt->first << " earliestTimeStamp:" << (paramStatisticIt->second).earliestTimeStamp << " recordCount:" << (paramStatisticIt->second).recordCount <<endl;
    
    return   0;
}


 
//==============================================================================================================================
//==============================================================================================================================
/**
 *  @Func: addConfigFile
 *  @Param: file, type:: std::string
 *  @Return: bool
 *  
 **/
bool ParamConfigFileMapping::addConfigFile(std::string file)
{
    if(file.empty()){
        LogUserOperError("file[%s] cannot empty, ERROR.\n", file.c_str());
        return false;
    }
    m_cfgFileList.push_back(file);
    
    return true;
}

/**
 *  @Func: deleteConfigFile
 *  @Param: file, type:: std::string&
 *  @Return: bool
 *  
 **/
bool ParamConfigFileMapping::deleteConfigFile(std::string file)
{
    if(file.empty()){
        LogUserOperError("file[%s] cannot empty, ERROR.\n", file.c_str());
        return false;
    }
    m_cfgFileList.erase(std::find(m_cfgFileList.begin(), m_cfgFileList.end(), file));
    
    return true;
}


//
/**
 *  @Func: loadConfigFileParam
 *         ps. : 从 m_cfgFileList 文件列表中包含的文件里加载到 m_paramConfigFileMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamConfigFileMapping::loadConfigFileParam()
{
    FILE *fp = NULL;
    char ch;
    std::string line("");
    std::string tag("");
    std::string value("");
    std::vector<std::string>::iterator cfgFileListIt;
    std::string::size_type position;
    
    
    for (cfgFileListIt = m_cfgFileList.begin(); cfgFileListIt != m_cfgFileList.end(); ++cfgFileListIt){
        LogUserOperDebug("I will load \"%s\".\n", (*cfgFileListIt).c_str());
        fp = fopen((*cfgFileListIt).c_str(), "rb");
        if (!fp) {
            LogUserOperError("Open file error!\n");
            return -1;
        }
        while((ch = fgetc(fp)) != (char)EOF || !line.empty()) {
            if(ch != '\n' && ch != (char)EOF) {
                line += ch;
                continue;
            }
            position = line.find('=');
            if (position == std::string::npos) {
                LogSafeOperWarn("Not found \"=\", line=[%s]\n", line.c_str());
                line.clear();
                continue;
            }
            tag = line.substr(0, position);
            value = line.substr(position + 1);
            updateConfigFileParamMap(tag, value);
            line.clear();
        }
        fclose(fp);
        fp = NULL;
        LogUserOperDebug("load \"%s\" ok.\n", (*cfgFileListIt).c_str());
    }
    LogUserOperDebug("Has load ConfigFile OK.\n");
    
    return 0;
}     


//
/**
 *  @Func: updateConfigFileParamMap
 *         ps. : 将解析到的 添加到 m_paramConfigFileMap
 *  @Param: name, type:: std::string&
 *          value, type:: std::string&
 *  @Return: int
 *  
 **/
int ParamConfigFileMapping::updateConfigFileParamMap(std::string& name, std::string& value)
{
    if(name.empty()){
        LogUserOperError("name[%s]  is NULL.\n", name.c_str());
        return -1;
    }
    std::map<std::string, std::string>::iterator cfgFileMapIt = m_paramConfigFileMap.find(name);
    
    LogUserOperDebug("INPUT: name[%s] value[%s].\n", name.c_str(), value.c_str());
    if (gSensitiveParamFilter.filteringSensitiveParam(name)) {
        if (cfgFileMapIt != m_paramConfigFileMap.end())
            cfgFileMapIt->second = "";
        else
            m_paramConfigFileMap[name] = "";
    } else {
        if (cfgFileMapIt != m_paramConfigFileMap.end())
            cfgFileMapIt->second = value;
        else
            m_paramConfigFileMap[name] = value;
    }
    LogUserOperDebug("Has update OK.\n");
    
    return 0;
}     
/**
 *  @Func: unloadConfigFileParam
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamConfigFileMapping::unloadConfigFileParam()
{
    if(m_paramConfigFileMap.empty()) {
        LogUserOperError("unloadConfigFileParam:: m_paramConfigFileMap has been empty.\n");
        return -1;
    }
    m_paramConfigFileMap.erase(m_paramConfigFileMap.begin(), m_paramConfigFileMap.end()); //成片删除要注意的是，也是STL的特性，删除区间是一个前闭后开的集合 
    LogUserOperDebug("Has unload ConfigFile OK.\n");

    return 0;
}

//
/**
 *  @Func: getConfigFileParamValue
 *         ps. :从 m_paramConfigFileMap 获取参数的旧值
 *  @Param: name, type:: std::string&
 *  @Return: std::string
 *  
 **/
std::string ParamConfigFileMapping::getConfigFileParamValue(std::string name)
{
    std::map<std::string, std::string>::iterator cfgFileMapIt;
    std::string value("");
    
    cfgFileMapIt = m_paramConfigFileMap.find(name);
    if (cfgFileMapIt != m_paramConfigFileMap.end())
        value = cfgFileMapIt->second;
    LogUserOperDebug("GET: name[%s] value[%s].\n", name.c_str(), value.c_str());

    return value;
}
/**
 *  @Func: paramConfigFileMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamConfigFileMapping::paramConfigFileMapOutput()
{
    std::map<std::string, std::string>::iterator cfgFileMapIt;
    
    for(cfgFileMapIt = m_paramConfigFileMap.begin();cfgFileMapIt != m_paramConfigFileMap.end(); ++cfgFileMapIt){
        cout << "paramConfigFileMapOutput:: key: "<< cfgFileMapIt->first << " value: " << cfgFileMapIt->second <<endl;
    }
    
    return   0;
}



 
//==============================================================================================================================
//==============================================================================================================================

static std::string gSensitiveListStr[] = {"ntvpasswd", "ntvAESpasswd", "defContPwd", "defAESContpwd", 
                                        "netpasswd", "syncntvpasswd", "syncntvAESpasswd", "netAESpasswd", "dhcppasswd", "ipoeAESpasswd", "wifi_password", 
                                        "authbg_md5", "bootlogo_md5", 
                                        "Device.ManagementServer.Password", 
                                        "Device.ManagementServer.ConnectionRequestPassword", 
                                        "Device.ManagementServer.STUNPassword"
                                        };
                                        
SensitiveParamFiltering::SensitiveParamFiltering()
{
    LogUserOperDebug("\n");
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
        LogUserOperError("param[%s] cannot empty, ERROR.\n", param.c_str());
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

 
 
 
//==============================================================================================================================
//==============================================================================================================================
ParamModifyRecordMapping::ParamModifyRecordMapping(std::string fileName)
    : m_fileName(fileName)
    , m_version(PARAM_CHANGE_RECORD_KEEPING_VERSION)
{
    m_mutex = mid_mutex_create();
    
    //paramStatisticsMapping initial
    m_paramStatisticsMapping = new ParamStatisticsMapping(fileName);    
}

int IsModifyRecordMappingLoaded = 0;
/**
 *  @Func: loadModifyRecordParam
 *         ps. :逐行从文件 config_param_modify.record 加载到 m_paramModifyRecordMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::loadModifyRecordParam()  
{
    (*m_paramStatisticsMapping).loadStatisticsRecord();
    (*m_paramStatisticsMapping).statisticsRecordMapOutput();

    FILE *fp = NULL;
    bool fileAvailableLineFlag = false;
    std::string line("");

    LogUserOperDebug("loadModifyRecordParam.\n");
    std::ifstream fin(m_fileName.c_str(), std::ios::in);
    while(std::getline(fin, line)) {
        if (!fileAvailableLineFlag) {   //清除第一行
            std::string::size_type position = line.find('=');
            if (position != std::string::npos) {
                std::string tag   = line.substr(0, position);
                std::string value = line.substr(position + 1);
                
                if ((tag.compare("CFversion") == 0) && (value.compare(m_version) == 0)) {
                    fileAvailableLineFlag = true;
                    line.clear();
                    LogUserOperDebug("File is Available.\n");
                    continue;
                }
            }
            LogSafeOperWarn("The available information is not found.(%s)\n", line.c_str());
            break;
        }
        LogUserOperDebug("Start parse line[%s].\n", line.c_str());
        updateModifyRecordParamMap(parseRecordLine(line)); //逐行添加
        line.clear();
    }
    LogUserOperDebug("Load %s ok.\n", m_fileName.c_str());
    
    IsModifyRecordMappingLoaded = 1;
    dereplicationModifyRecordParamMap();
    
    return 0;
}

/**
 *  @Func: updateModifyRecordParamMap
 *         ps. :将解析到的 添加到 m_paramModifyRecordMap 
 *  @Param: paramItem, type:: ParametersItem *
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::updateModifyRecordParamMap(ParametersItem *paramItem)
{
    if( (paramItem->paramName).compare(ERROR_PARAM_ITEM.paramName) == 0 
        || (paramItem->paramName).empty() || (paramItem->modifyTimeStamp).empty()){
        LogUserOperError("paramItem[%p], paramName[%s] modifyTimeStamp[%s].\n", paramItem, paramItem->paramName.c_str(), paramItem->modifyTimeStamp.c_str());
        return -1;
    }    
    
    //if has loaded then: sync  m_paramStatisticsMapping
    if(IsModifyRecordMappingLoaded == 1){
        (*m_paramStatisticsMapping).updateStatisticsRecord(paramItem);
    }
    pair<std::string, ParametersItem> modifyRecord(paramItem->paramName, *paramItem);
    m_paramModifyRecordMap.insert(modifyRecord);
    
/*     LogUserOperDebug( "updateModifyRecordParamMap paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
                      (paramItem->paramName).c_str(), (paramItem->newValue).c_str(), (paramItem->sourceModule).c_str(), 
                      (paramItem->oldValue).c_str(), (paramItem->modifyTimeStamp).c_str(), (paramItem->modifiedFile).c_str()
                    );      
 */
    return 0;
}

/**
 *  @Func: dereplicationModifyRecordParamMap
 *         ps. :去除多余的时间戳最早的条目，去除重复
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::dereplicationModifyRecordParamMap()
{
    int count = 0, i = 0;
    int tmpSize = 0, deleteSize = 0;
    std::string timeStamp("");
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt;
    std::map<std::string, ParamStatistics> paramStatisticsMap = (*m_paramStatisticsMapping).getStatisticsRecordMap();
    
    /**
     *  判断 m_paramStatisticsMapping 中的个数:
     *  if 大于 COUNT_OF_PARAM_IN_MODIFY_FILE_MAX 个：
     *      获取 m_paramStatisticsMapping 中的时间戳；
     *      查找时间戳 m_paramModifyRecordMap对应的条目,删除条目；
     *      计数器 -1；
     *      更新获取最早的时间戳；
     *  否则，直接添加；
     **/
    for(paramStatisticIt = paramStatisticsMap.begin(); paramStatisticIt != paramStatisticsMap.end(); ++paramStatisticIt){
        while( (*m_paramStatisticsMapping).getStatisticsRecordCount((paramStatisticIt->second).paramName) > COUNT_OF_PARAM_IN_MODIFY_FILE_MAX ){
            tmpSize = m_paramModifyRecordMap.size();
            //删除最早时间戳的条目
            timeStamp = (*m_paramStatisticsMapping).getStatisticsRecordTimeStamps((paramStatisticIt->second).paramName);
            deleteModifyRecordByTimeStamp((paramStatisticIt->second).paramName, timeStamp);
            
            //更新计数器
            deleteSize = tmpSize - m_paramModifyRecordMap.size();
            LogUserOperDebug("name[%s] has been deleted item count[%d] .\n", (paramStatisticIt->second).paramName.c_str(), tmpSize, deleteSize);
            (*m_paramStatisticsMapping).modifyStatisticsRecordCount((paramStatisticIt->second).paramName, STATISTAC_RECORD_COUNT_FLAG_MINUS, deleteSize);
            
            //更新最早时间戳
            getModifyRecordEarliestTimeStamp((paramStatisticIt->second).paramName, timeStamp);
            (*m_paramStatisticsMapping).modifyStatisticsRecordTimeStamps((paramStatisticIt->second).paramName, timeStamp);
        }
        LogUserOperDebug("name[%s] earliestTimeStamp[%s] recordCount[%d].\n", (paramStatisticIt->second).paramName.c_str(), (paramStatisticIt->second).earliestTimeStamp.c_str(), (paramStatisticIt->second).recordCount);
    }
    
    return 0;
}

/**
 *  @Func: unloadModifyRecordParam
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::unloadModifyRecordParam()  
{
    if(m_paramModifyRecordMap.empty()){
        LogUserOperError("unloadModifyRecordParam:: m_paramModifyRecordMap has been empty, ERROR.\n");
        return -1;
    }
    m_paramModifyRecordMap.erase(m_paramModifyRecordMap.begin(), m_paramModifyRecordMap.end());
    LogUserOperDebug("Has unload ModifyRecord OK.\n");

    return 0;
}

/**
 *  @Func: getModifyRecordEarliestTimeStamp
 *  @Param: name, type:: std::string&
 *          timeStamp, type:: std::string&
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::getModifyRecordEarliestTimeStamp(std::string& name, std::string& timeStamp)
{
    if(name.empty()){
        LogUserOperError(" paramName[%s].\n",name.c_str());
        return -1;
    }
    std::multimap<std::string, ParametersItem>::iterator it = m_paramModifyRecordMap.find(name);
    
    if(it == m_paramModifyRecordMap.end()){
        LogUserOperDebug("Cannot find ModifyRecordParam[%s] .\n", name.c_str());
        return -1;
    }
    timeStamp = (it->second).modifyTimeStamp;
    it = m_paramModifyRecordMap.lower_bound(name);  
    while(it != m_paramModifyRecordMap.upper_bound(name)) {  
        LogUserOperDebug("modifyTimeStamp[%s], TimeStamp[%s]\n", (it->second).modifyTimeStamp.c_str(), timeStamp.c_str());
        if(dateTimeCompare(timeStamp, (it->second).modifyTimeStamp) > 0){
            timeStamp = (it->second).modifyTimeStamp;
        }
        ++it;  
    } 
    LogUserOperDebug("GET: name[%s] earliestTimeStamp[%s].\n", name.c_str(), timeStamp.c_str());
    
    return 0;
}

/**
 *  @Func: formatingParamItemOutput
 *  @Param: 
 *  @Return: int
 *  
 *  e.g: Parameter[ Device.Username ] @<2012-Jan-01 08:02:05.521> modified by[ JS ]from[ testcpe ]to[ testcpe55 ]into file[ /root/yx_config_tr069.ini ].
 **/
int ParamModifyRecordMapping::formatingParamItemOutput()
{
    FILE *fp = NULL;
    std::string line("");
    std::multimap<std::string, ParametersItem>::iterator it;

    mid_mutex_lock(m_mutex);
    if ((fp = fopen(m_fileName.c_str(), "wb")) == NULL) {
        LogUserOperError("Open file error.(%s)\n", m_fileName.c_str());
        return -1;
    }

    line = "CFversion=" + m_version + "\n";
    fwrite(line.c_str(), 1, line.length(), fp);
    for (it = m_paramModifyRecordMap.begin(); it != m_paramModifyRecordMap.end(); ++it) {
        line  = "Parameter[ "     + it->first;
        line += " ] @<"           + (it->second).modifyTimeStamp;
        line += "> modified by[ " + (it->second).sourceModule;
        if (gSensitiveParamFilter.filteringSensitiveParam(it->first))
            line += " ]from[ *** ]to[ ***";
        else {
            line += " ]from[ "    + (it->second).oldValue;
            line += " ]to[ "      + (it->second).newValue;
        }
        line += " ]into file[ "   + (it->second).modifiedFile;
        line += " ].\n";
        fwrite(line.c_str(), 1, line.length(), fp);
    }
    fclose(fp);
    mid_mutex_unlock(m_mutex);
    LogUserOperDebug("Format Output to [%s] ok.\n", m_fileName.c_str());

    //(*m_paramStatisticsMapping).statisticsRecordMapOutput();
    return 0;
}

/**
 *  @Func: resetModifyRecordOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::resetModifyRecordOutput()
{
    FILE *fp = NULL;
    std::string line("");
    
    mid_mutex_lock(m_mutex);
    if ((fp = fopen(m_fileName.c_str(), "wb")) == NULL) {
        LogUserOperError("Open file error.(%s)\n", m_fileName.c_str());
        return -1;
    }
    line = "CFversion=" + m_version + "\n";
    fwrite(line.c_str(), 1, line.length(), fp);
    fclose(fp);
    mid_mutex_unlock(m_mutex);
    LogUserOperDebug("Reset [%s] ok.\n", m_fileName.c_str());

    return 0;
}

/**
 *  @Func: deleteModifyRecordByTimeStamp
 *  @Param: name, type:: std::string&
 *          timeStamp, type:: std::string&
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::deleteModifyRecordByTimeStamp(std::string& name, std::string& timeStamp)
{
    std::multimap<std::string, ParametersItem>::iterator it = m_paramModifyRecordMap.find(name);
    
    LogUserOperDebug("RecordParam[%s]  modifyTimeStamp[%s] .\n", name.c_str(), timeStamp.c_str());
    if(it == m_paramModifyRecordMap.end()){
        LogUserOperError("Cannot find ModifyRecordParam[%s] modifyTimeStamp[%s] .\n", name.c_str(), timeStamp.c_str());
        return -1;
    }
#if 0
    //delete method 1
    it = m_paramModifyRecordMap.lower_bound(name);  
    while(it != m_paramModifyRecordMap.upper_bound(name)) {  
        LogUserOperDebug("earliestTimeStamp[%s].\n", (it->second).modifyTimeStamp.c_str());
        if(dateTimeCompare((it->second).modifyTimeStamp, timeStamp) == 0){
            m_paramModifyRecordMap.erase(it);
        }
        ++it;  
    } 
#else
    //delete method 2
    pair<std::multimap<std::string, ParametersItem>::iterator, std::multimap<std::string, ParametersItem>::iterator> position;
    position = m_paramModifyRecordMap.equal_range(name);
    /**
     *  如果键存在，函数返回2个指针，第一个指针指向键第一个匹配的元素
     *  第二个指针指向键最后一个匹配的元素的下一位置
     **/
    while (position.first != position.second){
        LogUserOperDebug(" earliestTimeStamp[%s]\n", (position.first->second).modifyTimeStamp.c_str());
        if(dateTimeCompare(((position.first)->second).modifyTimeStamp, timeStamp) == 0){
            m_paramModifyRecordMap.erase(position.first);
        }
        position.first++;
    }
#endif
    LogUserOperDebug("Has delete OK.\n");
    
    //paramModifyRecordMapOutput();
    return  0;
}

/**
 *  @Func: paramModifyRecordMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int ParamModifyRecordMapping::paramModifyRecordMapOutput()
{
    std::multimap<std::string, ParametersItem>::iterator it;
    
    for(it = m_paramModifyRecordMap.begin(); it != m_paramModifyRecordMap.end(); ++it)
        cout << "paramModifyRecordMapOutput:: key:"<< it->first << " paramName:" << (it->second).paramName << " newValue:" << (it->second).newValue << " sourceModule: " << (it->second).sourceModule << " oldValue:" << (it->second).oldValue << " modifyTimeStamp:" << (it->second).modifyTimeStamp << " modifiedFile:" << (it->second).modifiedFile  <<endl;
    
    return   0;
}





//==============================================================================================================================
//==============================================================================================================================
#define     SYSTEM_OPTIONS_FILE             CONFIG_FILE_DIR"/yx_config_system.ini"
#define     CUSTOM_OPTIONS_FILE             CONFIG_FILE_DIR"/yx_config_customer.ini"
#define     MONITOR_OPTIONS_FILE            CONFIG_FILE_DIR"/yx_config_tr069.ini"
#define     VERSION_OPTIONS_FILE            CONFIG_FILE_DIR"/yx_config_version.ini"

int gModifyRecordResetFlag = 0;

SettingModifyRecordKeeping::SettingModifyRecordKeeping(std::string fileName) 
    : dirty(false)
{
    //create ParamStatisticsMapping ParamConfigFileMapping  ParamModifyRecordMapping       
    m_paramConfigFileMapping = new ParamConfigFileMapping();
    (*m_paramConfigFileMapping).addConfigFile(SYSTEM_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(CUSTOM_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(MONITOR_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(VERSION_OPTIONS_FILE);
    
    //initial  m_paramModifyRecordMapping  
    m_paramModifyRecordMapping = new ParamModifyRecordMapping(fileName);
}

/**
 *  @Func: load
 *         ps. :initial ParamStatisticsMapping  ParamConfigFileMapping  ParamModifyRecordMapping 
 *  @Param: 
 *  @Return: int
 *  
 **/
int SettingModifyRecordKeeping::load(void)
{
    (*m_paramConfigFileMapping).loadConfigFileParam();
    (*m_paramModifyRecordMapping).loadModifyRecordParam();
    LogUserOperDebug("Has loading OK.\n");
    
    //(*m_paramModifyRecordMapping).paramModifyRecordMapOutput();
    
    return 0;
}
/**
 *  @Func: save
 *  @Param: 
 *  @Return: int
 *  
 *  e.g: <2012-Jan-01 08:02:05.521>
 **/
int SettingModifyRecordKeeping::save(void)
{
    if(gModifyRecordResetFlag == 1){
        LogUserOperDebug("save, gModifyRecordResetFlag = 1.\n");
        return 0;
    }
    // sync  m_paramConfigFileMapping && save m_paramModifyRecordMapping to file
    (*m_paramConfigFileMapping).updateConfigFileParamMap(m_paramItem.paramName, m_paramItem.newValue);
    (*m_paramModifyRecordMapping).formatingParamItemOutput();
    LogUserOperDebug("Has saved OK.\n");
    
    return 0;
}
/**
 *  @Func: reSet
 *  @Param: 
 *  @Return: int
 *  
 *  e.g: <2012-Jan-01 08:02:05.521>
 **/
int SettingModifyRecordKeeping::reSet(void)
{
    //delete file
    fstream fstr(PARAM_CHANGE_RECORD_KEEPING_FILE, ios::out | ios::binary);
    fstr << endl;
    fstr.close();
    (*m_paramModifyRecordMapping).resetModifyRecordOutput();
    gModifyRecordResetFlag = 1;
    
    LogUserOperDebug("Has reSetting OK.\n");
    return 0;
}
/**
 *  @Func: set
 *  @Param: name, type:: const char *
 *          value, type:: const char *
 *          module, type:: RecordChangedSource
 *          fileTag, type:: const char *
 *  @Return: int
 *  
 **/
int SettingModifyRecordKeeping::set(const char* name, const char* value, RecordChangedSource module, const char* fileTag)
{
    initialParamItem(name, value, module, fileTag);
    
    //save to m_paramModifyRecordMapping && dereplication
    (*m_paramModifyRecordMapping).updateModifyRecordParamMap(&m_paramItem);
    (*m_paramModifyRecordMapping).dereplicationModifyRecordParamMap();

    //sync  m_paramConfigFileMapping
    (*m_paramConfigFileMapping).updateConfigFileParamMap(m_paramItem.paramName, m_paramItem.newValue);
    LogUserOperDebug("Has set item[%s] value[%s] OK.\n", name, value);

    return 0;
}


/**
 *  @Func: setModifyTimeStamp
 *  @Param: 
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::setModifyTimeStamp()
{
    char buf[64] = {0};
    SysTime::DateTime sDTime;
    struct timespec sTimeSpec;

    SysTime::GetDateTime(&sDTime);
    clock_gettime(CLOCK_MONOTONIC, &sTimeSpec);
    snprintf(buf, 64 - 1, "%04d-%s-%02d %02d:%02d:%02d.%03d", 
                            sDTime.mYear, gMonthStr[sDTime.mMonth].c_str(), sDTime.mDay, 
                            sDTime.mHour, sDTime.mMinute, sDTime.mSecond, sTimeSpec.tv_nsec / 1000000);
                            
    m_paramItem.modifyTimeStamp = buf;
    LogUserOperDebug("SET: modifyTimeStamp[%s] OK.\n", (m_paramItem.modifyTimeStamp).c_str());

    return ;
}

/**
 *  @Func: setModifySourceModule
 *  @Param: module, type:: RecordChangedSource
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::setModifySourceModule(RecordChangedSource module)
{
    std::string value("");
    
    switch(module) {
    case RecordChangedSource_STBMonitor:
        m_paramItem.sourceModule = "STBManageTool";
        break;
    case RecordChangedSource_tr069Manager:
        m_paramItem.sourceModule = "TR069";
        break;
    case RecordChangedSource_JsLocalSetting:
        m_paramItem.sourceModule = "JSLocal";
        break;
    case RecordChangedSource_JsEpgSetting:
        m_paramItem.sourceModule = "JSEPG";
        break;
    case RecordChangedSource_Other:
        m_paramItem.sourceModule = "Other";
        break;
    default:
        m_paramItem.sourceModule = "";
        break;
    }
    LogUserOperDebug("SET: sourceModule[%s] OK.\n", (m_paramItem.sourceModule).c_str());

    return ;
}

/**
 *  @Func: setModifiedFile
 *  @Param: fileTag, type:: const char *
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::setModifiedFile(const char* fileTag)
{
    std::string fTag(fileTag);
    
    if(fTag.compare("system") == 0){
        m_paramItem.modifiedFile = CONFIG_FILE_DIR"/yx_config_system.ini";
    } else if(fTag.compare("customer") == 0){
        m_paramItem.modifiedFile = CONFIG_FILE_DIR"/yx_config_customer.ini";
    } else if(fTag.compare("tr069") == 0){
        m_paramItem.modifiedFile = CONFIG_FILE_DIR"/yx_config_tr069.ini";
    } else if(fTag.compare("version") == 0){
        m_paramItem.modifiedFile = CONFIG_FILE_DIR"/yx_config_version.ini";
    } else {
        m_paramItem.modifiedFile = "";
    }
    LogUserOperDebug("SET: modifiedFile[%s].\n", (m_paramItem.modifiedFile).c_str());

    return ;
}

/**
 *  @Func: setParamOldValue
 *  @Param: 
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::setParamOldValue()
{
    if(m_paramItem.paramName.empty()){
        LogUserOperError("setParamOldValue m_paramItem.paramName[%s]ERROR.\n", m_paramItem.paramName.c_str());
        return;
    }
    m_paramItem.oldValue = (*m_paramConfigFileMapping).getConfigFileParamValue(m_paramItem.paramName);
    LogUserOperDebug("SET: oldvalue[%s] OK.\n", m_paramItem.oldValue.c_str());

    return ;
}

/**
 *  @Func: initialParamItem
 *  @Param: name, type:: const char *
 *          value, type:: const char *
 *          module, type:: RecordChangedSource
 *          fileTag, type:: const char *
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::initialParamItem(const char* name, const char* value, RecordChangedSource module, const char* fileTag)
{
    m_paramItem.paramName = name;
    m_paramItem.newValue = value;
    setParamOldValue();
    setModifyTimeStamp();
    setModifySourceModule(module);
    setModifiedFile(fileTag);
    LogUserOperDebug("initialParamItem m_paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
                    m_paramItem.paramName.c_str(), m_paramItem.newValue.c_str(), (m_paramItem.sourceModule).c_str(), 
                    m_paramItem.oldValue.c_str(), m_paramItem.modifyTimeStamp.c_str(), m_paramItem.modifiedFile.c_str());
    
    return ;
}


//==============================================================================================================================
//==============================================================================================================================
}
 
 
//g_settingModifyRecord
extern "C" {

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
void settingModifyRecordLoad()
{
    pthread_mutex_lock(&g_mutex);
    Hippo::g_settingModifyRecordKeeping.load();
    pthread_mutex_unlock(&g_mutex);
    LogUserOperDebug("settingModifyRecordLoad.\n");
    
    return;
}

void settingModifyRecordSave()
{
    LogUserOperDebug("settingModifyRecordSave.\n");
    Hippo::g_settingModifyRecordKeeping.save();
        
    return;
}

//settingModifyRecordSet("syncntvAESpasswd", sysConfigs.syncntvAESpasswd, (Modify_Mode)whoModify, "system");
void settingModifyRecordSet(const char* name, const char* value, RecordChangedSource module, const char* file)
{
    Hippo::g_settingModifyRecordKeeping.set(name, value, module, file);
    LogUserOperDebug("settingModifyRecordSet Name[%s] value[%s]  module[%d] file[%s].\n", name, value, module, file);
    
    return;
}

void settingModifyRecordReset(RecordChangedSource module)
{
    LogUserOperDebug("settingModifyRecordReset.\n");
    Hippo::g_settingModifyRecordKeeping.reSet();
    
    return;
}

}

 
 














