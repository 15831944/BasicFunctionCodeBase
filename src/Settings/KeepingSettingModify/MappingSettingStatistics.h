#ifndef __MAPPING_SETTING_STATISTICS_H__
#define __MAPPING_SETTING_STATISTICS_H__

#ifdef __cplusplus
#include <string>
#include <map>

#include "KeepingModifyUtilitys.h"

#define STATISTAC_RECORD_COUNT_FLAG_ADD         1
#define STATISTAC_RECORD_COUNT_FLAG_MINUS       0


class MappingSettingStatistics {
    public:
        MappingSettingStatistics(std::string fileName);
        ~MappingSettingStatistics(){}
        
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





#endif  // __cplusplus




#endif //__MAPPING_SETTING_STATISTICS_H__

