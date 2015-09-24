
########################################################################################
#############               生成 ftpserver 库文件                           ############## 
########################################################################################
LIST (APPEND ftpserver_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Errors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NetWorking.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ServerMain.c
    )
    

    
########################################################################################
#############               设置 ftpserver 库版本号                         ############## 
########################################################################################
set(ftpserver_LIB_VERSION     "1.0.0")
set(ftpserver_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Monitors
    ${PROJECT_SOURCE_DIR}/Monitors/FTPServer
    ${PROJECT_SOURCE_DIR}/Monitors/FTPServer/includes
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
IF (MODULE_ftpserver)
    #生成动态库  静态库则 STATIC  
    add_library (ftpserverlib SHARED      ${ftpserver_LIB_SRCS})  
    add_library (ftpservers   STATIC      ${ftpserver_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (ftpserverlib loglib pthread)
    add_dependencies (ftpservers   loglib pthread)
   
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (ftpserverlib  loglib pthread)
    target_link_libraries (ftpservers    loglib pthread)

    # 设置版本号，SOVERSION为API版本号
    set_target_properties(ftpserverlib  PROPERTIES 
                                        VERSION     ${ftpserver_LIB_VERSION} 
                                        SOVERSION   ${ftpserver_LIB_SOVERSION} )
ELSE (MODULE_ftpserver)
    MESSAGE(STATUS "Not Include ftpserver module.")
ENDIF (MODULE_ftpserver)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestFTPServer.elf    ftpserverTest.c)
    add_dependencies(TestFTPServer.elf      loglib monitorlib ftpserverlib pthread)
    target_link_libraries(TestFTPServer.elf loglib monitorlib ftpserverlib pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include ftpserver module.")
ENDIF (TEST_MODULE_FLAG)




