
########################################################################################
#############               生成 ftpclient 库文件                           ############## 
########################################################################################
LIST (APPEND ftpclient_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/FTPClient.c
    #${CMAKE_CURRENT_SOURCE_DIR}/NetWorking.c
    #${CMAKE_CURRENT_SOURCE_DIR}/ServerMain.c
    )
    

    
########################################################################################
#############               设置 ftpclient 库版本号                         ############## 
########################################################################################
set(ftpclient_LIB_VERSION     "1.0.0")
set(ftpclient_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Monitors
    ${PROJECT_SOURCE_DIR}/Monitors/FTPClient
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
IF (MODULE_ftpclient)
    #生成动态库  静态库则 STATIC  
    add_library (ftpclientlib SHARED      ${ftpclient_LIB_SRCS})  
    add_library (ftpclients   STATIC      ${ftpclient_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (ftpclientlib  pthread)
    add_dependencies (ftpclients    pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(ftpclientlib  PROPERTIES 
                                        VERSION     ${ftpclient_LIB_VERSION} 
                                        SOVERSION   ${ftpclient_LIB_SOVERSION} )
ELSE (MODULE_ftpclient)
    MESSAGE(STATUS "Not Include ftpclient module.")
ENDIF (MODULE_ftpclient)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestFTPClient.elf    ftpclientTest.c)
    add_dependencies(TestFTPClient.elf      loglib  monitorlib  ftpclientlib    pthread)
    target_link_libraries(TestFTPClient.elf loglib  monitorlib  ftpclientlib    pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include ftpclient module.")
ENDIF (TEST_MODULE_FLAG)




