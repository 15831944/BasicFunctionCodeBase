#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <stdio.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "MappingSettingModifyRecord.h"
#include "SensitiveParamFiltering.h"

 
using namespace std;


int IsModifyRecordMappingLoaded = 0;



MappingSettingModifyRecord::MappingSettingModifyRecord(std::string fileName)
    : m_fileName(fileName)
    , m_version(PARAM_CHANGE_RECORD_KEEPING_VERSION)
{
    if (m_mutex) {
        settingsLogWarning("thread mutex has init.\n");
    }
    m_mutex = ThreadMutexCreate( );
    
    //paramStatisticsMapping initial
    m_paramStatisticsMapping = new MappingSettingStatistics(fileName);    
}


/**
 *  @Func: loadModifyRecordParam
 *         ps. :逐行从文件 config_param_modify.record 加载到 m_paramModifyRecordMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::loadModifyRecordParam()  
{
    (*m_paramStatisticsMapping).loadStatisticsRecord();
    (*m_paramStatisticsMapping).statisticsRecordMapOutput();

    FILE *fp = NULL;
    bool fileAvailableLineFlag = false;
    std::string line("");

    settingsLogVerbose("loadModifyRecordParam.\n");
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
                    settingsLogVerbose("File is Available.\n");
                    continue;
                }
            }
            settingsLogWarning("The available information is not found.(%s)\n", line.c_str());
            break;
        }
        settingsLogVerbose("Start parse line[%s].\n", line.c_str());
        updateModifyRecordParamMap(parseRecordLine(line)); //逐行添加
        line.clear();
    }
    settingsLogVerbose("Load %s ok.\n", m_fileName.c_str());
    
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
int MappingSettingModifyRecord::updateModifyRecordParamMap(ParametersItem *paramItem)
{
    if( (paramItem->paramName).compare(ERROR_PARAM_ITEM.paramName) == 0 
        || (paramItem->paramName).empty() || (paramItem->modifyTimeStamp).empty()){
        settingsLogError("paramItem[%p], paramName[%s] modifyTimeStamp[%s].\n", paramItem, paramItem->paramName.c_str(), paramItem->modifyTimeStamp.c_str());
        return -1;
    }    
    
    //if has loaded then: sync  m_paramStatisticsMapping
    if(IsModifyRecordMappingLoaded == 1){
        (*m_paramStatisticsMapping).updateStatisticsRecord(paramItem);
    }
    pair<std::string, ParametersItem> modifyRecord(paramItem->paramName, *paramItem);
    m_paramModifyRecordMap.insert(modifyRecord);
    
/*     settingsLogVerbose( "updateModifyRecordParamMap paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
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
int MappingSettingModifyRecord::dereplicationModifyRecordParamMap()
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
            settingsLogVerbose("name[%s] has been deleted item count[%d] .\n", (paramStatisticIt->second).paramName.c_str(), tmpSize, deleteSize);
            (*m_paramStatisticsMapping).modifyStatisticsRecordCount((paramStatisticIt->second).paramName, STATISTAC_RECORD_COUNT_FLAG_MINUS, deleteSize);
            
            //更新最早时间戳
            getModifyRecordEarliestTimeStamp((paramStatisticIt->second).paramName, timeStamp);
            (*m_paramStatisticsMapping).modifyStatisticsRecordTimeStamps((paramStatisticIt->second).paramName, timeStamp);
        }
        settingsLogVerbose("name[%s] earliestTimeStamp[%s] recordCount[%d].\n", (paramStatisticIt->second).paramName.c_str(), (paramStatisticIt->second).earliestTimeStamp.c_str(), (paramStatisticIt->second).recordCount);
    }
    
    return 0;
}


/**
 *  @Func: unloadModifyRecordParam
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::unloadModifyRecordParam()  
{
    if(m_paramModifyRecordMap.empty()){
        settingsLogError("unloadModifyRecordParam:: m_paramModifyRecordMap has been empty, ERROR.\n");
        return -1;
    }
    m_paramModifyRecordMap.erase(m_paramModifyRecordMap.begin(), m_paramModifyRecordMap.end());
    settingsLogVerbose("Has unload ModifyRecord OK.\n");

    return 0;
}


/**
 *  @Func: getModifyRecordEarliestTimeStamp
 *  @Param: name, type:: std::string&
 *          timeStamp, type:: std::string&
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::getModifyRecordEarliestTimeStamp(std::string& name, std::string& timeStamp)
{
    if(name.empty()){
        settingsLogError(" paramName[%s].\n",name.c_str());
        return -1;
    }
    std::multimap<std::string, ParametersItem>::iterator it = m_paramModifyRecordMap.find(name);
    
    if(it == m_paramModifyRecordMap.end()){
        settingsLogVerbose("Cannot find ModifyRecordParam[%s] .\n", name.c_str());
        return -1;
    }
    timeStamp = (it->second).modifyTimeStamp;
    it = m_paramModifyRecordMap.lower_bound(name);  
    while(it != m_paramModifyRecordMap.upper_bound(name)) {  
        settingsLogVerbose("modifyTimeStamp[%s], TimeStamp[%s]\n", (it->second).modifyTimeStamp.c_str(), timeStamp.c_str());
        if(dateTimeCompare(timeStamp, (it->second).modifyTimeStamp) > 0){
            timeStamp = (it->second).modifyTimeStamp;
        }
        ++it;  
    } 
    settingsLogVerbose("GET: name[%s] earliestTimeStamp[%s].\n", name.c_str(), timeStamp.c_str());
    
    return 0;
}


/**
 *  @Func: formatingParamItemOutput
 *  @Param: 
 *  @Return: int
 *  
 *  e.g: Parameter[ Device.Username ] @<2012-Jan-01 08:02:05.521> modified by[ JS ]from[ testcpe ]to[ testcpe55 ]into file[ /root/yx_config_tr069.ini ].
 **/
