########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND utils_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/logUtils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/UtilsInterface.c
    )


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
## add_subdirectory( Crypto )
add_subdirectory( ParserIni )
add_subdirectory( ParserJsonCPP )
add_subdirectory( ParserTinyXML2 )
add_subdirectory( Sqlite3 )

MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 utils 库版本号                         ############## 
########################################################################################
set(utils_LIB_VERSION     "1.0.0")
set(utils_LIB_SOVERSION   "1")



########################################################################################
#############           生成目标库文件 并 标明版本号                      ############## 
########################################################################################
IF (COMPONENT_utils)
    #生成动态库  静态库则 STATIC  
    add_library (utilslib   SHARED          ${utils_SRCS})  
    add_library (utilss     STATIC          ${utils_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (utilslib  loglib )
    add_dependencies (utilss    loglib )
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (utilslib  loglib )
    target_link_libraries (utilss    loglib )
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(utilslib  PROPERTIES 
                                    VERSION     ${utils_LIB_VERSION} 
                                    SOVERSION   ${utils_LIB_SOVERSION} )
ELSE (COMPONENT_utils)
    MESSAGE(STATUS "Not Include COMPONENT_utils module.")
ENDIF (COMPONENT_utils)


