#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "SettingModifyRecordKeeping.h"

 
using namespace std; 


int gModifyRecordResetFlag = 0;


SettingModifyRecordKeeping::SettingModifyRecordKeeping(std::string fileName) 
{
    //create ParamStatisticsMapping MappingSettingConfigFile  MappingSettingModifyRecord       
    m_paramConfigFileMapping = new MappingSettingConfigFile();
    (*m_paramConfigFileMapping).addConfigFile(SYSTEM_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(CUSTOM_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(MONITOR_OPTIONS_FILE);
    (*m_paramConfigFileMapping).addConfigFile(VERSION_OPTIONS_FILE);
    
    //initial  m_paramModifyRecordMapping  
    m_paramModifyRecordMapping = new MappingSettingModifyRecord(fileName);
}

/**
 *  @Func: load
 *         ps. :initial ParamStatisticsMapping  MappingSettingConfigFile  MappingSettingModifyRecord 
 *  @Param: 
 *  @Return: int
 *  
 **/
int SettingModifyRecordKeeping::load(void)
{
    (*m_paramConfigFileMapping).loadConfigFileParam();
    (*m_paramModifyRecordMapping).loadModifyRecordParam();
    settingsLogVerbose("Has loading OK.\n");
    
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
        settingsLogVerbose("save, gModifyRecordResetFlag = 1.\n");
        return 0;
    }
    // sync  m_paramConfigFileMapping && save m_paramModifyRecordMapping to file
    (*m_paramConfigFileMapping).updateConfigFileParamMap(m_paramItem.paramName, m_paramItem.newValue);
    (*m_paramModifyRecordMapping).formatingParamItemOutput();
    settingsLogVerbose("Has saved OK.\n");
    
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
    
    settingsLogVerbose("Has reSetting OK.\n");
    return 0;
}
/**
 *  @Func: set
 *  @Param: name, type:: const char *
 *          value, type:: const char *
 *          module, type:: int
 *          fileTag, type:: const char *
 *  @Return: int
 *  
 **/
int SettingModifyRecordKeeping::set(const char* name, const char* value, int module, const char* fileTag)
{
    initialParamItem(name, value, module, fileTag);
    
    //save to m_paramModifyRecordMapping && dereplication
    (*m_paramModifyRecordMapping).updateModifyRecordParamMap(&m_paramItem);
    (*m_paramModifyRecordMapping).dereplicationModifyRecordParamMap();

    //sync  m_paramConfigFileMapping
    (*m_paramConfigFileMapping).updateConfigFileParamMap(m_paramItem.paramName, m_paramItem.newValue);
    settingsLogVerbose("Has set item[%s] value[%s] OK.\n", name, value);

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
    DateTimeStamp sDTime;
    struct timespec sTimeSpec;

    getDateTimeStamp(&sDTime);
    clock_gettime(CLOCK_MONOTONIC, &sTimeSpec);
    snprintf(buf, 64 - 1, "%04d-%s-%02d %02d:%02d:%02d.%03d", 
                            sDTime.mYear, gMonthStr[sDTime.mMonth].c_str(), sDTime.mDay, 
                            sDTime.mHour, sDTime.mMinute, sDTime.mSecond, sTimeSpec.tv_nsec / 1000000);
                            
    m_paramItem.modifyTimeStamp = buf;
    settingsLogVerbose("SET: modifyTimeStamp[%s] OK.\n", (m_paramItem.modifyTimeStamp).c_str());

    return ;
}

/**
 *  @Func: setModifySourceModule
 *  @Param: module, type:: int
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::setModifySourceModule(int module)
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
    settingsLogVerbose("SET: sourceModule[%s] OK.\n", (m_paramItem.sourceModule).c_str());

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
        m_paramItem.modifiedFile = SYSTEM_OPTIONS_FILE;
    } else if(fTag.compare("customer") == 0){
        m_paramItem.modifiedFile = CUSTOM_OPTIONS_FILE;
    } else if(fTag.compare("tr069") == 0){
        m_paramItem.modifiedFile = MONITOR_OPTIONS_FILE;
    } else if(fTag.compare("version") == 0){
        m_paramItem.modifiedFile = VERSION_OPTIONS_FILE;
    } else {
        m_paramItem.modifiedFile = "";
    }
    settingsLogVerbose("SET: modifiedFile[%s].\n", (m_paramItem.modifiedFile).c_str());

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
        settingsLogError("setParamOldValue m_paramItem.paramName[%s]ERROR.\n", m_paramItem.paramName.c_str());
        return;
    }
    m_paramItem.oldValue = (*m_paramConfigFileMapping).getConfigFileParamValue(m_paramItem.paramName);
    settingsLogVerbose("SET: oldvalue[%s] OK.\n", m_paramItem.oldValue.c_str());

    return ;
}

/**
 *  @Func: initialParamItem
 *  @Param: name, type:: const char *
 *          value, type:: const char *
 *          module, type:: int
 *          fileTag, type:: const char *
 *  @Return: void
 *  
 **/
void SettingModifyRecordKeeping::initialParamItem(const char* name, const char* value, int module, const char* fileTag)
{
    m_paramItem.paramName = name;
    m_paramItem.newValue = value;
    setParamOldValue();
    setModifyTimeStamp();
    setModifySourceModule(module);
    setModifiedFile(fileTag);
    settingsLogVerbose("initialParamItem m_paramItem.paramName[%s] newValue[%s] sourceModule[%s] oldValue[%s] modifyTimeStamp[%s] modifiedFile[%s].\n", 
                    m_paramItem.paramName.c_str(), m_paramItem.newValue.c_str(), (m_paramItem.sourceModule).c_str(), 
                    m_paramItem.oldValue.c_str(), m_paramItem.modifyTimeStamp.c_str(), m_paramItem.modifiedFile.c_str());
    
    return ;
}
















