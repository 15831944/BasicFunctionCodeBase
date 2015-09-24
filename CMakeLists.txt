#
# Main cmake script file for myplayer project.
#

#########################################################################################
#########         检查CMake版本,设置工程所需要的最低CMake版本                ############ 
#########################################################################################
cmake_minimum_required(VERSION 2.8.3)
# 使用CMAKE_POLICY命令来设置版本的兼容性，以避免一些不必要的警告信息。
#cmake_policy(VERSION 2.8)



#########################################################################################
##################         不去检查工具链是否正常                      ################## 
#########################################################################################
set (CMAKE_C_COMPILER_WORKS 1)
set (CMAKE_CXX_COMPILER_WORKS 1)



#########################################################################################
########   CMake 生成的 makefile 时把各步骤实际调用的命令、参数一一列出    ############## 
#########################################################################################
set (CMAKE_VERBOSE_MAKEFILE ON)



#########################################################################################
##################         检查 SVN 并获取SVN version                  ################## 
#########################################################################################
IF (EXISTS "${PROJECT_SOURCE_DIR}/.svn/") ## when SVN version >= 1.8, getting subversion failed.
    include(FindSubversion)
    find_package(Subversion)
    IF (Subversion_FOUND)
        Subversion_WC_INFO(${CMAKE_CURRENT_LIST_DIR} myplayer)
    ELSE (Subversion_FOUND)
        MESSAGE (FATAL_ERROR "Subversion not found.")
    ENDIF (Subversion_FOUND)
ELSE (EXISTS "${PROJECT_SOURCE_DIR}/.svn/")
    MESSAGE (STATUS "Subversion not found.")
ENDIF (EXISTS "${PROJECT_SOURCE_DIR}/.svn/")
IF(${myplayer_WC_REVISION})
    set (BUILD_Revision    ${myplayer_WC_REVISION}    CACHE STRING "software revision")
    MESSAGE (STATUS "BUILD_Revision from Subversion.")
ELSE(${myplayer_WC_REVISION})
    set (BUILD_Revision    "12134"                    CACHE STRING "software revision")
    MESSAGE (STATUS "BUILD_Revision set is 12134.")
ENDIF(${myplayer_WC_REVISION})



#########################################################################################
##################       判断gcc编译器版本 并判断是否支持预编译头      ################## 
#########################################################################################
set(CMAKE_COMPILER_IS_GNUCXX 1) 
IF(CMAKE_COMPILER_IS_GNUCXX )
    #获取GCC和G++编译器的版本号 
    EXEC_PROGRAM(
        gcc  ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
        OUTPUT_VARIABLE _gcc_compiler_output
        RETURN_VALUE _ret_value
    )
    STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"  gcc_version  ${_gcc_compiler_output})

    EXEC_PROGRAM(
        g++  ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
        OUTPUT_VARIABLE _gcxx_compiler_output
        RETURN_VALUE _ret_value
    )
    STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"  gcxx_version  ${_gcc_compiler_output})
    MESSAGE("GCC Version: ${gcc_version} ; G++ Version:${gcxx_version};")
    
    #判断GNU编译器是否支持预编译头
    IF(gcc_version MATCHES "4\\.[0-9](\\.[0-9])?")
        SET(PCHSupport_FOUND TRUE)
        MESSAGE("GCC Version: ${gcc_version}, PCHSupport_FOUND id TRUE")
    ELSE(gcc_version MATCHES "4\\.[0-9](\\.[0-9])?")
        IF(gcc_version MATCHES "3\\.4(\\.[0-9])?")
            SET(PCHSupport_FOUND TRUE)
            MESSAGE("GCC Version: ${gcc_version}, PCHSupport_FOUND id TRUE")
        ENDIF(gcc_version MATCHES "3\\.4(\\.[0-9])?")
    ENDIF(gcc_version MATCHES "4\\.[0-9](\\.[0-9])?")

ELSE(CMAKE_COMPILER_IS_GNUCXX )
    IF(WIN32)
        SET(PCHSupport_FOUND TRUE) # for experimental msvc support
        MESSAGE("MSVC for experimental msvc support, PCHSupport_FOUND id TRUE")
    ELSE(WIN32)
        SET(PCHSupport_FOUND FALSE)
        MESSAGE("OTHER for un-support, PCHSupport_FOUND id FALSE")
    ENDIF(WIN32)
    
