########################################################################################
#########                   根据平台判断所需的SDK以及库文件                    #########
#########    生成build_info.h头文件  准备在编译链接时所需的第三方库和头文件    ######### 
########################################################################################
add_custom_command(OUTPUT build_info.h
    PRE_BUILD
    COMMAND exec "${BUILD_SCRIPT_PATH}/make_version.sh"
    COMMAND exec ${BUILD_SCRIPT_PATH}/prepare_arch_libs.sh ${LIBRARY_OUTPUT_PATH} ${SRC_LIBS_PATH} ${SRC_INCLUDE_PATH} ${Module_ffmpeg_Version} ${Module_SDL2_Version} ${TARGET_SYSTEM}
    COMMENT "To making VERSION && preparing Arch Libs ... "
    VERBATIM
)
add_custom_target (build_info
    ALL DEPENDS build_info.h
    COMMENT "test build_info"
    VERBATIM
    )   

    
    
    
########################################################################################
###############              cmake用于测试的hello world 文件              ##############
########################################################################################
SET(RUN_HELLO_WORLD_COMMAND_FILE    "${CMAKE_BINARY_DIR}/run-hello-world.cmake")

FILE(WRITE   ${RUN_HELLO_WORLD_COMMAND_FILE}  "SET(ENV{LANG en})\n")
FILE(APPEND  ${RUN_HELLO_WORLD_COMMAND_FILE}  "EXECUTE_PROCESS(COMMAND \"./hello-world\" WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/bin\")\n")
FILE(APPEND  ${RUN_HELLO_WORLD_COMMAND_FILE}  "EXECUTE_PROCESS(COMMAND \"${GCOVR_EXECUTABLE}\" --html  -r \"${CMAKE_SOURCE_DIR}\" --output \"${COVERAGE_HTML_FILE}\" WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\")\n")

EXECUTE_PROCESS(COMMAND   hello-world   WORKING_DIRECTORY    ${CMAKE_BINARY_DIR})

ADD_CUSTOM_TARGET(run-hello-world
    COMMAND ${CMAKE_COMMAND} ARGS -P ${RUN_HELLO_WORLD_COMMAND_FILE}
    DEPENDS hello-world
    )

 
