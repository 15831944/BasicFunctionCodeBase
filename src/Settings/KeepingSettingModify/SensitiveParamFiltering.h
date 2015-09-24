#ifndef __SENSITIVE_PARAM_FILTERING_H__
#define __SENSITIVE_PARAM_FILTERING_H__


#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>


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


extern "C" SensitiveParamFiltering gSensitiveParamFilter;



#endif  // __cplusplus



#endif //__SENSITIVE_PARAM_FILTERING_H__

