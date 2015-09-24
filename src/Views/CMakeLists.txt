########################################################################################
#############                生成 views 库文件                           ############## 
########################################################################################
LIST (APPEND views_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/LogViews.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewsInterface.c
    )
    


########################################################################################
#############               添加源码子目录                                ############## 
########################################################################################
#add_subdirectory( BMP )
#add_subdirectory( GIF )
#add_subdirectory( JPEG )
#add_subdirectory( PNG )

#MESSAGE("${CMAKE_CURRENT_SOURCE_DIR} status.")



########################################################################################
#############               设置 views 库版本号                          ############## 
########################################################################################
set(views_LIB_VERSION     "1.0.0")
set(views_LIB_SOVERSION   "1")



########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Views   
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
IF (COMPONENT_views)
    #生成动态库  静态库则 STATIC  
    add_library (viewslib     SHARED        ${views_LIB_SRCS})  
    add_library (viewss       STATIC       ${views_LIB_SRCS})  

    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    #add_dependencies(viewslib     pthread)
    #add_dependencies(viewss     pthread)
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    #target_link_libraries(viewslib pthread)
    #target_link_libraries(viewss pthread)

    # 设置版本号，SOVERSION为API版本号
    set_target_properties(viewslib  PROPERTIES 
                                    VERSION     ${views_LIB_VERSION}   
                                    SOVERSION   ${views_LIB_SOVERSION} )
ELSE (COMPONENT_views)
    MESSAGE(STATUS "Not Include COMPONENT_views module.")
ENDIF (COMPONENT_views)

