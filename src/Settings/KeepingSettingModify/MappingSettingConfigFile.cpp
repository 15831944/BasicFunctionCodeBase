#include <algorithm>
#include <iostream>

#include <stdio.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "MappingSettingConfigFile.h"
#include "SensitiveParamFiltering.h"

 
using namespace std; 


/**
 *  @Func: addConfigFile
 *  @Param: file, type:: std::string
 *  @Return: bool
 *  
 **/
bool MappingSettingConfigFile::addConfigFile(std::string file)
{
    if(file.empty()){
        settingsLogError("file[%s] cannot empty, ERROR.\n", file.c_str());
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
bool MappingSettingConfigFile::deleteConfigFile(std::string file)
{
    if(file.empty()){
        settingsLogError("file[%s] cannot empty, ERROR.\n", file.c_str());
        return false;
    }
    m_cfgFileList.erase(std::find(m_cfgFileList.begin(), m_cfgFileList.end(), file));
    
    return true;
}


/**
 *  @Func: loadConfigFileParam
 *         ps. : 从 m_cfgFileList 文件列表中包含的文件里加载到 m_paramConfigFileMap
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingConfigFile::loadConfigFileParam()
{
    FILE *fp = NULL;
    char ch;
    std::string line("");
    std::string tag("");
    std::string value("");
    std::vector<std::string>::iterator cfgFileListIt;
    std::string::size_type position;
    
    
    for (cfgFileListIt = m_cfgFileList.begin(); cfgFileListIt != m_cfgFileList.end(); ++cfgFileListIt){
        settingsLogVerbose("I will load \"%s\".\n", (*cfgFileListIt).c_str());
        fp = fopen((*cfgFileListIt).c_str(), "rb");
        if (!fp) {
            settingsLogError("Open file error!\n");
            return -1;
        }
        while((ch = fgetc(fp)) != (char)EOF || !line.empty()) {
            if(ch != '\n' && ch != (char)EOF) {
                line += ch;
                continue;
            }
            position = line.find('=');
            if (position == std::string::npos) {
                settingsLogWarning("Not found \"=\", line=[%s]\n", line.c_str());
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
        settingsLogVerbose("load \"%s\" ok.\n", (*cfgFileListIt).c_str());
    }
    settingsLogVerbose("Has load ConfigFile OK.\n");
    
    return 0;
}     


/**
 *  @Func: updateConfigFileParamMap
 *         ps. : 将解析到的 添加到 m_paramConfigFileMap
 *  @Param: name, type:: std::string&
 *          value, type:: std::string&
 *  @Return: int
 *  
 **/
int MappingSettingConfigFile::updateConfigFileParamMap(std::string& name, std::string& value)
{
    if(name.empty()){
        settingsLogError("name[%s]  is NULL.\n", name.c_str());
        return -1;
    }
    std::map<std::string, std::string>::iterator cfgFileMapIt = m_paramConfigFileMap.find(name);
    
    settingsLogVerbose("INPUT: name[%s] value[%s].\n", name.c_str(), value.c_str());
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
    settingsLogVerbose("Has update OK.\n");
    
    return 0;
}     


/**
 *  @Func: unloadConfigFileParam
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingConfigFile::unloadConfigFileParam()
{
    if(m_paramConfigFileMap.empty()) {
        settingsLogError("unloadConfigFileParam:: m_paramConfigFileMap has been empty.\n");
        return -1;
    }
    m_paramConfigFileMap.erase(m_paramConfigFileMap.begin(), m_paramConfigFileMap.end()); //成片删除要注意的是，也是STL的特性，删除区间是一个前闭后开的集合 
    settingsLogVerbose("Has unload ConfigFile OK.\n");

    return 0;
}


/**
 *  @Func: getConfigFileParamValue
 *         ps. :从 m_paramConfigFileMap 获取参数的旧值
 *  @Param: name, type:: std::string&
 *  @Return: std::string
 *  
 **/
std::string MappingSettingConfigFile::getConfigFileParamValue(std::string name)
{
    std::map<std::string, std::string>::iterator cfgFileMapIt;
    std::string value("");
    
    cfgFileMapIt = m_paramConfigFileMap.find(name);
    if (cfgFileMapIt != m_paramConfigFileMap.end())
        value = cfgFileMapIt->second;
    settingsLogVerbose("GET: name[%s] value[%s].\n", name.c_str(), value.c_str());

    return value;
}


/**
 *  @Func: paramConfigFileMapOutput
 *  @Param: 
 *  @Return: int
 *  
 **/
int MappingSettingConfigFile::paramConfigFileMapOutput()
{
    std::map<std::string, std::string>::iterator cfgFileMapIt;
    
    for(cfgFileMapIt = m_paramConfigFileMap.begin();cfgFileMapIt != m_paramConfigFileMap.end(); ++cfgFileMapIt){
        cout << "paramConfigFileMapOutput:: key: "<< cfgFileMapIt->first << " value: " << cfgFileMapIt->second <<endl;
    }
    
    return   0;
}


 














