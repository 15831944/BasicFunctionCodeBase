########################################################################################
#############               生成 usb mount 库文件                            ############## 
########################################################################################
LIST (APPEND mountusb_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mountusb.c
    )
    
    
########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/utils
    ${PROJECT_SOURCE_DIR}/utils/MountUSB
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
IF (MODULE_mountusb)
    #生成动态库  静态库则 STATIC  
    add_library (mountusblib SHARED      ${mountusb_LIB_SRCS})  
    add_library (mountusbs STATIC        ${mountusb_LIB_SRCS})  
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(mountusblib   PROPERTIES VERSION 1.0.0 SOVERSION 1 )
ELSE (MODULE_mountusb)
    MESSAGE(STATUS "Not Include mountusb module.")
ENDIF (MODULE_mountusb)


