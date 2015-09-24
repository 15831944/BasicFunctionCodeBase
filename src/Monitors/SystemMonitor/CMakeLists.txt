
########################################################################################
#############               生成 system_monitor 库文件                           ############## 
########################################################################################
LIST (APPEND system_monitor_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/TopUtility.c
    ##${CMAKE_CURRENT_SOURCE_DIR}/
    )
    

    
########################################################################################
#############               设置 system_monitor 库版本号                         ############## 
########################################################################################
set(system_monitor_LIB_VERSION     "1.0.0")
set(system_monitor_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Monitors
    ${PROJECT_SOURCE_DIR}/Monitors/SystemMonitor
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
IF (MODULE_system_monitor)
    #生成动态库  静态库则 STATIC  
    add_library (system_monitorlib SHARED      ${system_monitor_LIB_SRCS})  
    add_library (system_monitors   STATIC      ${system_monitor_LIB_SRCS})  
        
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(system_monitorlib PROPERTIES 
                                            VERSION     ${system_monitor_LIB_VERSION} 
                                            SOVERSION   ${system_monitor_LIB_SOVERSION} )
ELSE (MODULE_system_monitor)
    MESSAGE(STATUS "Not Include system_monitor module.")
ENDIF (MODULE_system_monitor)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestSystemMonitor.elf    system_monitorTest.c)
    add_dependencies(TestSystemMonitor.elf      loglib monitorlib system_monitorlib)
    target_link_libraries(TestSystemMonitor.elf loglib monitorlib system_monitorlib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include system_monitor module.")
ENDIF (TEST_MODULE_FLAG)




