#ifndef __SETTING_MODIFY_RECORD_KEEPING_H__
#define __SETTING_MODIFY_RECORD_KEEPING_H__

typedef enum {
    RecordChangedSource_Reserved = 0,
    RecordChangedSource_STBMonitor,
    RecordChangedSource_tr069Manager,
    RecordChangedSource_JsLocalSetting,
    RecordChangedSource_JsEpgSetting,
    RecordChangedSource_Other,
    RecordChangedSource_Unknow
} RecordChangedSource;

#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>

extern "C" {
#include "mid_mutex.h"
}

#define     COUNT_OF_PARAM_IN_MODIFY_FILE_MAX           10
#define     PARAM_CHANGE_RECORD_KEEPING_FILE_PATH       CONFIG_FILE_DIR"/config_param_modify.record"




typedef struct _ParametersItem {
    std::string     paramName;  //修改的谁
    std::string     oldValue;   //原来的值是什么
    std::string     newValue;   //被修改成什么值
    std::string     modifyTimeStamp;    //什么时间修改的
    std::string     sourceModule;       //谁来修改的
    std::string     modifiedFile;       //被修改的谁是哪个文件中的
} ParametersItem;

 typedef struct _ParamStatistics{
    std::string paramName;          //paramName, 字段名
    std::string earliestTimeStamp;  //earliestTimeStamp, record文件中paramName字段的最早记录
    int         recordCount;        //recordCount, record文件中paramName字段的记录次数
} ParamStatistics;


namespace Hippo {
class ParamStatisticsMapping {
    public:
        ParamStatisticsMapping(std::string fileName);
        ~ParamStatisticsMapping(){}
        
        int loadStatisticsRecord();
        int updateStatisticsRecord(ParametersItem *paramItem);  //添加一条完整的记录，包括paramName，earliestTimeStamp，recordCount
        int unloadStatisticsRecord();
        bool modifyStatisticsRecordTimeStamps(std::string& name, std::string& timeStamp);  //修改paramName对应的earliestTimeStamp
        std::string getStatisticsRecordTimeStamps(std::string& name);
        int modifyStatisticsRecordCount(std::string& name, int IsFlagAdd, int step); //修改paramName对应的recordCount
        int getStatisticsRecordCount(std::string& name);
        std::map<std::string, ParamStatistics> getStatisticsRecordMap();
        int statisticsRecordMapOutput();
        
    private:
        std::map<std::string, ParamStatistics>      m_paramStatisticsMap;   // 维护一个已修改参数的信息映射表，参数信息均来源于 PARAM_CHANGE_RECORD_KEEPING_FILE_PATH 文件中；
        std::string     m_filePath;   
        int m_RecordFileAvailable;
};

class ParamConfigFileMapping { /* 在内存中维护对配置文件中参数的键值对的拷贝 */
    public:
        ParamConfigFileMapping(){}
        ~ParamConfigFileMapping(){}
        
        bool addConfigFile(std::string file);
        bool deleteConfigFile(std::string file);
        int loadConfigFileParam();                                          //将 m_cfgFileList 中配置文件里的键值对加载到 m_paramConfigFileMap
        int unloadConfigFileParam();
        int updateConfigFileParamMap(std::string& name, std::string& value);  //在每次将修改信息写入到 m_fileName 文件后，更新内存中的配置文件拷贝 m_paramConfigFileMap ，
        std::string getConfigFileParamValue(std::string name);
        int paramConfigFileMapOutput();
        
    private:
        std::map<std::string, std::string>  m_paramConfigFileMap;   //保存各个配置文件（包括：yx_config_system.ini，yx_config_customer.ini，yx_config_tr069.ini）中的键值对
        std::vector<std::string>    m_cfgFileList;                  //可能会修改的配置文件列表，在构造时初始化
};

 
 class SensitiveParamFiltering { /* 敏感词过滤 */
    public:
        SensitiveParamFiltering();
        ~SensitiveParamFiltering(){}
        
        bool filteringSensitiveParam(std::string param);    //在构造时初始化敏感字段，在需要时通过该方法剔除敏感字段
        bool addSensitiveParam(std::string& param);
        int  sensitiveParamFilterOutput();
    private:
        std::vector<std::string>  m_sensitiveParamFilter;   //保存需要过滤的敏感字段，在构造操作时初始化
};


class ParamModifyRecordMapping { /* 在内存中维护对记录修改的文件中参数的拷贝 */
    public:
        ParamModifyRecordMapping(std::string fileName);
        ~ParamModifyRecordMapping(){}
        
        int loadModifyRecordParam();        //将文件 m_fileName 中的修改信息加载到 m_paramModifyRecordMap
        int unloadModifyRecordParam();
        int updateModifyRecordParamMap(ParametersItem *paramItem);   //在每次将修改信息写入到 m_fileName 文件后，更新内存中的文件拷贝 m_paramModifyRecordMap ，
        int dereplicationModifyRecordParamMap();
        int getModifyRecordEarliestTimeStamp(std::string& name, std::string& timeStamp);
        int deleteModifyRecordByTimeStamp(std::string& name, std::string& timeStamp);
        int formatingParamItemOutput();
        int resetModifyRecordOutput();
        int paramModifyRecordMapOutput();
        
    private:
        std::string     m_fileName;         //用来保存参数记录的文件名称，即 PARAM_CHANGE_RECORD_KEEPING_FILE_PATH
        std::multimap<std::string, ParametersItem>  m_paramModifyRecordMap;
        ParamStatisticsMapping*      m_paramStatisticsMapping;
        std::string     m_version;
        mid_mutex_t     m_mutex;
};


class SettingModifyRecordKeeping {
    public:
        SettingModifyRecordKeeping(std::string fileName); 
        ~SettingModifyRecordKeeping() {}

        int load(void);
        int save(void);
        int reSet(void);
        int set(const char* name, const char* value, RecordChangedSource module, const char* fileTag);
                
        void setModifyTimeStamp();                              //设置 m_paramItem 的modifyTimeStamp
        void setModifySourceModule(RecordChangedSource module); //设置 m_paramItem 的sourceModule
        void setModifiedFile(const char* fileTag);              //设置 m_paramItem 的modifiedFile
        void setParamOldValue();                                //设置 m_paramItem 的oldValue, 从 m_paramConfigFileMap 中获取
        void initialParamItem(const char* name, const char* value, RecordChangedSource module, const char* fileTag);
        
    private:
        ParametersItem      m_paramItem;
        ParamModifyRecordMapping*    m_paramModifyRecordMapping;
        ParamConfigFileMapping*      m_paramConfigFileMapping;
        bool            dirty;
		std::string 	m_fileName;
       
};

SettingModifyRecordKeeping& settingModifyRecord();

} //
#endif // __cplusplus



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void settingModifyRecordLoad();
void settingModifyRecordSave();
void settingModifyRecordReset(RecordChangedSource module);
void settingModifyRecordSet(const char* name, const char* value, RecordChangedSource module, const char* file);

#ifdef __cplusplus
}
#endif





#endif //__SETTING_MODIFY_RECORD_KEEPING_H__