ENDIF(CMAKE_COMPILER_IS_GNUCXX )



########################################################################################
#############                 项目定名以及目录                            ############## 
########################################################################################
project(myplayer CXX C)
set (EXECUTABLE_OUTPUT_PATH ${myplayer_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH    ${myplayer_BINARY_DIR}/libs)
set (PROJECT_TOP_DIR        ${CMAKE_CURRENT_LIST_DIR})
set (PROJECT_SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/src)



########################################################################################
#############                  设置项目的默认构建类型                     ############## 
########################################################################################
#   CMAKE_BUILD_TYPE类型	    对应的c编译选项变量	            对应的c++编译选项变量
#   None	                    CMAKE_C_FLAGS	                CMAKE_CXX_FLAGS
#   Debug                       CMAKE_C_FLAGS_DEBUG	            CMAKE_CXX_FLAGS_DEBUG
#   Release                     CMAKE_C_FLAGS_RELEASE           CMAKE_CXX_FLAGS_RELEASE
#   RelWithDebInfo              CMAKE_C_FLAGS_RELWITHDEBINFO	CMAKE_CXX_FLAGS_RELWITHDEBINFO
#   MinSizeRel                  CMAKE_C_FLAGS_MINSIZEREL        CMAKE_CXX_FLAGS_MINSIZEREL
set (CMAKE_BUILD_TYPE       None        CACHE STRING "project build type")  



########################################################################################
#########        set some PATHs of Source && Options for Compiler             ########## 
########################################################################################
set (BUILD_SCRIPT_PATH           ${CMAKE_CURRENT_SOURCE_DIR}/config/scripts)
set (BUILD_CMAKE_PATH            ${CMAKE_CURRENT_SOURCE_DIR}/config/cmake)
set (BUILD_TOOLS_PATH            ${CMAKE_CURRENT_SOURCE_DIR}/config/tools)

##set (SRC_SDK_PATH        ${CMAKE_CURRENT_SOURCE_DIR}/build/arch_sdk)
set (SRC_LIBS_PATH         ${CMAKE_CURRENT_SOURCE_DIR}/src/libs)
set (SRC_INCLUDE_PATH      ${CMAKE_CURRENT_SOURCE_DIR}/src/includes)

set (DISTRIBUTE_PATH       ${CMAKE_CURRENT_SOURCE_DIR}/dist)



########################################################################################
#########                  设置项目安装路径                                   ########## 
########################################################################################
# 设置程序的安装目录，优先级比cmake命令参数设置高
set (CMAKE_INSTALL_PREFIX   ${DISTRIBUTE_PATH}/debug)



########################################################################################
#########                    包含 CMAKE 脚本                                  ########## 
########################################################################################
include (${BUILD_CMAKE_PATH}/common_PlatformOptions.cmake)
include (${BUILD_CMAKE_PATH}/common_ModulesList.cmake)
include (${BUILD_CMAKE_PATH}/common_CompileOptions.cmake)
include (${BUILD_CMAKE_PATH}/common_SourceCodeConfig.cmake)



########################################################################################
##########                     添加项目代码子目录                            ########### 
########################################################################################
#link_directories ("This_is_test_link_directory")
#include (src/CMakeLists.txt)
add_subdirectory(src)






########################################################################################
###########           附加功能：                                            ############
###########                      判断是否进行 源代码的格式化                ############ 
###########           使用方法： 在ccmake完成后，执行 make format命令       ############ 
########################################################################################
#  增加一个总是会被构建的但没有输出的目标TAGET：astyle_formatting，
#  通过common_SourceCodeConfig.cmake文件中指定的ASTYLE_FORMATTING_FLAG标志判定是否执行；
IF (ASTYLE_FORMATTING_FLAG)
    add_custom_target( 
        format
        COMMAND exec "${BUILD_SCRIPT_PATH}/astyle_format.sh"
        COMMENT "test astyle_format.sh"
        VERBATIM
    )
    
ELSE (ASTYLE_FORMATTING_FLAG)
    MESSAGE (STATUS "ASTYLE_FORMATTING_FLAG is OFF.")
    
ENDIF (ASTYLE_FORMATTING_FLAG)






