
########################################################################################
#############               生成 tinyjpeg 库文件                          ############## 
########################################################################################
LIST (APPEND tinyjpeg_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyjpeg.c
    )
    

    
########################################################################################
#############               设置 tinyjpeg 库版本号                        ############## 
########################################################################################
set(tinyjpeg_LIB_VERSION     "1.0.0")
set(tinyjpeg_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Views
    ${PROJECT_SOURCE_DIR}/Views/JPEG
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
IF (MODULE_tinyjpeg)
    #生成动态库  静态库则 STATIC  
    add_library (tinyjpeglib SHARED      ${tinyjpeg_LIB_SRCS})  
    add_library (tinyjpegs   STATIC      ${tinyjpeg_LIB_SRCS})  
    
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(tinyjpeglib   PROPERTIES 
                                        VERSION     ${tinyjpeg_LIB_VERSION} 
                                        SOVERSION   ${tinyjpeg_LIB_SOVERSION} )
ELSE (MODULE_tinyjpeg)
    MESSAGE(STATUS "Not Include tinyjpeg module.")
ENDIF (MODULE_tinyjpeg)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestTinyJPEG.elf    tinyjpegTest.c)
    add_dependencies(TestTinyJPEG.elf      loglib  viewslib tinyjpeglib )
    target_link_libraries(TestTinyJPEG.elf loglib  viewslib tinyjpeglib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include tinyjpeg module.")
ENDIF (TEST_MODULE_FLAG)




