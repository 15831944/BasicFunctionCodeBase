#include <stdio.h>

#include "Log/LogC.h"
#include "logSettings.h"
#include "ThreadMutex.h"



#ifdef __cplusplus

#include "SettingModifyRecordKeepingInterface.h"
#include "SettingModifyRecordKeeping.h"
static SettingModifyRecordKeeping g_settingModifyRecordKeeping(PARAM_CHANGE_RECORD_KEEPING_FILE);

#endif  // __cplusplus
 

 //g_settingModifyRecord
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



static ThreadMutex_Type g_mutex = ThreadMutexCreate( );
void settingModifyRecordLoad()
{
    if (ThreadMutexLock(g_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    g_settingModifyRecordKeeping.load();
    if (ThreadMutexUnLock(g_mutex))
        settingsLogWarning("thread mutex locking error.\n");
    settingsLogVerbose("settingModifyRecordLoad.\n");
    
    return;
}

void settingModifyRecordSave()
{
    settingsLogVerbose("settingModifyRecordSave.\n");
    g_settingModifyRecordKeeping.save();
        
    return;
}

//settingModifyRecordSet("syncntvAESpasswd", sysConfigs.syncntvAESpasswd, (Modify_Mode)whoModify, "system");
void settingModifyRecordSet(const char* name, const char* value, int module, const char* file)
{
    g_settingModifyRecordKeeping.set(name, value, module, file);
    settingsLogVerbose("settingModifyRecordSet Name[%s] value[%s]  module[%d] file[%s].\n", name, value, module, file);
    
    return;
}

void settingModifyRecordReset(int module)
{
    settingsLogVerbose("settingModifyRecordReset.\n");
    g_settingModifyRecordKeeping.reSet();
    
    return;
}



















#ifdef __cplusplus
}
#endif
 
 
 
