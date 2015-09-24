########################################################################################
#############              生成 thread 库文件                               ############## 
########################################################################################
LIST (APPEND thread_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/logThreads.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadMutex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadTask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadInterface.c
    )
MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")


########################################################################################
#############               设置 thread 库版本号                          ############## 
########################################################################################
set(thread_LIB_VERSION     "1.0.0")
set(thread_LIB_SOVERSION   "1")



########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Threads
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
IF (COMPONENT_thread)
    #生成动态库  静态库则 STATIC  
    add_library (threadlib  SHARED      ${thread_LIB_SRCS})  
    add_library (threads    STATIC      ${thread_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (threadlib  loglib  pthread)
    add_dependencies (threads    loglib  pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (threadlib  loglib pthread)
    target_link_libraries (threads    loglib pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(threadlib     PROPERTIES 
                                        VERSION     ${thread_LIB_VERSION} 
                                        SOVERSION   ${thread_LIB_SOVERSION} )
ELSE (COMPONENT_thread)
    MESSAGE(STATUS "Not Include thread module.")
ENDIF (COMPONENT_thread)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestThread.elf    threadTest.c)
    add_dependencies(TestThread.elf  threadlib   loglib     pthread)
    target_link_libraries(TestThread.elf loglib  threadlib  pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include Threads module.")
ENDIF (TEST_MODULE_FLAG)




