
########################################################################################
#############                 生成目标库测试文件                          ############## 
########################################################################################
IF (APPLICATIONS_weather)
add_custom_command( TARGET ${TARGET_NAME} 
                    POST_BUILD
                    COMMAND /bin/sh cd ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND /bin/sh make clean && /bin/sh make
                    COMMENT "In ${CMAKE_CURRENT_SOURCE_DIR}, /bin/sh make "
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    VERBATIM
    )
MESSAGE("APPLICATIONS_weather status.")

ELSE (APPLICATIONS_weather)
    MESSAGE(STATUS "Not Include weather module.")
ENDIF (APPLICATIONS_weather)







    
    
    
    