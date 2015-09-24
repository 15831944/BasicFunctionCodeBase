########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND settings_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logSettings.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsUtilitySqlite3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsUtilityIniText.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsInterface.c
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( Hash )
#add_subdirectory( MappingTree )
add_subdirectory( KeepingSettingModify )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 settings 库版本号                         ############## 
########################################################################################
set(settings_LIB_VERSION     "1.0.0")
set(settings_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_settings)
    #生成动态库  静态库则 STATIC  
    add_library (settingslib    SHARED          ${settings_SRCS})  
    add_library (settingss      STATIC          ${settings_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (settingslib  loglib sqlite3lib threadlib dl rt)
    add_dependencies (settingss    loglib sqlite3lib threadlib dl rt)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (settingslib  loglib sqlite3lib threadlib dl rt)
    target_link_libraries (settingss    loglib sqlite3lib threadlib dl rt)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(settingslib   PROPERTIES 
                                        VERSION         ${settings_LIB_VERSION} 
                                        SOVERSION       ${settings_LIB_SOVERSION} )
ELSE (COMPONENT_settings)
    MESSAGE(STATUS "Not Include COMPONENT_settings module.")
ENDIF (COMPONENT_settings)


