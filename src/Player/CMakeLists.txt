########################################################################################
#############                生成 player  库文件                          ############## 
########################################################################################
LIST (APPEND player_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/LogPlayer.c
    
#### BasicUtility
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicUtility/BasicPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicUtility/TestPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicUtility/UtilityTime.c
    
#### Codecs
    ${CMAKE_CURRENT_SOURCE_DIR}/Codecs/CodecUtilityInit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Codecs/SoftwareCodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Codecs/HardwareCodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Codecs/CodecUtilityInterfaces.c

    ${CMAKE_CURRENT_SOURCE_DIR}/AudioCtrl/AudioUtilityInterfaces.c
    
#### PlayList
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayList/MappingPlayList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayList/PlayListUtility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayList/PlayListUtilityInit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayList/PlayListUtilityInterfaces.cpp
    
#### PlayUtility
    ${CMAKE_CURRENT_SOURCE_DIR}/InitPlayer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayUtilityMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayUtility4UpperInterfaces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayUtilityHandler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/PlayUtilityInit.c

    ##${CMAKE_CURRENT_SOURCE_DIR}/Others/OutPutAVInfo.c
)
    


########################################################################################
#############               设置 player 库版本号                          ############## 
########################################################################################
set(player_LIB_VERSION     "1.0.0")
set(player_LIB_SOVERSION   "1")



########################################################################################
#############              添加头文件目录                                 ############## 
########################################################################################
include_directories(  
    ${PROJECT_SOURCE_DIR}/includes  
    ${PROJECT_SOURCE_DIR}/Player   
    ${PROJECT_SOURCE_DIR}/Player/Codecs
    ${PROJECT_SOURCE_DIR}/Player/PlayList
    ${PROJECT_SOURCE_DIR}/Player/BasicUtility  
    ${PROJECT_SOURCE_DIR}/Utils/ParserJsonCPP/json
    ${PROJECT_SOURCE_DIR}/Utils/ParserJsonCPP/include
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
IF (COMPONENT_player)
    #生成动态库  静态库则 STATIC  
    add_library (playerlib      SHARED          ${player_SRCS})  
    add_library (players        STATIC          ${player_SRCS})  
        
    # 设置依赖关系，如果当前库是最底层的共享库，则不需要设置
    add_dependencies (playerlib  loglib SDL2main SDL2 avcodec avformat avutil swscale threadlib)
    add_dependencies (players    loglib SDL2main SDL2 avcodec avformat avutil swscale threadlib)
    
    # 添加要链接的共享库, 库的顺序就是编译器的链接时顺序
    target_link_libraries (playerlib  loglib SDL2main SDL2 avcodec avformat avutil swscale threadlib)
    target_link_libraries (players    loglib SDL2main SDL2 avcodec avformat avutil swscale threadlib)
    
    # 设置版本号，SOVERSION为API版本号
    set_target_properties(playerlib     PROPERTIES 
                                        VERSION ${player_LIB_VERSION} 
                                        SOVERSION ${player_LIB_SOVERSION} 
    )
ELSE (COMPONENT_player)
    MESSAGE(STATUS "Not Include COMPONENT_player module.")
ENDIF (COMPONENT_player)
    



    
    
 