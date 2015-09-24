########################################################################################
##########         生成 loglib 库文件                                        ########### 
########################################################################################
LIST (APPEND log_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/LogC.c 
    )
    
    
########################################################################################
#############               设置 loglib 库版本号                          ############## 
########################################################################################
set(log_LIB_VERSION     "1.0.0")
set(log_LIB_SOVERSION   "1")



########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
add_subdirectory( DebugHeap )
add_subdirectory( FunctionsStatistics )
add_subdirectory( StackInfoDebug )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
##########            添加 编译库文件所需的头文件目录                        ########### 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Log   
)  
  
  
########################################################################################
##########            添加 链接库文件目录                                    ########### 
########################################################################################
link_directories(  
    /usr/local/lib   
    )  


########################################################################################
##########            生成目标库文件 并 标明版本号                           ########### 
########################################################################################
IF (COMPONENT_log)
    #生成动态库  静态库则 STATIC  
    add_library (loglib     SHARED          ${log_LIB_SRCS})  
    add_library (logs       STATIC            ${log_LIB_SRCS})  
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(loglib    PROPERTIES 
                                    VERSION     ${log_LIB_VERSION} 
                                    SOVERSION   ${log_LIB_SOVERSION} )
    
ELSE (COMPONENT_log)
    MESSAGE(STATUS "Not Include log module.")
ENDIF (COMPONENT_log)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(Testlog.elf  logTest.c)
    add_dependencies(Testlog.elf        loglib)
    target_link_libraries(Testlog.elf   loglib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)



