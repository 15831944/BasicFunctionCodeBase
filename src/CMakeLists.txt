########################################################################################
#############           设置可执行文件名称                                ############## 
########################################################################################
set( TARGET_NAME myplayer.elf )



########################################################################################
#######     生成build_info.h头文件  准备在编译链接时所需的第三方库和头文件      ######## 
########################################################################################
include (${BUILD_CMAKE_PATH}/PlatformPrepare_ArchSDK.cmake)



########################################################################################
##########         添加源文件所需要的头文件目录，指定头文件搜寻目录          ########### 
########################################################################################
include_directories(
	${SRC_INCLUDE_PATH}
    ${SRC_INCLUDE_PATH}/ffmpeg
    ${SRC_INCLUDE_PATH}/SDL2/include
    
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Log
    ${PROJECT_SOURCE_DIR}/Messages
    ${PROJECT_SOURCE_DIR}/Threads
    ${PROJECT_SOURCE_DIR}/Player

    ${PROJECT_SOURCE_DIR}/Monitors
    ${PROJECT_SOURCE_DIR}/Monitors/FTPClient
    ${PROJECT_SOURCE_DIR}/Monitors/FTPServer
    ${PROJECT_SOURCE_DIR}/Monitors/FTPServer/includes
    
    ${PROJECT_SOURCE_DIR}/Settings
    ${PROJECT_SOURCE_DIR}/Crypto
    ${PROJECT_SOURCE_DIR}/Network
    
    ${PROJECT_SOURCE_DIR}/Utils/
    ${PROJECT_SOURCE_DIR}/Utils/ParserIni
    ${PROJECT_SOURCE_DIR}/Utils/ParserJsonCPP
    ${PROJECT_SOURCE_DIR}/Utils/ParserTinyXML2
    ${PROJECT_SOURCE_DIR}/Utils/Sqlite3
    
    ${PROJECT_SOURCE_DIR}/Views
    
    ${PROJECT_SOURCE_DIR}/Xmpp_IM

)



########################################################################################
#########          添加源文件所需的链接库文件路径，指定库文件搜索目录         ########## 
########################################################################################
link_directories(
    ${SRC_LIBS_PATH}
    ${LIBRARY_OUTPUT_PATH}
    ${LIBRARY_OUTPUT_PATH}/ffmpeg
    ${LIBRARY_OUTPUT_PATH}/SDL2/lib
    
    #/usr/local/SDL2/lib
    ${SRC_SDK_PATH}/debug
    ${SRC_SDK_PATH}/release    
) 



########################################################################################
#############           添加所要编译的源文件.                             ############## 
########################################################################################
list( APPEND myplayer_SRCS
    InitVersions.c 
    logEntry.c 
    
	SystemInit.c
    )

   
   
########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
add_subdirectory( Log )
add_subdirectory( Messages )
add_subdirectory( Monitors )
add_subdirectory( Threads )
add_subdirectory( Settings )
add_subdirectory( Utils )
add_subdirectory( Player )
add_subdirectory( Views )
add_subdirectory( Crypto )
add_subdirectory( Network )
add_subdirectory( Xmpp_IM )


MESSAGE("${TARGET_NAME} status.")



########################################################################################
#############          设置错误标志以便检查包含顺序.                      ############## 
########################################################################################
#GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
#GET_FILENAME_COMPONENT(_name ${_input} NAME)
#GET_FILENAME_COMPONENT(_path ${_input} PATH)
#SET(_IS_CURRENT_BINARY_DIR_BEFORE_INCLUDE FALSE)
#FOREACH(item ${_directory_flags})
    # 判断当前代码生成目录是否在头文件包含路径中首部, 如果不是则给出错误.
#    IF(${item} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT _IS_CURRENT_BINARY_DIR_BEFORE_INCLUDE )
#        SET(_IS_CURRENT_BINARY_DIR_BEFORE_INCLUDE TRUE)
#    ELSEIF(NOT _IS_CURRENT_BINARY_DIR_BEFORE_INCLUDE AND NOT ${item} STREQUAL ${CMAKE_CURRENT_BINARY_DIR}) 
#        MESSAGE(FATAL_ERROR  "This is the ADD_PRECOMPILED_HEADER macro. "  
#                "CMAKE_CURREN_BINARY_DIR has to mentioned at INCLUDE_DIRECTORIES's argument list before ${_path}, where ${_name} is located"
#                )
#    ENDIF(${item} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT _IS_CURRENT_BINARY_DIR_BEFORE_INCLUDE )

#    LIST(APPEND _compile_FLAGS "-I${item}")
#ENDFOREACH(item)



########################################################################################
#############              指定生成文件名称                               ############## 
########################################################################################
add_executable( ${TARGET_NAME} ${myplayer_SRCS} )



########################################################################################
#############         设置目标依赖关系，如果没有，则不需要设置            ############## 
########################################################################################
add_dependencies(${TARGET_NAME}     players     viewss 
                loglib  messagelib  threadlib   utilslib  monitorlib    settingslib
                jsoncpplib  ini-parserlib   sqlite3lib  ftpserverlib
                
                avcodec avformat    avutil  swscale     ##ffmpeg##
                SDL2main SDL2     ##SDL##
                )



########################################################################################
#############                 添加要链接的共享库                          ############## 
########################################################################################
target_link_libraries(${TARGET_NAME}    
    players
    viewss
    
    ## Components 组件 ## 
    loglib 
    -finstrument-functions functions_statisticslib
    messagelib
    threadlib
    utilslib
    monitorlib
    settingslib
    
    ## Modules 模块 ## 
    jsoncpplib
    ini-parserlib
    sqlite3lib
    ftpserverlib

    ## Decode 解码 ##
    avcodec
    avformat
    avutil
    swscale
    #avdevice
    #avfilter

    ## SDL 显示 ##
    SDL2main
    SDL2
    )
#link_libraries(${TARGET_NAME}  loglib -finstrument-functions functions_statisticslib)



########################################################################################
########          添加 编译完成之后执行的命令   只打包 所依赖的链接共享库       ######## 
########################################################################################
add_custom_command(TARGET ${TARGET_NAME} 
    POST_BUILD
	COMMAND sudo cp -arpf ${TARGET_NAME} ${DISTRIBUTE_PATH}/
	#COMMENT "cp ${TARGET_NAME} ${DISTRIBUTE_PATH}/${TARGET_NAME}"
    COMMAND sudo tar -zcvf ${DISTRIBUTE_PATH}/libs.tar.gz ${LIBRARY_OUTPUT_PATH}/
    COMMENT "cp ${LIBRARY_OUTPUT_PATH}/* ${DISTRIBUTE_PATH}/ "
	WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
	VERBATIM
)



########################################################################################
##############            编译完成之后 执行编译applications命令          ############### 
########################################################################################
add_subdirectory( Weather )
add_subdirectory( Astyle )



########################################################################################
#############                   安装可执行程序文件                        ############## 
########################################################################################
# 安装头文件和库文件到相应的目录
#install(TARGETS ${TARGET_NAME} ARCHIVE DESTINATION ${DISTRIBUTE_PATH}/debug)



########################################################################################
##########                添加测试程序  运行make test来执行测试               ########## 
########################################################################################
### 创建一个名为mytest的测试，传递的参数包括配置名，以及生成的可执行目标文件的完整路径
#add_test(NAME  mytest COMMAND  ${TARGET_NAME} --config $<CONFIGURATION> --exe $<TARGET_FILE:${TARGET_NAME}> )
#enable_testing()






