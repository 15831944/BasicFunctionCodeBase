########################################################################################
##########            生成 stackinfolib 库文件                    ########### 
########################################################################################
LIST (APPEND stackinfo_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/FunctionStack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessAuxiliaryInfo.c
    )


########################################################################################
#############               设置 stackinfolib 库版本号                    ############## 
########################################################################################
set(stackinfo_LIB_VERSION     "1.0.0")
set(stackinfo_LIB_SOVERSION   "1")



########################################################################################
##########            添加 编译库文件所需的头文件目录                        ########### 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Log   
    ${PROJECT_SOURCE_DIR}/Log/StackInfoDebug
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
IF (MODULE_stackinfo)
    #生成动态库  静态库则 STATIC  
    add_library (stackinfolib  SHARED         ${stackinfo_LIB_SRCS})  
    add_library (stackinfos    STATIC            ${stackinfo_LIB_SRCS})  
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(stackinfolib   PROPERTIES 
                            VERSION     ${stackinfo_LIB_VERSION} 
                            SOVERSION   ${stackinfo_LIB_SOVERSION} )

ELSE (MODULE_stackinfo)
    MESSAGE(STATUS "Not Include stackinfo module.")
ENDIF (MODULE_stackinfo)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestStackInfo.elf    stackinfoTest.c)
    add_dependencies(TestStackInfo.elf          stackinfolib)
    target_link_libraries(TestStackInfo.elf     stackinfolib)

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include jsoncpp module.")
ENDIF (TEST_MODULE_FLAG)



