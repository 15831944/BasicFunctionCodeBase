
########################################################################################
#############               生成 gif 库文件                           ############## 
########################################################################################
LIST (APPEND gif_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/decoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gif2bmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gifscan.c
    )
    

    
########################################################################################
#############               设置 gif 库版本号                         ############## 
########################################################################################
set(gif_LIB_VERSION     "1.0.0")
set(gif_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Views
    ${PROJECT_SOURCE_DIR}/Views/GIF
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
IF (MODULE_gif)
    #生成动态库  静态库则 STATIC  
    add_library (giflib SHARED      ${gif_LIB_SRCS})  
    add_library (gifs   STATIC      ${gif_LIB_SRCS})  
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(giflib    PROPERTIES 
                                    VERSION     ${gif_LIB_VERSION} 
                                    SOVERSION   ${gif_LIB_SOVERSION} )
ELSE (MODULE_gif)
    MESSAGE(STATUS "Not Include gif module.")
ENDIF (MODULE_gif)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    add_executable(TestGIF.elf    gifTest.c)
    add_dependencies(TestGIF.elf      loglib  viewslib giflib )
    target_link_libraries(TestGIF.elf loglib  viewslib giflib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include gif module.")
ENDIF (TEST_MODULE_FLAG)




