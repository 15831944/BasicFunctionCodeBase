
########################################################################################
#############               生成 easybmp 库文件                           ############## 
########################################################################################
LIST (APPEND easybmp_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/EasyBMP.cpp
    )
    

    
########################################################################################
#############               设置 easybmp 库版本号                         ############## 
########################################################################################
set(easybmp_LIB_VERSION     "1.0.0")
set(easybmp_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Views
    ${PROJECT_SOURCE_DIR}/Views/BMP
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
IF (MODULE_easybmp)
    #生成动态库  静态库则 STATIC  
    add_library (easybmplib SHARED      ${easybmp_LIB_SRCS})  
    add_library (easybmps   STATIC      ${easybmp_LIB_SRCS})  
    
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(easybmplib    PROPERTIES 
                                        VERSION     ${easybmp_LIB_VERSION} 
                                        SOVERSION   ${easybmp_LIB_SOVERSION} )
ELSE (MODULE_easybmp)
    MESSAGE(STATUS "Not Include easybmp module.")
ENDIF (MODULE_easybmp)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestEasyBMP.elf    ./test/EasyBMPsample.cpp)
    add_dependencies(TestEasyBMP.elf      loglib  viewslib easybmplib )
    target_link_libraries(TestEasyBMP.elf loglib  viewslib easybmplib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include easybmp module.")
ENDIF (TEST_MODULE_FLAG)




