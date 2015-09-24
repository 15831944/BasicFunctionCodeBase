/**
 *  InitPlayer.c文件
 *  负责初始化Play播放相关的结构体，主要是解码器跟上层控制相关的结构；
 *  主要有以下部分：
 *          初始化
 *          创建消息循环
 *          创建解码循环
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"

#include "SettingsUtilitySqlite3.h"
#include "sqlite/sqlite3.h"


#define TABLE_EXIST_FLAG_INTEGER            1
#define TABLE_EXIST_FLAG_VAECHAR            (1<<1)

static sqlite3  *gSettingsDB = NULL;
ThreadMutex_Type     gSettingsSqlite3Mutex = NULL;


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
 *  ID----Name----Value(int/varchar)----MDate
 *  
 **/



//打开数据库
int SqliteConnectDB(const char* dbPath)
{
    int result;
    char sqlBuf[128] = {0};
    char *errmsg = NULL;
    char **ret;
    int row, column;
    int flag = 0;
    
    settingsLogDebug("ConnectDB. \n");

    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    result = sqlite3_open( dbPath, &gSettingsDB );
    if ( result != SQLITE_OK ) { //数据库打开失败
        settingsLogError("sqlite3_open return value[%d].\n", result);
        return -1;
    }
    
    sprintf(sqlBuf, "SELECT * FROM %s ;", SQLITE_DB_TABLE_NAME_INT);
    sqlite3_get_table(gSettingsDB, sqlBuf, &ret, &row, &column, &errmsg);
    settingsLogInfo("row[%d], column[%d]. \n", row, column);
    if(column != 4){
        flag = flag | TABLE_EXIST_FLAG_INTEGER;
    }
    
    sprintf(sqlBuf, "SELECT * FROM %s ;", SQLITE_DB_TABLE_NAME_CHAR);
    sqlite3_get_table(gSettingsDB, sqlBuf, &ret, &row, &column, &errmsg);
    settingsLogInfo("row[%d], column[%d]. \n", row, column);
    if(column != 4)
        flag = flag | TABLE_EXIST_FLAG_VAECHAR;
    
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");

    if(flag != 0){
        settingsLogInfo("flag[%d]. \n", flag);
        SqliteTableClear(flag);  //deleteTable
        SqliteTableCreate(flag);  //createTable
    }
    
    settingsLogDebug("ConnectDB END. \n");
    return 0;
}



//关闭数据库
int SqliteTableClose()
{
    settingsLogDebug("SqliteTableClose. \n");
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");    

    sqlite3_close( gSettingsDB );
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    return 0;
}



//创建新表
//createtable tabname(col1 type1 [not null][primary key],col2 type2 [not null],..)
int SqliteTableCreate(int flag)
{
    int result;
    char sqlBuf[512] = {0};
    char *errmsg = NULL;
    
    settingsLogDebug("SqliteTableCreate. \n");
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    if((flag & 1) == TABLE_EXIST_FLAG_INTEGER) {
        //创建表，有4个字段： ID, Name, Value, MDate。其中ID是一个自动增加的类型，可以不去指定这个字段，它会自己从0开始增加
        sprintf(sqlBuf, "CREATE TABLE %s( \
                        %s %s PRIMARY KEY AUTOINCREMENT, \
                        %s %s NOT NULL, \
                        %s %s NOT NULL, \
                        %s %s NOT NULL DEFAULT (datetime('now', 'localtime')));", SQLITE_DB_TABLE_NAME_INT, 
                COLUM_1_NAME, COLUM_1_TYPE, COLUM_2_NAME, COLUM_2_TYPE, COLUM_3_NAME, COLUM_3_TYPE_1, COLUM_4_NAME, COLUM_4_TYPE);
        result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
        if (result != SQLITE_OK ) {
            settingsLogWarning( "创建表失败，错误码:%d，错误原因:%s\n", result, errmsg );
        }
    }
    
    if((flag & 2) == TABLE_EXIST_FLAG_VAECHAR) {
        //创建表，有4个字段： ID, Name, Value, MDate。其中ID是一个自动增加的类型，可以不去指定这个字段，它会自己从0开始增加
        sprintf(sqlBuf, "CREATE TABLE %s( \
                        %s %s PRIMARY KEY AUTOINCREMENT, \
                        %s %s NOT NULL, \
                        %s %s NOT NULL, \
                        %s %s NOT NULL DEFAULT (datetime('now', 'localtime')));", SQLITE_DB_TABLE_NAME_CHAR, 
                COLUM_1_NAME, COLUM_1_TYPE, COLUM_2_NAME, COLUM_2_TYPE, COLUM_3_NAME, COLUM_3_TYPE_1, COLUM_4_NAME, COLUM_4_TYPE);
        result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
        if (result != SQLITE_OK ) {
            settingsLogWarning( "创建表失败，错误码:%d，错误原因:%s\n", result, errmsg );
        }
    }
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    settingsLogDebug("SqliteTableCreate END.\n");
    return 0;
}
 
