########################################################################################
######################              平台配置选项                    ####################
########################################################################################
set (TARGET_PLATFORM           "X86_32"                     CACHE STRING "Platform: X86_32, X86_64, arm, mips ...;")
set (TARGET_SYSTEM             "linux"                      CACHE STRING "OS: linux, android, windows, macos ...;")   

set (HOST_SYSTEM               ${CMAKE_SYSTEM}              CACHE STRING "host OS full name: Linux-2.6.32-33-generic, macos10X ...;")
set (HOST_SYSTEM_PROCESSOR     ${CMAKE_SYSTEM_PROCESSOR}    CACHE STRING "host system processor: i386, i586, i686, mips68k, arm9, arm11 ...;")

set (HOST_COMPILER_VERSION     "GCC${gcc_version}::G++${gcxx_version}"      CACHE STRING "GCC/G++ version;")
set (HOST_COMPILER_PCHSupport  ${PCHSupport_FOUND}                          CACHE STRING "host OS: linux,  unix,  windows,   macos;")






