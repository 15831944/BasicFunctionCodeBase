#ifndef __SETTINGS_INTERFACE_H__
#define __SETTINGS_INTERFACE_H__
 
 
#define     SQLITE_DATABASE_PATH        "./sqlite_settings.db"
#define     TABLE_NAME_LENGTH           64

 

int InitSettings(void);

int SettingsOptionsGetInt(char* name, int value);

int SettingsOptionsSetInt(char* name, int value);

int SettingsOptionsGetString(char* name, char* value);

int SettingsOptionsSetString(char* name, char* value);

int SettingsOptionsSave(char* name, char* value);

int SettingsOptionsReset(char* name, char* value);










#endif  //__SETTINGS_INTERFACE_H__

