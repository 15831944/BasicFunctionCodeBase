#ifndef __SETTING_MODIFY_RECORD_KEEPING_H__
#define __SETTING_MODIFY_RECORD_KEEPING_H__

#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>

#include "KeepingModifyUtilitys.h"
#include "MappingSettingConfigFile.h"
#include "MappingSettingModifyRecord.h"

#define     COUNT_OF_PARAM_IN_MODIFY_FILE_MAX           10
#define     PARAM_CHANGE_RECORD_KEEPING_FILE        "./config_param_modify.record"

#define     SYSTEM_OPTIONS_FILE             "./yx_config_system.ini"
#define     CUSTOM_OPTIONS_FILE             "./yx_config_customer.ini"
#define     MONITOR_OPTIONS_FILE            "./yx_config_tr069.ini"
#define     VERSION_OPTIONS_FILE            "./yx_config_version.ini"



class SettingModifyRecordKeeping {
    public:
        SettingModifyRecordKeeping(std::string fileName); 
        ~SettingModifyRecordKeeping() {}

        int load(void);
        int save(void);
        int reSet(void);
        int set(const char* name, const char* value, int module, const char* fileTag);
                
        void setModifyTimeStamp();                              //设置 m_paramItem 的modifyTimeStamp
        void setModifySourceModule(int module); //设置 m_paramItem 的sourceModule
        void setModifiedFile(const char* fileTag);              //设置 m_paramItem 的modifiedFile
        void setParamOldValue();                                //设置 m_paramItem 的oldValue, 从 m_paramConfigFileMap 中获取
        void initialParamItem(const char* name, const char* value, int module, const char* fileTag);
        
    private:
        ParametersItem      m_paramItem;
        MappingSettingModifyRecord*    m_paramModifyRecordMapping;
        MappingSettingConfigFile*      m_paramConfigFileMapping;
		std::string 	m_fileName;
       
};

#endif  // __cplusplus



#endif //__SETTING_MODIFY_RECORD_KEEPING_H__

