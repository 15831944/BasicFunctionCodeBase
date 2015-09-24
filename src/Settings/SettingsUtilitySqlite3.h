#ifndef __SETTINGS_UTILITY_SQLITE3_H__
#define __SETTINGS_UTILITY_SQLITE3_H__


#define SQLITE_DB_TABLE_NAME_INT        "IntegerSetting_Table"
#define SQLITE_DB_TABLE_NAME_CHAR       "VarcharSetting_Table"

//ID NAME  VALUE  MDATE
#define COLUM_1_NAME                "ID"
#define COLUM_1_TYPE                "INTEGER"

#define COLUM_2_NAME                "Name"
#define COLUM_2_TYPE                "VARCHAR(32)"

#define COLUM_3_NAME                "Value"
#define COLUM_3_TYPE_1              "INTEGER"
#define COLUM_3_TYPE_2              "VARCHAR(128)"

#define COLUM_4_NAME                "MDATE"
#define COLUM_4_TYPE                "TimeStamp"

 
 enum _SqliteTableDataType{
    DATATYPE_REAERVED,
    DATATYPE_INTEGER,
    DATATYPE_VARCHAR,
    DATATYPE_MAX
};


/**
 *  创建数据表
 *  清空数据表
 *  添加记录
 *  删除记录
 *  修改记录
 *  查询记录
 *  
 *  根据所存储的数据类型，创建2张表；
 *      INT （或INTEGER）数据类型，存储从-2的31次方 （-2 ，147 ，483 ，648） 到2的31次方-1 （2 ，147 ，483，647） 之间的所有正负整数。
 *                  每个INT 类型的数据按4 个字节存储，其中1 位表示整数值的正负号，其它31 位表示整数值的长度和大小。
 *      VARCHAR数据类型，定义形式为VARCHAR [ （n） ]。 它与CHAR 类型相似，n 的取值也为1 到8000， 
 *                  若输入的数据过长，将会截掉其超出部分。不同的是，VARCHAR数据类型具有变动长度的特性，因为VARCHAR数据类型的存储长度为实际数值长度，
 *                  若输入数据的字符数小于n ，则系统不会在其后添加空格来填满设定好的空间。
 *                  一般情况下，由于CHAR 数据类型长度固定，因此它比VARCHAR 类型的处理速度快。
 *  
 *  表的格式：
 *  ID----Name----Value(int/varchar)----ModifyDate
 *  
 **/


int SqliteConnectDB(const char* dbPath);
int SqliteTableClose();
int SqliteTableCreate(int flag);
int SqliteTableClear(int flag);

int SqliteRecordAdd(char* name, void* value, int valueType);
int SqliteRecordDelete(char* name, void* value, int valueType);
int SqliteRecordModify(char* name, void* value, int valueType);
int SqliteRecordInquiry(char* name, void* value, int valueType, char* timestamp);
int SqliteRecordLoading();
 










#endif  //__SETTINGS_UTILITY_SQLITE3_H__

