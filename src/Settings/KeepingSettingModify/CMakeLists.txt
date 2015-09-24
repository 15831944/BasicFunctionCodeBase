
########################################################################################
#############             生成 keeping_setting_modify 库文件              ############## 
########################################################################################
LIST (APPEND modifykeeping_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/KeepingModifyUtilitys.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MappingSettingConfigFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MappingSettingModifyRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MappingSettingStatistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensitiveParamFiltering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingModifyRecordKeeping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingModifyRecordKeepingInterface.cpp
    )
    

    
########################################################################################
#############           设置 keeping_setting_modify 库版本号              ############## 
########################################################################################
set(modifykeeping_LIB_VERSION     "1.0.0")
set(modifykeeping_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Settings
    ${PROJECT_SOURCE_DIR}/Settings/KeepingSettingModify
)  


########################################################################################
#############               添加 链接库文件目录                           ############## 
########################################################################################
link_directories(  
    /usr/local/lib   
    )  


########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (MODULE_modifykeeping)
    #生成动态库  静态库则 STATIC  
    add_library (modifykeepinglib SHARED      ${modifykeeping_LIB_SRCS})  
    add_library (modifykeepings   STATIC      ${modifykeeping_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (modifykeepinglib  loglib threadlib pthread)
    add_dependencies (modifykeepings    loglib threadlib pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (modifykeepinglib  loglib threadlib pthread)
    target_link_libraries (modifykeepings    loglib threadlib pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(modifykeepinglib  PROPERTIES 
                                            VERSION     ${modifykeeping_LIB_VERSION} 
                                            SOVERSION   ${modifykeeping_LIB_SOVERSION} )
ELSE (MODULE_modifykeeping)
    MESSAGE(STATUS "Not Include keeping_setting_modify module.")
ENDIF (MODULE_modifykeeping)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestModifyKeeping.elf    modifykeepingTest.cpp)
    add_dependencies(TestModifyKeeping.elf      loglib settingslib modifykeepinglib threadlib pthread)
    target_link_libraries(TestModifyKeeping.elf loglib settingslib modifykeepinglib threadlib pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include keeping_setting_modify module.")
ENDIF (TEST_MODULE_FLAG)




