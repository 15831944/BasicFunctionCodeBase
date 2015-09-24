#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "MappingSettingStatistics.h"
 
using namespace std;



static int gIsDeleteFlag = 0;

MappingSettingStatistics::MappingSettingStatistics(std::string fileName)
    : m_filePath(fileName)
{
    m_RecordFileAvailable = 0;
    settingsLogVerbose(" initial.\n");
    
}


/**
 *  @Func: loadStatisticsRecord
 *          ps.: 逐行从 config_param_modify.record 文件加载到 m_paramStatisticsMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingStatistics::loadStatisticsRecord()
{
    FILE *fp = NULL;
    char ch;
    std::string line("");
    
    settingsLogVerbose("\n");
    std::ifstream fin(m_filePath.c_str(), std::ios::in);
    while(std::getline(fin, line)) {
        if (!m_RecordFileAvailable) {   //清除第一行
            m_RecordFileAvailable = 1;
            std::string::size_type position = line.find('=');
            if (position != std::string::npos) {
                settingsLogVerbose("Will jump the line[%s].\n", line.c_str());
                line.clear();
                continue;
            }
            settingsLogWarning("The available information is not found.(%s)\n", line.c_str());
            break;
        }
        settingsLogVerbose("Start parse the line[%s].\n", line.c_str());
        updateStatisticsRecord(parseRecordLine(line));   //逐行添加
        line.clear();
    }
    settingsLogVerbose("Load \"%s\" ok.\n", m_filePath.c_str());
    
    return 0;
}


/**
 *  @Func: updateStatisticsRecord
 *         ps. :将逐行解析到的 添加到 m_paramStatisticsMap
 *  @Param: paramItem, type:: ParametersItem *
 *  @Return: int
 *  
 **/
int MappingSettingStatistics::updateStatisticsRecord(ParametersItem *paramItem)
{
    ParamStatistics *paramStatistic = new ParamStatistics();
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(paramItem->paramName); 
    
    if( (paramItem->paramName).compare(ERROR_PARAM_ITEM.paramName) == 0 
        || (paramItem->paramName).empty() || (paramItem->modifyTimeStamp).empty()){
        settingsLogError("paramItem[%p], paramName[%s] modifyTimeStamp[%s].\n", paramItem, paramItem->paramName.c_str(), paramItem->modifyTimeStamp.c_str());
        return -1;
    }
    paramStatistic->paramName = paramItem->paramName;
    paramStatistic->earliestTimeStamp = paramItem->modifyTimeStamp;
    settingsLogVerbose("INPUT: paramName[%s] modifyTimeStamp[%s].\n", paramStatistic->paramName.c_str(), paramStatistic->earliestTimeStamp.c_str());

    //判断字段是否在统计列表 m_paramStatisticsMap
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到        
        settingsLogVerbose("ISFIND: recordCount[%d], earliestTimeStamp[%s]\n", (paramStatisticIt->second).recordCount, (paramStatisticIt->second).earliestTimeStamp.c_str());
        if(dateTimeCompare((paramStatisticIt->second).earliestTimeStamp,  paramStatistic->earliestTimeStamp) > 0){  //时间早于m_paramStatisticsMap中的时间戳   
            paramStatistic->recordCount = (paramStatisticIt->second).recordCount;
            m_paramStatisticsMap[paramStatistic->paramName] = *paramStatistic;
        } 
    } else { //未找到
        paramStatistic->recordCount = 0;
        m_paramStatisticsMap[paramStatistic->paramName] = *paramStatistic;
        settingsLogError("\n");
    }
    modifyStatisticsRecordCount(paramStatistic->paramName, STATISTAC_RECORD_COUNT_FLAG_ADD, 1);
    settingsLogVerbose("Has update OK.\n");
    
