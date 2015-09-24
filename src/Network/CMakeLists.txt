########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND network_LIB_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logNetwork.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NetworkInterface.c
    )

    

########################################################################################
#############                      添加源码子目录                         ############## 
########################################################################################

#####  Net Basic libs   #####
#add_subdirectory( adns )
#add_subdirectory( dns-own )
#add_subdirectory( ftpclient-own )
#add_subdirectory( ftp-own )
#add_subdirectory( simple-ftp )
#add_subdirectory( sntp )
#add_subdirectory( sntp-own )
#add_subdirectory( tiny-tcp )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")




########################################################################################
#############               设置 network 库版本号                         ############## 
########################################################################################
set(network_LIB_VERSION     "1.0.0")
set(network_LIB_SOVERSION   "1")


    
########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_network)
    #生成动态库  静态库则 STATIC  
    add_library (networklib  SHARED          ${network_LIB_SRCS})  
    add_library (networks    STATIC          ${network_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (networklib  loglib )
    add_dependencies (networks    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (networklib  loglib )
    target_link_libraries (networks    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(networklib    PROPERTIES 
                                        VERSION     ${network_LIB_VERSION} 
                                        SOVERSION   ${network_LIB_SOVERSION} )
ELSE (COMPONENT_network)
    MESSAGE(STATUS "Not Include network module.")
ENDIF (COMPONENT_network)




