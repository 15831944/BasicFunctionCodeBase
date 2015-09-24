
########################################################################################
#############               生成 lodepng 库文件                           ############## 
########################################################################################
LIST (APPEND lodepng_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lodepng.cpp
    )
    

    
########################################################################################
#############               设置 lodepng 库版本号                         ############## 
########################################################################################
set(lodepng_LIB_VERSION     "1.0.0")
set(lodepng_LIB_SOVERSION   "1")


   
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Views
    ${PROJECT_SOURCE_DIR}/Views/PNG
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
IF (MODULE_lodepng)
    #生成动态库  静态库则 STATIC  
    add_library (lodepnglib SHARED      ${lodepng_LIB_SRCS})  
    add_library (lodepngs   STATIC      ${lodepng_LIB_SRCS})  
    
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(lodepnglib    PROPERTIES 
                                        VERSION     ${lodepng_LIB_VERSION} 
                                        SOVERSION   ${lodepng_LIB_SOVERSION} )
ELSE (MODULE_lodepng)
    MESSAGE(STATUS "Not Include lodepng module.")
ENDIF (MODULE_lodepng)



########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (TEST_MODULE_FLAG)
    LIST (APPEND lodepng_test_SRCS
        #${CMAKE_CURRENT_SOURCE_DIR}/test/example_decode.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_decode.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test/example_encode.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_encode.cpp
        
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_bmp2png.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_png2bmp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_optimize_png.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/example_png_info.cpp
        
        ${CMAKE_CURRENT_SOURCE_DIR}/test/lodepngTest
        )
        
    add_executable(TestLodePNG.elf    ${lodepng_test_SRCS})
    add_dependencies(TestLodePNG.elf      loglib  viewslib lodepnglib )
    target_link_libraries(TestLodePNG.elf loglib  viewslib lodepnglib )

ELSE (TEST_MODULE_FLAG)
    MESSAGE(STATUS "Not Include lodepng module.")
ENDIF (TEST_MODULE_FLAG)