int MappingSettingModifyRecord::formatingParamItemOutput()
{
    FILE *fp = NULL;
    std::string line("");
    std::multimap<std::string, ParametersItem>::iterator it;

    if (ThreadMutexLock(m_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    if ((fp = fopen(m_fileName.c_str(), "wb")) == NULL) {
        settingsLogError("Open file error.(%s)\n", m_fileName.c_str());
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
    if (ThreadMutexUnLock(m_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    settingsLogVerbose("Format Output to [%s] ok.\n", m_fileName.c_str());

    //(*m_paramStatisticsMapping).statisticsRecordMapOutput();  //for test
    return 0;
}


/**
 *  @Func: resetModifyRecordOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::resetModifyRecordOutput()
{
    FILE *fp = NULL;
    std::string line("");
    
    if (ThreadMutexLock(m_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    if ((fp = fopen(m_fileName.c_str(), "wb")) == NULL) {
        settingsLogError("Open file error.(%s)\n", m_fileName.c_str());
        return -1;
    }
    line = "CFversion=" + m_version + "\n";
    fwrite(line.c_str(), 1, line.length(), fp);
    fclose(fp);
    if (ThreadMutexUnLock(m_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    settingsLogVerbose("Reset [%s] ok.\n", m_fileName.c_str());

    return 0;
}


/**
 *  @Func: deleteModifyRecordByTimeStamp
 *  @Param: name, type:: std::string&
 *          timeStamp, type:: std::string&
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::deleteModifyRecordByTimeStamp(std::string& name, std::string& timeStamp)
{
    std::multimap<std::string, ParametersItem>::iterator it = m_paramModifyRecordMap.find(name);
    
    settingsLogVerbose("RecordParam[%s]  modifyTimeStamp[%s] .\n", name.c_str(), timeStamp.c_str());
    if(it == m_paramModifyRecordMap.end()){
        settingsLogError("Cannot find ModifyRecordParam[%s] modifyTimeStamp[%s] .\n", name.c_str(), timeStamp.c_str());
        return -1;
    }
#if 0
    //delete method 1
    it = m_paramModifyRecordMap.lower_bound(name);  
    while(it != m_paramModifyRecordMap.upper_bound(name)) {  
        settingsLogVerbose("earliestTimeStamp[%s].\n", (it->second).modifyTimeStamp.c_str());
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
        settingsLogVerbose(" earliestTimeStamp[%s]\n", (position.first->second).modifyTimeStamp.c_str());
        if(dateTimeCompare(((position.first)->second).modifyTimeStamp, timeStamp) == 0){
            m_paramModifyRecordMap.erase(position.first);
        }
        position.first++;
    }
#endif
    settingsLogVerbose("Has delete OK.\n");
    
    //paramModifyRecordMapOutput();   //for test
    return  0;
}


/**
 *  @Func: paramModifyRecordMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingModifyRecord::paramModifyRecordMapOutput()
{
    std::multimap<std::string, ParametersItem>::iterator it;
    
    for(it = m_paramModifyRecordMap.begin(); it != m_paramModifyRecordMap.end(); ++it)
        cout << "paramModifyRecordMapOutput:: key:"<< it->first << " paramName:" << (it->second).paramName << " newValue:" << (it->second).newValue << " sourceModule: " << (it->second).sourceModule << " oldValue:" << (it->second).oldValue << " modifyTimeStamp:" << (it->second).modifyTimeStamp << " modifiedFile:" << (it->second).modifiedFile  <<endl;
    
    return   0;
}




 
 














