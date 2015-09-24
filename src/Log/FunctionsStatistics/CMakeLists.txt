########################################################################################
##########            生成 functions_statisticslib 库文件                    ########### 
########################################################################################
LIST (APPEND functions_statistics_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/FunctionsStatistics.cpp
    )


########################################################################################
#############               设置 functions_statisticslib 库版本号                    ############## 
########################################################################################
set(functions_statistics_LIB_VERSION     "1.0.0")
set(functions_statistics_LIB_SOVERSION   "1")



########################################################################################
##########            添加 编译库文件所需的头文件目录                        ########### 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Log   
    ${PROJECT_SOURCE_DIR}/Log/FunctionsStatistics
)  
  
  
########################################################################################
##########            添加 链接库文件目录                                    ########### 
########################################################################################
link_directories(  
    /usr/local/lib   
    )  

    
########################################################################################
##########        生成目标库文件 添加所需要依赖库 设置依赖关系               ########### 
########################################################################################
IF (MODULE_functions_statistics)
    #生成动态库  静态库则 STATIC  
    add_library (functions_statisticslib SHARED         ${functions_statistics_LIB_SRCS})  
    add_library (functions_statistics STATIC            ${functions_statistics_LIB_SRCS})  
    
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (functions_statisticslib   pthread)
    add_dependencies (functions_statistics      pthread)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (functions_statisticslib  pthread)
    target_link_libraries (functions_statistics     pthread)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(functions_statisticslib   PROPERTIES 
                            VERSION     ${functions_statistics_LIB_VERSION} 
                            SOVERSION   ${functions_statistics_LIB_SOVERSION} )

ELSE (MODULE_functions_statistics)
    MESSAGE(STATUS "Not Include functions_statistics module.")
ENDIF (MODULE_functions_statistics)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(Testfunctions_statistics.elf    functions_statisticsTest.cpp)
    add_dependencies(Testfunctions_statistics.elf          functions_statisticslib )
    target_link_libraries(Testfunctions_statistics.elf     functions_statisticslib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)



