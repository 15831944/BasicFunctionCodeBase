########################################################################################
#############              生成 threadpool 库文件                               ############## 
########################################################################################
LIST (APPEND threadpool_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool.c
    )



########################################################################################
#############               设置 threadpool 库版本号                          ############## 
########################################################################################
set(threadpool_LIB_VERSION     "1.0.0")
set(threadpool_LIB_SOVERSION   "1")



########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Threads
    ${PROJECT_SOURCE_DIR}/Threads/ThreadPool
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
IF (COMPONENT_threadpool)
    #生成动态库  静态库则 STATIC  
    add_library (threadpoollib SHARED   ${threadpool_LIB_SRCS})  
    add_library (threadpools STATIC     ${threadpool_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (threadpoollib   pthread)
    add_dependencies (threadpools     pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (threadpoollib  pthread)
    target_link_libraries (threadpools    pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(threadpoollib   PROPERTIES 
                            VERSION   ${threadpool_LIB_VERSION} 
                            SOVERSION ${threadpool_LIB_SOVERSION} )
ELSE (COMPONENT_threadpool)
    MESSAGE(STATUS "Not Include threadpool module.")
ENDIF (COMPONENT_threadpool)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestThreadPool-heavy.elf    heavyTest.c)
    add_dependencies(TestThreadPool-heavy.elf  threadpoollib     pthread)
    target_link_libraries(TestThreadPool-heavy.elf  threadpoollib  pthread)
    
    add_executable(TestThreadPool-shutdown.elf    shutdownTest.c)
    add_dependencies(TestThreadPool-shutdown.elf  threadpoollib     pthread)
    target_link_libraries(TestThreadPool-shutdown.elf  threadpoollib  pthread)
    
    add_executable(TestThreadPool-thrd.elf    thrdTest.c)
    add_dependencies(TestThreadPool-thrd.elf  threadpoollib     pthread)
    target_link_libraries(TestThreadPool-thrd.elf  threadpoollib  pthread)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include Threads module.")
ENDIF (TEST_MODULE_FLAG)




