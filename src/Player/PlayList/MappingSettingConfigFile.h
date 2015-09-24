#ifndef __MAPPING_SETTING_CONFIG_FILE_H__
#define __MAPPING_SETTING_CONFIG_FILE_H__

#ifdef __cplusplus
#include <string>
#include <vector>
#include <map>

#define     PARAM_CHANGE_RECORD_KEEPING_FILE_PATH       "./config_param_modify.record"


class MappingSettingConfigFile { /* 在内存中维护对配置文件中参数的键值对的拷贝 */
    public:
        MappingSettingConfigFile(){}
        ~MappingSettingConfigFile(){}
        
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




#endif  // __cplusplus






#endif //__MAPPING_SETTING_CONFIG_FILE_H__

