########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND xmpp_Libs_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logXmppIM.c
    ${CMAKE_CURRENT_SOURCE_DIR}/XmppIMInterface.c
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( tests )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 gloox 库版本号                         ############## 
########################################################################################
set(xmpp_LIB_VERSION     "1.0.0")
set(xmpp_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (MODULE_xmpp)
    #生成动态库  静态库则 STATIC  
    add_library (xmpplib   SHARED          ${xmpp_Libs_SRCS})  
    add_library (xmpps     STATIC          ${xmpp_Libs_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (xmpplib  loglib )
    add_dependencies (xmpps    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (xmpplib  loglib )
    target_link_libraries (xmpps    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(xmpplib  PROPERTIES 
                                    VERSION     ${xmpp_LIB_VERSION} 
                                    SOVERSION   ${xmpp_LIB_SOVERSION} )
ELSE (MODULE_xmpp)
    MESSAGE(STATUS "Not Include MODULE_xmpp module.")
ENDIF (MODULE_xmpp)


