
   
########################################################################################
##########            生成目标库文件 并 标明版本号                           ########### 
########################################################################################
IF (APPLICATIONS_astyle)
add_custom_command( TARGET ${TARGET_NAME} 
                    POST_BUILD
                    COMMAND /bin/sh cd ${CMAKE_CURRENT_SOURCE_DIR}/src 
                    COMMAND /bin/sh make clean && /bin/sh make
                    COMMENT "In ${CMAKE_CURRENT_SOURCE_DIR}/src, /bin/sh make "
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    VERBATIM
    )
MESSAGE("APPLICATIONS_astyle status.")
    
ELSE (APPLICATIONS_astyle)
    MESSAGE(STATUS "Not Include astyle module.")
ENDIF (APPLICATIONS_astyle)





########################################################################################
#############           添加 编译完成之后执行的命令                      ############## 
########################################################################################
#add_custom_command(TARGET ${NAME_ASTYLE} 
#    POST_BUILD
#	COMMAND /bin/sh make -f ${CMAKE_CURRENT_LIST_DIR}/Makefile
#	COMMENT "/bin/sh make -f ${CMAKE_CURRENT_LIST_DIR}/Makefile"
#	WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
#	VERBATIM
#	)

    