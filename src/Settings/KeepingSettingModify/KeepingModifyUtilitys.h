#ifndef __KEEPING_MODIFY_UTILITY_H__
#define __KEEPING_MODIFY_UTILITY_H__

#ifdef __cplusplus

#include <string>

#define     COUNT_OF_PARAM_IN_MODIFY_FILE_MAX           10
#define     PARAM_CHANGE_RECORD_KEEPING_VERSION     "1.0"
#define     STRING_ERROR    "string error"


typedef struct _DateTimeStamp {
    uint16_t mYear;          //!< e.g. 2005
    uint8_t  mMonth;         //!< 1..12
    uint8_t  mDayOfWeek;     //!< 0..6, 0==Sunday
    uint8_t  mDay;           //!< 1..31
    uint8_t  mHour;          //!< 0..23
    uint8_t  mMinute;        //!< 0..59
    uint8_t  mSecond;        //!< 0..59
} DateTimeStamp;
    
void getDateTimeStamp(DateTimeStamp* dt);



typedef enum {
    RecordChangedSource_Reserved = 0,
    RecordChangedSource_STBMonitor,
    RecordChangedSource_tr069Manager,
    RecordChangedSource_JsLocalSetting,
    RecordChangedSource_JsEpgSetting,
    RecordChangedSource_Other,
    RecordChangedSource_Unknow
} RecordChangedSource;

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

extern "C" ParametersItem ERROR_PARAM_ITEM;
extern "C" std::string gMonthStr[12 + 1];

int dateTimeCompare(std::string& strBaseDTime, std::string& strNowDTime);
std::string getValueByTag(std::string& str, const char *tag);
ParametersItem* parseRecordLine(std::string& strLine);



#endif  // __cplusplus



#endif //__KEEPING_MODIFY_UTILITY_H__

