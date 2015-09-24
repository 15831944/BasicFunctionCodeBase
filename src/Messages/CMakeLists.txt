########################################################################################
#############              生成 message 库文件                               ############## 
########################################################################################
LIST (APPEND message_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/logMessage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageQueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageType.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageInterface.c
    )



########################################################################################
#############               设置 message 库版本号                          ############## 
########################################################################################
set(message_LIB_VERSION     "1.0.0")
set(message_LIB_SOVERSION   "1")



########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Messages
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
IF (COMPONENT_message)
    #生成动态库  静态库则 STATIC  
    add_library (messagelib     SHARED      ${message_LIB_SRCS})  
    add_library (messages       STATIC        ${message_LIB_SRCS})  
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(messagelib    PROPERTIES 
                                        VERSION     ${message_LIB_VERSION} 
                                        SOVERSION   ${message_LIB_SOVERSION} )
ELSE (COMPONENT_message)
    MESSAGE(STATUS "Not Include message module.")
ENDIF (COMPONENT_message)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestMessage.elf  messageTest.c)
    add_dependencies(TestMessage.elf    messagelib  loglib)
    target_link_libraries(TestMessage.elf   loglib  messagelib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)




