########################################################################################
############        根据目标系统和目标平台架构 选择合适的编译器选项        #############
########################################################################################
if (${TARGET_SYSTEM} STREQUAL "linux") 
    # 设置编译器选项  构建类型:CMAKE_BUILD_TYPE=None
    set( CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -g -Wall -O2 -fpic -fPIC -pipe -pg"    CACHE STRING " CMAKE_C_FLAGS.")  
    set( CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -g -Wall -O2 -fpic -fPIC -pipe -pg"  CACHE STRING " CMAKE_C_FLAGS .") 
    
    #构建类型:CMAKE_BUILD_TYPE=Debug
    set( CMAKE_C_FLAGS_DEBUG    "${CMAKE_C_FLAGS_DEBUG} -g -Wall -O2 -fpic -fPIC -pipe"     CACHE STRING " CMAKE_C_FLAGS_DEBUG.")
    set( CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -O2 -fpic -fPIC -pipe"   CACHE STRING " CMAKE_CXX_FLAGS_DEBUG.")
    
    #构建类型:CMAKE_BUILD_TYPE=Release
    set( CMAKE_C_FLAGS_RELEASE      "${CMAKE_C_FLAGS_RELEASE} -Wall -O2 -fpic -fPIC -pipe"      CACHE STRING " CMAKE_C_FLAGS_RELEASE.")
    set( CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2 -fpic -fPIC -pipe"    CACHE STRING " CMAKE_CXX_FLAGS_RELEASE.")
    
    #set(CMAKE_EXE_LINKER_FLAGS             " ${CMAKE_EXE_LINKER_FLAGS} -lpthread -finstrument-functions"   )
    #set(CMAKE_EXE_LINKER_FLAGS_RELEASE     "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -lpthread -finstrument-functions"    )
    
    if (${TARGET_PLATFORM} STREQUAL "arm")
        LIST (APPEND PLATFORM_DEFINITIONS
            "-D_FILE_OFFSET_BITS=64"
            "-D_LARGEFILE_SOURCE"
            "-D_LARGEFILE64_SOURCE"
            "-D_THREAD_SAFE"
            "-march=armv7-a"
            "-mcpu=cortex-a9"
            "-mfloat-abi=softfp"
            "-mfpu=vfpv3-d16"
            "-Wmissing-prototypes"
        )
        
    else (${TARGET_PLATFORM} STREQUAL "arm")
        if (${TARGET_PLATFORM} STREQUAL "mips")
            LIST (APPEND PLATFORM_DEFINITIONS
                "-D_FILE_OFFSET_BITS=64"
                "-D_LARGEFILE_SOURCE"
                "-D_LARGEFILE64_SOURCE"
                "-D_THREAD_SAFE"
                "-Wmissing-prototypes"
            )
        else (${TARGET_PLATFORM} STREQUAL "mips")
            MESSAGE(STATUS "")
        endif (${TARGET_PLATFORM} STREQUAL "mips")
    endif (${TARGET_PLATFORM} STREQUAL "arm")
    
elseif (${TARGET_SYSTEM} STREQUAL "android")
    #编译器选项  构建类型:CMAKE_BUILD_TYPE=None
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2 -fpic -fPIC -pipe")  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2 -fpic -fPIC -pipe") 
    
    #构建类型:CMAKE_BUILD_TYPE=Debug
    set( CMAKE_C_FLAGS_DEBUG    "${CMAKE_C_FLAGS_DEBUG}" )
    set( CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG}" )
    
    #构建类型:CMAKE_BUILD_TYPE=Release
    set( CMAKE_C_FLAGS_RELEASE      "${CMAKE_C_FLAGS_RELEASE}" )
    set( CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE}" )
    
    LIST (APPEND PLATFORM_DEFINITIONS
        "-DANDROID"
        "-fno-short-enums"
        "-fvisibility=default"
        "-ffast-math"
        "-fno-strict-aliasing"
        "-O2"
        "-fPIC"
        "-fno-exceptions"
    )

elseif (${TARGET_SYSTEM} STREQUAL "windows")
    #编译器选项  构建类型:CMAKE_BUILD_TYPE=None
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall ")  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall") 

elseif (${TARGET_SYSTEM} STREQUAL "macos")
    #编译器选项  构建类型:CMAKE_BUILD_TYPE=None
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall ")  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall") 
    
endif (${TARGET_SYSTEM} STREQUAL "linux")



