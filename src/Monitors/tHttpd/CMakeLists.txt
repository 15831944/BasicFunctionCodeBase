
########################################################################################
#############               生成 tinyhttpd 库文件                           ############## 
########################################################################################
LIST (APPEND tinyhttpd_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/TinyHttpd.c
    ##${CMAKE_CURRENT_SOURCE_DIR}/
    )
    

    
########################################################################################
#############               设置 tinyhttpd 库版本号                         ############## 
########################################################################################
set(tinyhttpd_LIB_VERSION     "1.0.0")
set(tinyhttpd_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Monitors
    ${PROJECT_SOURCE_DIR}/Monitors/TinyHttpd
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
IF (MODULE_tinyhttpd)
    #生成动态库  静态库则 STATIC  
    add_library (tinyhttpdlib SHARED      ${tinyhttpd_LIB_SRCS})  
    add_library (tinyhttpds   STATIC      ${tinyhttpd_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (tinyhttpdlib  loglib threadlib pthread)
    add_dependencies (tinyhttpds    loglib threadlib pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (tinyhttpdlib  loglib threadlib pthread)
    target_link_libraries (tinyhttpds    loglib threadlib pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(tinyhttpdlib  PROPERTIES 
                                        VERSION     ${tinyhttpd_LIB_VERSION} 
                                        SOVERSION   ${tinyhttpd_LIB_SOVERSION} )
ELSE (MODULE_tinyhttpd)
    MESSAGE(STATUS "Not Include tinyhttpd module.")
ENDIF (MODULE_tinyhttpd)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestSystemMonitor.elf    tinyhttpdTest.c)
    add_dependencies(TestSystemMonitor.elf      loglib monitorlib threadlib pthread tinyhttpdlib)
    target_link_libraries(TestSystemMonitor.elf loglib monitorlib threadlib pthread tinyhttpdlib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include tinyhttpd module.")
ENDIF (TEST_MODULE_FLAG)