/*     settingsLogVerbose("updateStatisticsRecord paramStatistic->paramName[%s] paramStatistic->earliestTimeStamp[%s] paramStatistic->recordCount[%d].\n", 
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
int MappingSettingStatistics::unloadStatisticsRecord()
{
    if(m_paramStatisticsMap.empty()){
        settingsLogError("unloadStatisticsRecord:: m_paramStatisticsMap has been empty, ERROR.\n");
        return -1;
    }
    m_paramStatisticsMap.erase(m_paramStatisticsMap.begin(), m_paramStatisticsMap.end());
    settingsLogVerbose("Has unload OK.\n");
    
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
bool MappingSettingStatistics::modifyStatisticsRecordTimeStamps(std::string& name, std::string& timeStamp)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 
        || name.empty() || timeStamp.empty()){
        settingsLogError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str(), timeStamp.c_str());
        return false;
    }
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 

    settingsLogVerbose("SET: name[%s], TimeStamp[%s]\n", name.c_str(), timeStamp.c_str());
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        settingsLogVerbose("HASFIND: earliestTimeStamp[%s]\n", (paramStatisticIt->second).earliestTimeStamp.c_str());
        m_paramStatisticsMap[name].earliestTimeStamp = timeStamp;
    } else { //未找到
        settingsLogVerbose("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return false;
    } 
    settingsLogVerbose("Has modify TimeStamps OK.\n");
    
    return true;
}


/**
 *  @Func: getStatisticsRecordTimeStamps
 *  @Param: name, type:: std::string&
 *  @Return: std::string
 *  
 **/
std::string MappingSettingStatistics::getStatisticsRecordTimeStamps(std::string& name)
{
    std::string timeStamp("");
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 

    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        timeStamp = m_paramStatisticsMap[name].earliestTimeStamp;
    } else { //未找到
        settingsLogError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
    } 
    settingsLogVerbose("GET: name[%s], TimeStamp[%s]\n", name.c_str(), timeStamp.c_str());

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
int MappingSettingStatistics::modifyStatisticsRecordCount(std::string& name, int IsFlagAdd, int step)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 || name.empty()){
        settingsLogError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str());
        return -1;
    }
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 
    
    settingsLogVerbose("SET: name[%s], flag[%d], step[%d]\n", name.c_str(), IsFlagAdd, step);
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        if(IsFlagAdd == STATISTAC_RECORD_COUNT_FLAG_ADD){         //add 
            settingsLogVerbose("HASFIND: recordCount[%d].\n", (paramStatisticIt->second).recordCount);
            (paramStatisticIt->second).recordCount = (paramStatisticIt->second).recordCount + 1;
        }else if(IsFlagAdd == STATISTAC_RECORD_COUNT_FLAG_MINUS){ //minus
            settingsLogVerbose("HASFIND: recordCount[%d] step[%d].\n",(paramStatisticIt->second).recordCount, step);
            (paramStatisticIt->second).recordCount = (paramStatisticIt->second).recordCount - step;
        }else{
            settingsLogError("m_paramStatisticsMap IsFlagAdd is[%d] ERROR.\n", IsFlagAdd);
            return -1;
        }
    } else {
        settingsLogError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return -1;
    }
    settingsLogVerbose("Has modify RecordCount OK.\n");
    
    return 0;
}


/**
 *  @Func: getStatisticsRecordCount
 *  @Param: name, type:: std::string&
 *  @Return: int
 *  
 **/
int MappingSettingStatistics::getStatisticsRecordCount(std::string& name)
{
    if( name.compare(ERROR_PARAM_ITEM.paramName) == 0 || name.empty()){
        settingsLogError("paramName[%s] modifyTimeStamp[%s].\n", name.c_str());
        return -1;
    }
    int count = 0;
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt = m_paramStatisticsMap.find(name); 
    
    if (paramStatisticIt != m_paramStatisticsMap.end()){ //找到
        count = (paramStatisticIt->second).recordCount;
    } else {
        settingsLogError("m_paramStatisticsMap cannot find param[%s].\n", name.c_str());
        return -1;
    }
    settingsLogVerbose("GET: name[%s], count[%d]\n", name.c_str(), count);

    return count;
}


/**
 *  @Func: getStatisticsRecordMap
 *  @Param: 
 *  @Return: std::map<std::string, ParamStatistics>
 *  
 **/
std::map<std::string, ParamStatistics> MappingSettingStatistics::getStatisticsRecordMap()
{
    settingsLogVerbose("getStatisticsRecordMap.\n");
    return m_paramStatisticsMap;
}


/**
 *  @Func: statisticsRecordMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingStatistics::statisticsRecordMapOutput()
{
    std::map<std::string, ParamStatistics>::iterator paramStatisticIt;
    
    for(paramStatisticIt = m_paramStatisticsMap.begin(); paramStatisticIt != m_paramStatisticsMap.end(); ++paramStatisticIt)
        cout << "statisticsRecordMapOutput:: key:"<< paramStatisticIt->first << " earliestTimeStamp:" << (paramStatisticIt->second).earliestTimeStamp << " recordCount:" << (paramStatisticIt->second).recordCount <<endl;
    
    return   0;
}













