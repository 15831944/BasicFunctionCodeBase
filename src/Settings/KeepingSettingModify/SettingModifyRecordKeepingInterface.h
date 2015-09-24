#ifndef __SETTING_MODIFY_RECORD_KEEPING_INTERFACE_H__
#define __SETTING_MODIFY_RECORD_KEEPING_INTERFACE_H__



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


void settingModifyRecordLoad();
void settingModifyRecordSave();
void settingModifyRecordReset(int module);
void settingModifyRecordSet(const char* name, const char* value, int module, const char* file);





#ifdef __cplusplus
}
#endif
 




#endif //__SETTING_MODIFY_RECORD_KEEPING_INTERFACE_H__