//清空数据库,删除表
int SqliteTableClear(int flag)
{
    int result;
    char sqlBuf[128] = {0};
    char *errmsg = NULL;
    
    settingsLogDebug("SqliteTableClear. \n");
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");    
    if((flag & 1) == TABLE_EXIST_FLAG_INTEGER) {
        sprintf(sqlBuf, "DROP TABLE %s;", SQLITE_DB_TABLE_NAME_INT);
        result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
        if (result != SQLITE_OK ) {
            settingsLogWarning( "创建表失败，错误码:%d，错误原因:%s\n", result, errmsg );
        }
    }

    if((flag & 2) == TABLE_EXIST_FLAG_VAECHAR) {
        sprintf(sqlBuf, "DROP TABLE %s;", SQLITE_DB_TABLE_NAME_CHAR);
        result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
        if (result != SQLITE_OK ) {
            settingsLogWarning( "创建表失败，错误码:%d，错误原因:%s\n", result, errmsg );
        }
    }
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    settingsLogDebug("SqliteTableClear END. \n");
    return 0;
}



//添加记录
int SqliteRecordAdd(char* name, void* value, int valueType)
{
    int result;
    char *errmsg = NULL;
    char sqlBuf[128] = {0};
    
    settingsLogDebug("SqliteRecordAdd. \n");
    
    switch(valueType){
    case DATATYPE_INTEGER:{
        settingsLogDebug("DATATYPE_INTEGER. \n");
        int* tmpValue = (int*)value;
        sprintf(sqlBuf, "INSERT INTO %s( NAME, VALUE ) VALUES('%s', '%d');", SQLITE_DB_TABLE_NAME_INT, name, *tmpValue);
        break;
        }
    case DATATYPE_VARCHAR:{
        settingsLogDebug("DATATYPE_VARCHAR. \n");
        sprintf(sqlBuf, "INSERT INTO %s( NAME, VALUE ) VALUES('%s', '%s');", SQLITE_DB_TABLE_NAME_CHAR, name, value);
        break;
        }
    default:{
        settingsLogWarning("valueType is [%d] error.\n", valueType);
        break;   
        }    
    }
    
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
    if (result != SQLITE_OK ) {
        settingsLogWarning( "插入记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }
    
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    return 0;
}
 
//删除记录
//deletefrom table1 where 范围
int SqliteRecordDelete(char* name, void* value, int valueType)
{
    int result;
    char *errmsg = NULL;
    char sqlBuf[128] = {0};
    
    settingsLogDebug("SqliteRecordDelete. \n");
    
    switch(valueType){
    case DATATYPE_INTEGER:{
        settingsLogDebug("DATATYPE_INTEGER. \n");
        sprintf(sqlBuf, "DELETE FROM %s WHERE Name='%s';", SQLITE_DB_TABLE_NAME_INT, name);
        break;
        }
    case DATATYPE_VARCHAR:{
        settingsLogDebug("DATATYPE_VARCHAR. \n");
        sprintf(sqlBuf, "DELETE FROM %s WHERE Name='%s';", SQLITE_DB_TABLE_NAME_CHAR, name);
        break;
        }
    default:{
        settingsLogWarning("valueType is [%d] error.\n", valueType);
        break;   
        }    
    }
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
    if (result != SQLITE_OK ) {
        settingsLogWarning( "删除记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }
    
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    return 0;
}
 
//修改记录
    //update table1 set field1=value1 where 范围
int SqliteRecordModify(char* name, void* value, int valueType)
{
    int result;
    char *errmsg = NULL;
    char sqlBuf[128] = {0};
    
    settingsLogDebug("SqliteRecordModify. \n");
    
    switch(valueType){
    case DATATYPE_INTEGER:{
        settingsLogDebug("DATATYPE_INTEGER. \n");
        int* tmpValue = (int*)value;
        sprintf(sqlBuf, "UPDATE %s SET Value=%d WHERE Name=%s;", SQLITE_DB_TABLE_NAME_INT, *tmpValue, name);
        break;
        }
    case DATATYPE_VARCHAR:{
        settingsLogDebug("DATATYPE_VARCHAR. \n");
        sprintf(sqlBuf, "UPDATE %s SET Value=%s WHERE Name=%s;", SQLITE_DB_TABLE_NAME_CHAR, value, name);
        break;
        }
    default:{
        settingsLogWarning("valueType is [%d] error.\n", valueType);
        break;   
        }    
    }
    
    
    if (ThreadMutexLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    result = sqlite3_exec( gSettingsDB, sqlBuf, NULL, NULL, &errmsg );
    if (result != SQLITE_OK ) {
        settingsLogWarning( "更新记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }
    
    
    if (ThreadMutexUnLock(gSettingsSqlite3Mutex))
        settingsLogWarning("thread mutex locking error.\n");
    
    return 0;
}
 
//查询记录
//查询语句：select * from 表名 where 条件子句 group by 分组字句 having ... order by 排序子句
int SqliteRecordInquiry(char* name, void* value, int valueType, char* timestamp)
{
    int result;
    char *errmsg = NULL;
    char sqlBuf[128] = {0};
    char **ret;//二维数组用于存放结果
    int row, column;
    int i = 0;
    
    settingsLogDebug("SqliteRecordInquiry. \n");
    //value = (void*)malloc(sizeof(int));
    
    //int sqlite3_get_table(sqlite3 *,const char *sql,char ***result,int *nrow,int *ncolumn,char **errmsg);
    //ret：以数组的形式存放所要查询的数据，首先是表名，然后才是数据;nrow和ncolumn分别用于记录查询语句返回的结果的行数和列数，没有查到结果是返回0

    //ID NAME  VALUE  MDATE
    switch(valueType){
    case DATATYPE_INTEGER:{
        settingsLogDebug("DATATYPE_INTEGER. \n");
        sprintf(sqlBuf, "SELECT * FROM %s WHERE Name=%s;", SQLITE_DB_TABLE_NAME_INT, name);
        sqlite3_get_table(gSettingsDB, sqlBuf, &ret, &row, &column, &errmsg);
        
        if(value != NULL)
            ;//(int)*value = atoi(ret[2]);
        
        if(timestamp != NULL)
            timestamp = ret[3];
        
        break;
        }
    case DATATYPE_VARCHAR:{
        settingsLogDebug("DATATYPE_VARCHAR. \n");
        sprintf(sqlBuf, "SELECT * FROM %s WHERE Name=%s;", SQLITE_DB_TABLE_NAME_CHAR, name);
        sqlite3_get_table(gSettingsDB, sqlBuf, &ret, &row, &column, &errmsg);
        
        if(value != NULL)
            value = (void*)ret[2];
        
        if(timestamp != NULL)
            timestamp = ret[3];
        
        break;
        }
    default:{
        settingsLogWarning("valueType is [%d] error.\n", valueType);
        break;   
        }    
    }
    
    //输出查询的数据
    for(i = 0; i < (row + 1) * column; i++)
        printf("result[%d]=%s\n", i, ret[i]);
        

    return 0;
}



//sqlite 每查到一条记录，就调用一次这个回调  
int LoadInfo(void *para, int n_column, char **column_value, char **column_name)  
{      
    int i;
    printf("%=记录包含%d个字段\n",n_column);  
      
    for(i = 0; i < n_column; i++) {  
        printf("字段名: %s ,字段值:%s\n",column_name[i],column_value[i]);  
    }  
    
    printf("\n");  
      
    return 0;  
}  

int SqliteRecordLoading()
{
    int result;
    char *errmsg = NULL;
    char sqlBuf[128] = {0};
    
    settingsLogDebug("SqliteRecordLoading. \n");
    
    //查询数据库  
    sprintf(sqlBuf, "SELECT * FROM %s;", SQLITE_DB_TABLE_NAME_INT);  
    result = sqlite3_exec(gSettingsDB, sqlBuf, LoadInfo, NULL, &errmsg);  
    
    sprintf(sqlBuf, "SELECT * FROM %s;", SQLITE_DB_TABLE_NAME_CHAR);  
    result = sqlite3_exec(gSettingsDB, sqlBuf, LoadInfo, NULL, &errmsg);  
    
    return 0;
}













