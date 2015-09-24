########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND mediainfo_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logMediaInfo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaInfoInterface.c
    
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
## add_subdirectory( Crypto )
add_subdirectory( MediaInfoLib )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 mediainfo 库版本号                         ############## 
########################################################################################
set(mediainfo_LIB_VERSION     "1.0.0")
set(mediainfo_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_mediainfo)
    #生成动态库  静态库则 STATIC  
    add_library (mediainfolib   SHARED          ${mediainfo_SRCS})  
    add_library (mediainfos     STATIC          ${mediainfo_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (mediainfolib  loglib )
    add_dependencies (mediainfos    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (mediainfolib  loglib )
    target_link_libraries (mediainfos    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(mediainfolib      PROPERTIES 
                                            VERSION     ${mediainfo_LIB_VERSION} 
                                            SOVERSION   ${mediainfo_LIB_SOVERSION} )
ELSE (COMPONENT_mediainfo)
    MESSAGE(STATUS "Not Include COMPONENT_mediainfo module.")
ENDIF (COMPONENT_mediainfo)


