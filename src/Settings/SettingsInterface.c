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
#include "Settings.h"
#include "ThreadMutex.h"

#include "sqlite/sqlite3.h"
#include "SettingsInterface.h"
#include "SettingsUtilitySqlite3.h"
#include "KeepingSettingModify/SettingModifyRecordKeepingInterface.h"


#define TABLE_EXIST_FLAG_INTEGER            1
#define TABLE_EXIST_FLAG_VAECHAR            (1<<1)


static sqlite3  *gSettingsDB = NULL;
extern ThreadMutex_Type gSettingsSqlite3Mutex;



static int SettingsBuildTime()
{
    settingsLogInfo("Settings Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}



extern int SqliteConnectDB(const char* dbPath);
extern int SqliteRecordLoading();
/**
 *  name::  InitSettings
 *          初始化各个解码器, 主要是初始化ffmpeg解码器
 *
 **/
int InitSettings(void)
{
    log_settings_init();
    SettingsBuildTime();

    //TODO::
    //      初始化： 互斥量，配置文件读取参数配置，
    gSettingsSqlite3Mutex = ThreadMutexCreate();

    SqliteConnectDB(SQLITE_DATABASE_PATH);
    SqliteRecordLoading();
    //settingModifyRecordLoad();

    
    settingsLogDebug("InitSettings END. \n");
    return 0;
}



int SettingsOptionsGetInt(char* name, int value)
{
    char timestamp[32];  //YYYY-MM-DD HH:MM:SS
    
    settingsLogDebug("SettingsOptionsGetInt. \n");
    SqliteRecordInquiry(name, &value, DATATYPE_INTEGER, timestamp);
    
    return 0;
}


extern void settingModifyRecordSet(const char* name, const char* value, int module, const char* file);
int SettingsOptionsSetInt(char* name, int value)
{
    settingsLogDebug("SettingsOptionsSetInt. \n");
    SqliteRecordModify(name, value, DATATYPE_INTEGER);
    //RecordChangedSource module;
    //char file[] = "system";
    //settingModifyRecordSet((const char*)name, (const char*)value, 2, (const char*)file);
    
    return 0;
}


int SettingsOptionsGetString(char* name, char* value)
{
    char timestamp[32];  //YYYY-MM-DD HH:MM:SS
    
    settingsLogDebug("SettingsOptionsGetString. \n");
    SqliteRecordInquiry(name, value, DATATYPE_VARCHAR, timestamp);
    
    return 0;
}


int SettingsOptionsSetString(char* name, char* value)
{
    settingsLogDebug("SettingsOptionsSetString.\n");
    SqliteRecordModify(name, value, DATATYPE_VARCHAR);
    //RecordChangedSource module;
    //char file[] = "system";
    //settingModifyRecordSet((const char*)name, (const char*)value, 2, (const char*)file);
    
    return 0;
}


int SettingsOptionsSave(char* name, char* value)
{
    settingsLogDebug("SettingsOptionsSave.\n");
    //settingModifyRecordSave();

    return 0;
}


extern void settingModifyRecordReset(int module);
int SettingsOptionsReset(char* name, char* value)
{
    settingsLogDebug("SettingsOptionsReset.\n");
    //settingModifyRecordReset(2);  //RecordChangedSource module

    return 0;
}








