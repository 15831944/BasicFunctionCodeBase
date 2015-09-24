#ifndef __MAPPING_SETTING_MODIFY_RECORD_H__
#define __MAPPING_SETTING_MODIFY_RECORD_H__

#ifdef __cplusplus
#include <string>
#include <map>

#include "ThreadMutex.h"
#include "KeepingModifyUtilitys.h"
#include "MappingSettingStatistics.h"
 
class MappingSettingModifyRecord { /* 在内存中维护对记录修改的文件中参数的拷贝 */
    public:
        MappingSettingModifyRecord(std::string fileName);
        ~MappingSettingModifyRecord(){}
        
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
        MappingSettingStatistics*      m_paramStatisticsMapping;
        std::string     m_version;
        ThreadMutex_Type m_mutex;
};

#endif  // __cplusplus



#endif //__MAPPING_SETTING_MODIFY_RECORD_H__

