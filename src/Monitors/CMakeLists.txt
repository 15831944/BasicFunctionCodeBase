########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND monitor_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logMonitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MonitorInterface.c
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( FTPClient )
add_subdirectory( FTPServer )
add_subdirectory( SystemMonitor )
#add_subdirectory( TinyHttpd )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 monitor 库版本号                         ############## 
########################################################################################
set(monitor_LIB_VERSION     "1.0.0")
set(monitor_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_monitor)
    #生成动态库  静态库则 STATIC  
    add_library (monitorlib SHARED          ${monitor_SRCS})  
    add_library (monitors   STATIC          ${monitor_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (monitorlib  loglib ftpserverlib threadlib pthread)
    add_dependencies (monitors    loglib ftpserverlib threadlib pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (monitorlib  loglib ftpserverlib threadlib pthread)
    target_link_libraries (monitors    loglib ftpserverlib threadlib pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(monitorlib    PROPERTIES 
                                        VERSION     ${monitor_LIB_VERSION} 
                                        SOVERSION   ${monitor_LIB_SOVERSION} )
ELSE (COMPONENT_monitor)
    MESSAGE(STATUS "Not Include COMPONENT_monitor module.")
ENDIF (COMPONENT_monitor)


